#include "SocketModule.h"
namespace Core {
    void WebSocket::create(){
        // create socket
        // see docs: https://man7.org/linux/man-pages/man2/socket.2.html
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1) {
            SSL_CTX_free(ssl_ctx);
            throw std::runtime_error("Failed to create socket");
        }
    };
    void WebSocket::configure(){
        // set socket options
        // see docs: https://man7.org/linux/man-pages/man7/socket.7.html    
        int opt = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1 ||
            setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) == -1) {
            SSL_CTX_free(ssl_ctx);
            throw std::runtime_error("Failed to configure socket");
        }
    }
    void WebSocket::bind(){
        // bind socket to port
        // docs: https://man7.org/linux/man-pages/man2/bind.2.html
        sockaddr_in socketAddress{};
        socketAddress.sin_family = AF_INET;
        socketAddress.sin_addr.s_addr = INADDR_ANY;
        socketAddress.sin_port = htons(port);
        if (::bind(fd, (sockaddr*)&socketAddress, sizeof(socketAddress)) == -1){
            SSL_CTX_free(ssl_ctx);
            throw std::runtime_error("Failed to bind socket");
        }
    };
    void WebSocket::listen(){
        // set socket in listening mode
        // see docs: https://man7.org/linux/man-pages/man2/listen.2.html
        if (::listen(fd, backlog) == -1) {
            SSL_CTX_free(ssl_ctx);
            throw std::runtime_error("Failed to listen on socket");
        }
    };
    ClientSocket WebSocket::accept(){
        // accept incoming client connections
        // see docs: https://man7.org/linux/man-pages/man2/accept.2.html
        sockaddr_in peerAddress{};
        socklen_t peerLen = sizeof(peerAddress);
        int cfd = ::accept(fd, (sockaddr*)&peerAddress, &peerLen);
        if (cfd == -1) {
            throw std::runtime_error("Failed to accept connection");
        }
        return ClientSocket(cfd, this->ssl_ctx);
    };

    void WebSocket::configureSSL(){
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms(); 
        ssl_ctx = SSL_CTX_new(TLS_server_method());

        if (!ssl_ctx) {
            throw std::runtime_error("Failed to create SSL context");
        }
        if (SSL_CTX_use_certificate_file(ssl_ctx, "server.crt", SSL_FILETYPE_PEM) <= 0) {
            SSL_CTX_free(ssl_ctx); 
            throw std::runtime_error("Failed to load server certificate");
        }
        if (SSL_CTX_use_PrivateKey_file(ssl_ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
            SSL_CTX_free(ssl_ctx);
            throw std::runtime_error("Failed to load server private key");
        }
        if (!SSL_CTX_check_private_key(ssl_ctx)) {
            SSL_CTX_free(ssl_ctx);
            throw std::runtime_error("Private key does not match certificate");
        }
        
        SSL_CTX_set_options(ssl_ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);
        SSL_CTX_set_cipher_list(ssl_ctx, "ECDHE+AESGCM:ECDHE+CHACHA20:DHE+AESGCM:DHE+CHACHA20:!aNULL:!MD5:!DSS");
    }

    WebSocket::WebSocket(int port, int backlog){
        this->port = port;
        this->backlog = backlog;
        configureSSL();
        create();
        configure();
        bind();
        listen();
    };

    WebSocket::~WebSocket(){
        if(fd != -1){
            close(fd);
        }
        if(ssl_ctx){
            SSL_CTX_free(ssl_ctx);
        }
        EVP_cleanup();
    }

    ClientSocket::ClientSocket(int fd, SSL_CTX* ctx){
        this->fd = fd;
        configureSSL(ctx);
    }
    ClientSocket::~ClientSocket(){
        if(fd != -1){
            close(fd);
        }
        if(ssl){
            SSL_shutdown(ssl);
            SSL_free(ssl);
        }
    }
    void ClientSocket::configureSSL(SSL_CTX* ctx){
        this->ssl = SSL_new(ctx);
        if (!ssl) {
            throw std::runtime_error("Failed to create SSL object");
        }
        if (SSL_set_fd(ssl, fd) != 1) {
            SSL_free(ssl);
            throw std::runtime_error("Failed to set SSL file descriptor");
        }
        if(!SSL_accept(ssl)){
            SSL_free(ssl);
            throw std::runtime_error("Failed to complete SSL handshake");
        }
    }
    HttpRequest ClientSocket::recv(){
        /*
            See HTTP1.1 Message Syntax: https://datatracker.ietf.org/doc/html/rfc7230#section-3
            "...The normal procedure for parsing an HTTP message is to read the
            start-line into a structure, read each header field into a hash table
            by field name until the empty line, and then use the parsed data to
            determine if a message body is expected.  If a message body has been
            indicated, then it is read as a stream until an amount of octets
            equal to the message body length is read or the connection is closed."
        */
        Core::HttpRequest httpRequest;
        std::string http11 = "HTTP/1.1";
        char buffer[4096];
        std::string raw;
        ssize_t result;
        std::string termination = "\r\n\r\n";

        // Read until header termination
        while (raw.find(termination) == std::string::npos) {
            result = SSL_read(ssl, buffer, sizeof(buffer));
            if (result == -1) {
                return httpRequest;
            }
            if (result == 0) { 
                return httpRequest;
            }
            raw.append(buffer, result);
            if (raw.size() > 8192) {
                return httpRequest;
            }
        }
        // Parse Start Line: method and path
        size_t startLineEnd = raw.find("\r\n");
        if(startLineEnd == std::string::npos){
            return httpRequest;
        }
        std::string startLine = raw.substr(0, startLineEnd);
        std::stringstream startLineStream(startLine);
        std::string method, path, version;
        if (!(startLineStream >> method >> path >> version)) {
            return httpRequest;
        }
        if (method == "GET") {
            httpRequest.method = Core::GET;
        } else if (method == "POST") {
            httpRequest.method = Core::POST;
        } else if (method == "DELETE") {
            httpRequest.method = Core::DELETE_;
        } else {
            return httpRequest;
        }
        httpRequest.path = path;
        
        // Parse query parameters
        size_t queryParamIndex = httpRequest.path.find("?");
        if(queryParamIndex != std::string::npos){
            httpRequest.parameters = std::unordered_map<std::string,std::string>();
            std::string temp = httpRequest.path;
            httpRequest.path = temp.substr(0, queryParamIndex);
            std::string queryParamsSegment = temp.substr(queryParamIndex+1);
            std::stringstream ss(queryParamsSegment);
            std::string token;
            while(std::getline(ss, token, '&')){
                size_t eqPos = token.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = token.substr(0, eqPos);
                    std::string value = token.substr(eqPos + 1);
                    httpRequest.parameters.insert_or_assign(key, value);
                }
            }
        }

        // Parse Headers
        size_t headerStart = startLineEnd + 2;
        size_t headersEnd = raw.find(termination);
        std::string headerSection = raw.substr(headerStart, headersEnd - headerStart);
        std::stringstream ss(headerSection);
        std::string line;
        httpRequest.headers = std::unordered_map<std::string,std::string>();
        while(std::getline(ss, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 2);
                httpRequest.headers.insert_or_assign(key, value);
            }
        }

        // Read Full Content-Length Body
        size_t bodyStart = headersEnd + termination.size();
        std::string body = raw.substr(bodyStart);
        if (httpRequest.headers.find("Content-Length") != httpRequest.headers.end()) {
            int expectedSize = 0;
            try{
                expectedSize = std::stoi(httpRequest.headers["Content-Length"]);
            }catch(const std::exception& e){
                return httpRequest;
            }
            while (body.size() < expectedSize) {
                result = SSL_read(ssl, buffer, sizeof(buffer));
                if (result == -1) {
                    return httpRequest;
                }
                if (result == 0) { 
                    return httpRequest;
                }
                raw.append(buffer, result);
                body = raw.substr(bodyStart);
            }
            body = body.substr(0, expectedSize);
        }
        
        httpRequest.body = body;
        if(httpRequest.headers.find("Content-Type") != httpRequest.headers.end() && httpRequest.headers["Content-Type"] == "application/json"){
            httpRequest.json = JsonUtils::deserialize(body);
        }
        return httpRequest;
    }

    void ClientSocket::write(HttpResponse res){
        // write back
        // see docs: https://man7.org/linux/man-pages/man2/write.2.html
        auto it = res.headers.find("Content-Type");
        std::stringstream stream;
        if(it != res.headers.end()){
            if(it->second == "application/json"){
                std::string serialized = JsonUtils::serialize(res.body);
                stream << "HTTP/1.1 " << res.statusCode << " " << HttpResponse::getStatusStr(res.statusCode) << "\r\n";
                stream << "Content-Type:" << it->second << "\r\nContent-Length: " << serialized.size() <<"\r\n\r\n";
                stream << serialized;
                SSL_write(ssl, stream.str().c_str(), stream.str().size());
            }
            else if(it->second == "text/html" || it->second == "text/css" || it->second == "application/javascript"){
                stream << "HTTP/1.1 " << res.statusCode << " " << HttpResponse::getStatusStr(res.statusCode) << "\r\n";
                stream << "Content-Type:" << it->second << "\r\nContent-Length: " << res.text.size() <<"\r\n\r\n";
                stream << res.text;
                SSL_write(ssl, stream.str().c_str(), stream.str().size());
            }
        }else{
            stream << "HTTP/1.1 " << res.statusCode << " " << HttpResponse::getStatusStr(res.statusCode) << "\r\n";
            stream << "Content-Type:text/plain" << "\r\nContent-Length:" << res.text.size() <<"\r\n\r\n";
            stream << res.text;
            SSL_write(ssl, stream.str().c_str(), stream.str().size());
        }
    }
}