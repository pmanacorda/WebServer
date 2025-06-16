#include "AuthService.h"

namespace Services {
    std::string AuthService::generateCookie() {
        auto cookie = "session_token=abc123; HttpOnly; Secure; Path=/; Max-Age=3600";
        tokens[cookie] = std::time(nullptr);
        return cookie;
    }

    bool AuthService::validUser(std::string username, std::string password){
        return username == "Patrick" && password == "Manacorda";
    }

    bool AuthService::isAuthenticated(Core::HttpRequest& request) {
        auto it = request.headers.find("Cookie");
        if(it != request.headers.end()){
            std::string token = it->second;
            if (tokens.find(token) != tokens.end()) {
                auto now = std::time(nullptr);
                if (std::difftime(now, tokens[token]) < 3600) {
                    return true;
                } else {
                    tokens.erase(token);
                    return false;
                }
            }
        }
        return false;
    }
}