#include "BaseController.h"

namespace Controllers{
    LoginController::LoginController(std::any authService){
        _authService = std::any_cast<std::shared_ptr<Services::AuthService>>(authService);
    }
    
    void LoginController::Run(Core::HttpRequest& req, Core::HttpResponse& res){
        if(req.path.contains("login")){
            if(req.json.size() == 1){
                auto body = req.json[0];
                if (body.find("Username") != body.end() && 
                    body.find("Password") != body.end()) {
                    if (_authService->validUser(body["Username"], body["Password"])) {
                        res.headers["Set-Cookie"] = _authService->generateCookie();
                        res.headers["Content-Type"] = "application/json";
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
    }

    bool LoginController::isAuthenticated(Core::HttpRequest& req){
        return _authService->isAuthenticated(req);
    }
}