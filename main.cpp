/* AUTHOR: Patrick Manacorda */
#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <any>
#include "HttpModule.h"
#include "SocketModule.h"
#include "PresentationModule.h"
#include "BaseController.h"
#include "AuthService.h"
#include <typeindex>

auto buildServices(){
    std::unordered_map<std::type_index, std::any> services;
    auto authService = std::make_shared<Services::AuthService>();
    services[std::type_index(typeid(Services::AuthService))] = authService;
    return services;
}
inline auto services = buildServices();

auto buildRoutes(std::unordered_map<std::type_index, std::any> services) {
    std::unordered_map<std::string, std::shared_ptr<Controllers::BaseController>> map;

    auto loginController = std::make_shared<Controllers::LoginController>(services[std::type_index(typeid(Services::AuthService))]);
    map["/api/login"] = loginController;
    map["/api/logout"] = loginController;

    auto indexController = std::make_shared<Controllers::IndexController>();
    map["/index.html"] = indexController;
    map["/index.css"] = indexController;
    map["/index.js"] = indexController;
    map["/favicon.ico"] = indexController;

    auto aboutController = std::make_shared<Controllers::AboutController>();
    map["/about.html"] = aboutController;
    map["/about.css"] = aboutController;
    map["/about.js"] = aboutController;
    map["/api/about"] = aboutController;

    return map;
}


inline auto routes = buildRoutes(services);
inline std::atomic<uint16_t> thread_count{0};
inline std::vector<std::string> protectedRoutes = {"/about.html", "/about.css", "/about.js", "/api/about"};

void handle(Core::ClientSocket clientSocket) {
    try {
        bool keepAlive = true;
        while (keepAlive) {
            Core::HttpRequest request = clientSocket.recv();
            Core::HttpResponse response;
            
            if (request.path.empty()) {
                response.headers["Connection"] = "Close";
                response.statusCode = 499;
                clientSocket.write(response);
                break;
            }
            
            if (request.path == "/") {
                request.path = "/index.html";
            }

            if (std::find(protectedRoutes.begin(), protectedRoutes.end(), request.path) != protectedRoutes.end()) {
                auto authService = std::any_cast<std::shared_ptr<Services::AuthService>>(services[std::type_index(typeid(Services::AuthService))]);
                if (!authService->isAuthenticated(request)) {
                    response.statusCode = 401;
                    response.headers["Connection"] = "Close";
                    clientSocket.write(response);
                    break;
                }
            }
            
            if (routes.contains(request.path)) {
                routes[request.path]->Run(request, response);
            } else {
                response.statusCode = 404;
            }
            
            if (auto it = request.headers.find("Connection"); it != request.headers.end()) {
                keepAlive = (it->second == "Keep-Alive") || (it->second == "keep-alive");
            } else { 
                keepAlive = false; 
            }

            response.headers["Connection"] = keepAlive ? "Keep-Alive" : "Close";
            if (keepAlive) {
                response.headers["Keep-Alive"] = "timeout=30, max=100";
            }
            
            clientSocket.write(response);
        }
    } catch(...) { }
    
    thread_count.fetch_sub(1);
}

int main() {
    Core::Listener listener(443, 10);
    
    while (true) {
        try {
            Core::ClientSocket csocket = listener.accept();
            
            constexpr uint16_t MAX_THREADS = 100;
            
            if (thread_count.load() < MAX_THREADS) {
                thread_count.fetch_add(1);
                std::thread(handle, std::move(csocket)).detach();
            } else {
                Core::HttpResponse response;
                response.statusCode = 503;
                response.text = "Service temporarily unavailable";
                csocket.write(response);
            }
        } catch (...) { 
            continue; 
        }
    }
    return 1;
}