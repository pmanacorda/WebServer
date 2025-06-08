/* AUTHOR: Patrick Manacorda */
#include <iostream>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <cstring>

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
        exit(-1);
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
    socketAddress.sin_port = htons(80);
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
        exit(1);
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
    }
    return 1;
}