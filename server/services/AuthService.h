#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include "HttpModule.h"
#include <unordered_map>
#include <string>
#include <ctime>
#include <random>

namespace Services{
    class AuthService{
        private:
        std::unordered_map<std::string, std::time_t> tokens;
        
        public:
        std::string generateCookie();
        bool isAuthenticated(Core::HttpRequest&);
        bool validUser(std::string, std::string);
    };
}

#endif