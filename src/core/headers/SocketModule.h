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

        ClientSocket accept();
    };
}
#endif