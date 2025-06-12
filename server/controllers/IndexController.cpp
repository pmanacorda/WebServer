#include "BaseController.h"

namespace Controllers{
    void IndexController::Run(Core::HttpRequest& req, Core::HttpResponse& res){
        std::string content;
        std::string contentType;

        if(req.path == "/index.html"){
            content = readFile("client/index/index.html");
            contentType = "text/html";
        }
        else if(req.path == "/index.css"){
            content = readFile("client/index/index.css");
            contentType = "text/css";
        }
        else if(req.path == "/index.js"){
            content = readFile("client/index/index.js");
            contentType = "application/javascript";
        }
        else if(req.path == "/favicon.ico"){
            content = readFile("client/favicon.ico");
            contentType = "image/x-icon";
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
