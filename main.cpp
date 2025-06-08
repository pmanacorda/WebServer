/* AUTHOR: Patrick Manacorda */
#include <iostream>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

/*
    See: https://www.json.org/json-en.html
    This parser focuses on flat objects only
*/
std::unordered_map<std::string, std::string> parseJson(std::string raw){
    std::unordered_map<std::string, std::string> res;
    std::string key = "";
    std::string value = "";
    char buffer[100];
    int buffIndex = 0;
    for(int i=0; i<raw.size(); i++){
        switch(raw[i]){
            case '{':
            case ' ':
            case ':':
                break;
            case '"':
            case ',':
            case '}':
                if(buffIndex > 0){
                    if(key.size() == 0){
                        key = std::string(buffer, buffIndex);
                    }else{
                        value = std::string(buffer, buffIndex);;
                        res.insert_or_assign(key, value);
                        key = "";
                        value = "";
                    }
                    buffIndex = 0;
                }
                break;
            default:
                buffer[buffIndex++]=raw[i];
                break;
        }
    }
    return res;
}

std::string serializeJson(std::unordered_map<std::string,std::string> input){
    std::string output = "{";
    for(auto it = input.begin(); it != input.end(); it++){
        std::stringstream kvp;
        kvp << '"' << it->first << '"' << ':' << '"' << it->second << '"';
        output.append(kvp.str());
    }
    output += '}';
    return output;
}

/*
    See HTTP1.1 Message Syntax: https://datatracker.ietf.org/doc/html/rfc7230#section-3
    "...The normal procedure for parsing an HTTP message is to read the
    start-line into a structure, read each header field into a hash table
    by field name until the empty line, and then use the parsed data to
    determine if a message body is expected.  If a message body has been
    indicated, then it is read as a stream until an amount of octets
    equal to the message body length is read or the connection is closed."
*/
enum HttpMethod{
    GET,
    POST,
    DELETE
};
struct HttpRequest{
    HttpMethod method = GET;
    std::string path;
    std::string body;
    std::unordered_map<std::string,std::string> json;
    std::unordered_map<std::string,std::string> headers;
    std::unordered_map<std::string,std::string> parameters;
    HttpRequest() : method(GET), path(""), body("") {}
};
HttpRequest recv(int clientSocket) {
    HttpRequest httpRequest;
    std::string http11 = "HTTP/1.1";
    char buffer[4096];
    std::string raw;
    ssize_t result;
    std::string termination = "\r\n\r\n";

    // Read until header termination
    while (raw.find(termination) == std::string::npos) {
        result = read(clientSocket, buffer, sizeof(buffer));
        if (result == -1) {
            std::cerr << "ERROR - socket read failed: " << strerror(errno) << std::endl;
            return httpRequest;
        }
        if (result == 0) { 
            std::cerr << "ERROR - connection closed unexpectedly" << std::endl;
            return httpRequest;
        }
        raw.append(buffer, result);
        if (raw.size() > 8192) {
            std::cerr << "ERROR - headers too large\n";
            return httpRequest;
        }
    }
    // Parse Start Line: method and path
    size_t startLineEnd = raw.find("\r\n");
    if(startLineEnd == std::string::npos){
        std::cerr << "ERROR - Invalid HTTP format: no start line terminator" << std::endl;
        return httpRequest;
    }
    std::string startLine = raw.substr(0, startLineEnd);
    std::stringstream startLineStream(startLine);
    std::string method, path, version;
    if (!(startLineStream >> method >> path >> version)) {
        std::cerr << "ERROR - Invalid start line format" << std::endl;
        return httpRequest;
    }
    if (method == "GET") {
        httpRequest.method = GET;
    } else if (method == "POST") {
        httpRequest.method = POST;
    } else if (method == "DELETE") {
        httpRequest.method = DELETE;
    } else {
        std::cerr << "ERROR - Unsupported HTTP method: " << method << std::endl;
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
            std::cout << "ERROR - Content-Length parsing error " << e.what() << std::endl;
            return httpRequest;
        }
        while (body.size() < expectedSize) {
            result = read(clientSocket, buffer, sizeof(buffer));
            if (result == -1) {
                std::cerr << "ERROR - socket read failed: " << strerror(errno) << std::endl;
                return httpRequest;
            }
            if (result == 0) { 
                std::cerr << "ERROR - connection closed unexpectedly" << std::endl;
                return httpRequest;
            }
            raw.append(buffer, result);
            body = raw.substr(bodyStart);
        }
        body = body.substr(0, expectedSize);
    }

    std::cout << "INFO - successfully read full request" << std::endl;
    
    httpRequest.body = body;
    if(httpRequest.headers.find("Content-Type") != httpRequest.headers.end() && httpRequest.headers["Content-Type"] == "application/json"){
        httpRequest.json = parseJson(body);
    }
    return httpRequest;
}

struct HttpResponse{
    int statusCode = 200;
    std::unordered_map<std::string,std::string> body;
};

void router(HttpRequest &request, HttpResponse &response){
    std::unordered_map<std::string, std::function<void(HttpRequest&, HttpResponse&)>> controllers;
    controllers["/api/test"] = [](HttpRequest& req, HttpResponse& res) {
        std::cout << "Test controller triggered" << std::endl;
        res.body.insert_or_assign("Data", "Result");
        res.statusCode = 500;
    };
    if(controllers.find(request.path) != controllers.end()){
        controllers[request.path](request, response);
    }
}


void handle(int clientSocket){
    try{
        // receive content stream
        HttpRequest request = recv(clientSocket);
        HttpResponse response;
        router(request, response);
        // write ack back
        // see docs: https://man7.org/linux/man-pages/man2/write.2.html
        std::string serialized = serializeJson(response.body);
        std::unordered_map<int, std::string> statusCodes;
        statusCodes[200] = "OK";
        statusCodes[500] = "Internal Server Error";
        std::stringstream res;
        res << "HTTP/1.1 " << response.statusCode << " " << statusCodes[response.statusCode] << "\r\n";
        res << "Content-Type:application/json\r\nContent-Length: " << serialized.size() <<"\r\n\r\n";
        res << serialized;
        //"HTTP/1.1 200 OK\r\nContent-Type:application/json\r\nContent-Length: 3\r\n\r\nACK";
        write(clientSocket, res.str().c_str(), res.str().size());
    }catch(const std::exception& e){
        std::cout << "ERROR - " << e.what() << std::endl;
    }

    // Close Connection
    close(clientSocket);
}

int main(){
    std::cout << "INFO - Starting..." << std::endl;

    // create socket
    // see docs: https://man7.org/linux/man-pages/man2/socket.2.html
    int ipv4domain = AF_INET;
    int tcpConnections = SOCK_STREAM;
    int singleProtocol = 0;
    int socketFileDescriptor = socket(ipv4domain, tcpConnections, singleProtocol);
    if (socketFileDescriptor == -1){
        std::cerr << "FATAL - socket creation failed " << strerror(errno) << std::endl;
        exit(1);
    }
    // set socket options
    // see docs: https://man7.org/linux/man-pages/man7/socket.7.html
    int reuseAddress = SO_REUSEADDR;
    int keepAlive = SO_KEEPALIVE;
    int opt = 1;
    setsockopt(socketFileDescriptor, SOL_SOCKET, reuseAddress, &opt, sizeof(opt));
    setsockopt(socketFileDescriptor, SOL_SOCKET, keepAlive, &opt, sizeof(opt));
    
    // bind socket to port
    // docs: https://man7.org/linux/man-pages/man2/bind.2.html
    sockaddr_in socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = INADDR_ANY;
    socketAddress.sin_port = htons(8080);
    int bindResult = bind(socketFileDescriptor, (sockaddr*)&socketAddress, sizeof(socketAddress));
    if (bindResult == -1){
        std::cerr << "FATAL - socket binding failed " << strerror(errno) << std::endl;
        exit(-1);
    }

    // set socket in listening mode
    // see docs: https://man7.org/linux/man-pages/man2/listen.2.html
    int backlogConnectionQueue = 10;
    int listenResult = listen(socketFileDescriptor, backlogConnectionQueue);
    if (listenResult == -1){
        std::cerr << "FATAL - socket listening failed " << strerror(errno) << std::endl;
        exit(-1);
    }
    
    // accept incoming client connections
    // see docs: https://man7.org/linux/man-pages/man2/accept.2.html
    while(true){
        sockaddr_in peerAddress;
        socklen_t peerLen = sizeof(peerAddress);
        int clientSocket = accept(socketFileDescriptor, (sockaddr*)&peerAddress, &peerLen);
        if(clientSocket == -1){
            std::cout << "INFO - socket accept failed " << strerror(errno) << std::endl;
            continue;
        }
        std::cout << "CONNECTION RECEIVED WITH SOCKET " << clientSocket << std::endl;
        handle(clientSocket);
    }
    return 1;
}