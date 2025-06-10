/* AUTHOR: Patrick Manacorda */
#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>
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

void handle(Core::ClientSocket& clientSocket){
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
        
        clientSocket.write(response);
    }catch(...){}
}

int main(){
    Core::WebSocket listener(443, 10);
    while(true){
        try{
            Core::ClientSocket csocket = listener.accept();
            handle(csocket);
        }catch(...){ 
            continue; 
        }
    }
    return 1;
}