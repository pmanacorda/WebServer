#include "AuthService.h"

namespace Services {
    std::string AuthService::generateCookie() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        
        std::string token = "session_token=";
        for (int i = 0; i < 32; ++i) { // gen 32-bit hex
            token += "0123456789abcdef"[dis(gen)];
        }
        
        tokens[token] = std::time(nullptr);
        return token + "; HttpOnly; Secure; Path=/; Max-Age=3600";
    }

    bool AuthService::validUser(std::string username, std::string password){
        return username == "Patrick" && password == "Manacorda";
    }

    bool AuthService::isAuthenticated(Core::HttpRequest& request) {
        auto it = request.headers.find("Cookie");
        if(it != request.headers.end()){
            std::string cookieHeader = it->second;
            size_t pos = cookieHeader.find("session_token=");
            if (pos != std::string::npos) {
                pos += 14; // length of "session_token="
                size_t end = cookieHeader.find(";", pos);
                if (end == std::string::npos) end = cookieHeader.length();
                
                std::string token = "session_token=" + cookieHeader.substr(pos, end - pos);
                
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
        }
        return false;
    }
}