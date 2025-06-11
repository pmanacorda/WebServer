/* AUTHOR: Patrick Manacorda */
#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include "HttpModule.h"
#include "SocketModule.h"
#include "PresentationModule.h"
#include "BaseController.h"

std::unordered_map<std::string, std::shared_ptr<Controllers::BaseController>> buildRoutes() {
    std::unordered_map<std::string, std::shared_ptr<Controllers::BaseController>> map;
    map["/api/test"] = std::make_shared<Controllers::TestController>();

    auto indexController = std::make_shared<Controllers::IndexController>();
    map["/index.html"] = indexController;
    map["/index.css"] = indexController;
    map["/index.js"] = indexController;

    return map;
}

auto routes = buildRoutes();
std::atomic<uint16_t> thread_count = {0};

void handle(Core::ClientSocket clientSocket){
    try{
        Core::HttpRequest request = clientSocket.recv();
        Core::HttpResponse response;

        if(request.path == "/" || request.path == ""){
            request.path = "/index.html";
        }
        auto route = routes.find(request.path);
        if(route != routes.end()){
            route->second->Run(request, response);
        } else {
            response.statusCode = 404;
        }
        std::this_thread::sleep_for(std::chrono::seconds(60)); 
        clientSocket.write(response);
    }catch(...){}
    thread_count.fetch_sub(1);
}

int main(){
    Core::WebSocket listener(4430, 10);
    while(true){
        try{
            Core::ClientSocket csocket = listener.accept();
            if(thread_count.load() < 100){
                thread_count.fetch_add(1);
                std::thread(handle, std::move(csocket)).detach();
            }else{
                Core::HttpResponse response;
                response.statusCode = 503;
                response.text = "Service temporarily unavailable - too many connections";
                csocket.write(response);
            }
        }catch(...){ 
            continue; 
        }
    }
    return 1;
}