#include "BaseController.h"

namespace Controllers{
    void LoginController::Run(Core::HttpRequest& req, Core::HttpResponse& res){
        std::unordered_map<std::string, std::string> jsonObj;
        if(req.json.size() == 1){
            auto body = req.json[0];
            if (body.find("Username") != body.end() && 
                body.find("Password") != body.end()) {
                if (body["Username"] == "Patrick" && body["Password"] == "Manacorda") {
                    res.headers["Set-Cookie"] = "session_token=abc123; HttpOnly; Secure; Path=/; Max-Age=3600";
                    res.headers["Content-Type"] = "application/json";
                    res.statusCode = 200;
                    return; 
                }
            }
     }

        res.headers["Content-Type"] = "application/json";
        res.statusCode = 403;
    };
}