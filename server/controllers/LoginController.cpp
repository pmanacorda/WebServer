#include "BaseController.h"

namespace Controllers{
    void LoginController::Run(Core::HttpRequest& req, Core::HttpResponse& res){
        if(req.path.contains("login")){
            std::unordered_map<std::string, std::string> jsonObj;
            if(req.json.size() == 1){
                auto body = req.json[0];
                if (body.find("Username") != body.end() && 
                    body.find("Password") != body.end()) {
                    if (body["Username"] == "Patrick" && body["Password"] == "Manacorda") {
                        res.headers["Set-Cookie"] = "session_token=abc123; HttpOnly; Secure; Path=/; Max-Age=3600";
                        res.headers["Content-Type"] = "application/json";
                        tokens["session_token=abc123"] = std::time(nullptr);
                        res.statusCode = 200;
                        return; 
                    }
                }
            }

            res.headers["Content-Type"] = "application/json";
            res.statusCode = 403;
        }else{
            res.headers["Set-Cookie"] = "session_token=revoke; HttpOnly; Secure; Path=/; Max-Age=0";
            res.headers["Content-Type"] = "application/json";
            res.statusCode = 200;
            return; 
        }
    };

    bool LoginController::isAuthenticated(Core::HttpRequest& req){
        if(req.headers.find("Cookie") != req.headers.end()){
            auto cookie = req.headers["Cookie"];
            if(tokens.find(cookie) != tokens.end()){
                return std::difftime(std::time(nullptr), tokens[cookie]) < 3600;
            }
        }

        return false;
    }
}