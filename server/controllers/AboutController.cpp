#include "BaseController.h"

namespace Controllers{
    void AboutController::Run(Core::HttpRequest& req, Core::HttpResponse& res){
        std::string content;
        std::string contentType;

        if(req.path == "/about.html"){
            content = readFile("client/about/about.html");
            contentType = "text/html";
        }
        else if(req.path == "/about.css"){
            content = readFile("client/about/about.css");
            contentType = "text/css";
        }
        else if(req.path == "/about.js"){
            content = readFile("client/about/about.js");
            contentType = "application/javascript";
        }
        else if(req.path == "/api/about" && req.method == Core::HttpMethod::GET){
            std::unordered_map<std::string, std::string> result;
            result["Text"] = "test";
            res.statusCode = 200;
            res.body.push_back(std::move(result));
            res.headers["Content-Type"] = "application/json";
            return;
        }
        else {
            res.statusCode = 404;
            res.text = "File not found";
            res.headers["Content-Type"] = "text/plain";
            return;
        }

        res.statusCode = 200;
        res.text = content;
        res.headers["Content-Type"] = contentType;
    }
}
