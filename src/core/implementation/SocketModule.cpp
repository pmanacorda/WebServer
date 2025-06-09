#include "SocketModule.h"

namespace Core {
    void WebSocket::create(){
        // create socket
        // see docs: https://man7.org/linux/man-pages/man2/socket.2.html
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1) {
            throw std::runtime_error("Failed to create socket");
        }
    };
    void WebSocket::configure(){
        // set socket options
        // see docs: https://man7.org/linux/man-pages/man7/socket.7.html    
        int opt = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1 ||
            setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) == -1) {
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
            throw std::runtime_error("Failed to bind socket");
        }
    };
    void WebSocket::listen(){
        // set socket in listening mode
        // see docs: https://man7.org/linux/man-pages/man2/listen.2.html
        if (::listen(fd, backlog) == -1) {
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
        return ClientSocket(fd);
    };

    WebSocket::WebSocket(int port, int backlog){
        port = port;
        backlog = backlog;
        create();
        configure();
        listen();
        bind();
    };

    WebSocket::~WebSocket(){
        if(fd != -1){
            close(fd);
        }
    }

    ClientSocket::ClientSocket(int fd){
        fd = fd;
    }
    ClientSocket::~ClientSocket(){
        close(fd);
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
            result = read(fd, buffer, sizeof(buffer));
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
                result = read(fd, buffer, sizeof(buffer));
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
        std::string serialized = JsonUtils::serialize(res.body);
        std::stringstream stream;
        stream << "HTTP/1.1 " << res.statusCode << " " << HttpResponse::getStatusStr(res.statusCode) << "\r\n";
        stream << "Content-Type:application/json\r\nContent-Length: " << serialized.size() <<"\r\n\r\n";
        stream << serialized;
        ::write(fd, stream.str().c_str(), stream.str().size());
    }
}