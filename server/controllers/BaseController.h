#ifndef BASE_CONTROLLER_H
#define BASE_CONTROLLER_H
#include "HttpModule.h"
#include "AuthService.h"
#include <fstream>
#include <memory>
#include <sstream>
#include <any>
namespace Controllers{
    class BaseController{
        public:
        BaseController(){};
        ~BaseController(){};

        std::string readFile(const std::string& filename){
            std::ifstream file(filename);
            if(file.is_open()){
                std::stringstream buffer;
                buffer << file.rdbuf();
                return buffer.str();
            }
            return "";
        }
        virtual void Run(Core::HttpRequest& req, Core::HttpResponse& res){}
    };

    class LoginController : public BaseController {
    private:
        std::shared_ptr<Services::AuthService> _authService;
    public:
        LoginController(std::any);
        void Run(Core::HttpRequest& req, Core::HttpResponse& res) override;
        bool isAuthenticated(Core::HttpRequest& req);
    };

    class IndexController : public BaseController {
    public:
        void Run(Core::HttpRequest& req, Core::HttpResponse& res) override;
    };

    class AboutController : public BaseController {
    public:
        void Run(Core::HttpRequest& req, Core::HttpResponse& res) override;
    };
}

#endif