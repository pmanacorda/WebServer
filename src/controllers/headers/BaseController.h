#ifndef BASE_CONTROLLER_H
#define BASE_CONTROLLER_H
#include "HttpModule.h"
namespace Controllers{
    class BaseController{
        public:
        BaseController();
        ~BaseController();

        virtual void Run(Core::HttpRequest&, Core::HttpResponse&);
    };

    class TestController : public BaseController {
    public:
        void Run(Core::HttpRequest& req, Core::HttpResponse& res) override;
    };
}

#endif