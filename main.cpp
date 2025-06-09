/* AUTHOR: Patrick Manacorda */
#include <functional>
#include "core/headers/HttpModule.h"
#include "core/headers/SocketModule.h"
#include "core/headers/PresentationModule.h"

void router(Core::HttpRequest &request, Core::HttpResponse &response){
    std::unordered_map<std::string, std::function<void(Core::HttpRequest&, Core::HttpResponse&)>> controllers;
    controllers["/api/test"] = [](Core::HttpRequest& req, Core::HttpResponse& res) {
        res.body.insert_or_assign("Data", "Result");
        res.body.insert_or_assign("Data2", "Result2");
        res.statusCode = 200;
    };
    if(controllers.find(request.path) != controllers.end()){
        controllers[request.path](request, response);
    }else{
        response.statusCode = 404;
    }
}

void handle(Core::ClientSocket clientSocket){
    try{
        // receive content stream
        Core::HttpRequest request = clientSocket.recv();
        Core::HttpResponse response;
        router(request, response);
        clientSocket.write(response);
    }catch(...){}
}

int main(){
    Core::WebSocket listener(8080, 10);
    while(true){
        try{
            Core::ClientSocket csocket = listener.accept();
            handle(csocket);
        }catch(...){ continue; }
    }
    return 1;
}