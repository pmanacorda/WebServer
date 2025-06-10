#include "BaseController.h"

namespace Controllers{
    void IndexController::Run(Core::HttpRequest& req, Core::HttpResponse& res){
        std::string content;
        std::string contentType;

        if(req.path == "/index.html"){
            content = readFile("src/pages/index/index.html");
            contentType = "text/html";
        }
        else if(req.path == "/index.css"){
            content = readFile("src/pages/index/index.css");
            contentType = "text/css";
        }
        else if(req.path == "/index.js"){
            content = readFile("src/pages/index/index.js");
            contentType = "application/javascript";
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
