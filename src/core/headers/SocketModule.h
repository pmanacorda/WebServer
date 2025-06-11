#ifndef SOCKET_MODULE_H
#define SOCKET_MODULE_H
#include <sys/socket.h>
#include <stdexcept> 
#include <netinet/in.h>
#include <unistd.h>
#include "HttpModule.h"
#include "PresentationModule.h"
#include <sstream>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>

namespace Core {
        class ClientSocket{
        private:
        void configureSSL(SSL_CTX*);

        protected:
        int fd;
        SSL* ssl;

        public:
        ClientSocket(int, SSL_CTX*);
        ~ClientSocket();

        ClientSocket(const ClientSocket&) = delete;
        ClientSocket& operator=(const ClientSocket&) = delete;

        ClientSocket(ClientSocket&& other) noexcept {
            fd = other.fd;
            ssl = other.ssl;
            other.fd = -1;
            other.ssl = nullptr;
        }
        
        ClientSocket& operator=(ClientSocket&& other) noexcept {
            if(this != &other){
                if(fd != -1){
                    close(fd);
                }
                if(ssl){
                    SSL_shutdown(ssl);
                    SSL_free(ssl);
                }
                
                fd = other.fd;
                ssl = other.ssl;
                other.fd = -1;
                other.ssl = nullptr;
            }
            return *this;
        }

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
        void configureSSL();
        SSL_CTX *ssl_ctx;

        public:
        WebSocket(int,int);
        ~WebSocket();

        WebSocket(const WebSocket&) = delete;
        WebSocket& operator=(const WebSocket&) = delete;
        WebSocket(WebSocket&&) = delete;
        WebSocket& operator=(WebSocket&&) = delete;

        ClientSocket accept();
    };
}
#endif