#ifndef SOCKET_MODULE_H
#define SOCKET_MODULE_H
#include <sys/socket.h>
#include <stdexcept> 
#include <netinet/in.h>
#include <unistd.h>
#include "HttpModule.h"
#include "PresentationModule.h"
#include <sstream>

namespace Core {
        class ClientSocket{
        private:
        int fd;

        public:
        ClientSocket(int);
        ~ClientSocket();

        ClientSocket(const ClientSocket&) = delete;
        ClientSocket& operator=(const ClientSocket&) = delete;
        ClientSocket(ClientSocket&&) = default;
        ClientSocket& operator=(ClientSocket&&) = default;

        HttpRequest recv();
        void write(HttpResponse);
    };
    
    class WebSocket{
        private:
        int fd, port, backlog;
        void create();
        void configure();
        void bind();
        void listen();

        public:
        WebSocket(int,int);
        ~WebSocket();

        WebSocket(const WebSocket&) = delete;
        WebSocket& operator=(const WebSocket&) = delete;
        WebSocket(WebSocket&&) = default;
        WebSocket& operator=(WebSocket&&) = default;

        ClientSocket accept();
    };
}
#endif