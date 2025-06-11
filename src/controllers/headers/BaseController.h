#ifndef BASE_CONTROLLER_H
#define BASE_CONTROLLER_H
#include "HttpModule.h"
#include <fstream>
#include <sstream>
namespace Controllers{
    class BaseController{
        public:
        BaseController();
        ~BaseController();

        std::string readFile(const std::string&);
        virtual void Run(Core::HttpRequest&, Core::HttpResponse&);
    };

    class TestController : public BaseController {
    public:
        void Run(Core::HttpRequest& req, Core::HttpResponse& res) override;
    };

    class IndexController : public BaseController {
    public:
        void Run(Core::HttpRequest& req, Core::HttpResponse& res) override;
    };
}

#endif