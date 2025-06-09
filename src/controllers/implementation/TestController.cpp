#include "BaseController.h"

namespace Controllers{
    void TestController::Run(Core::HttpRequest& req, Core::HttpResponse& res){
        res.body.insert_or_assign("Data", "Result");
        res.body.insert_or_assign("Data2", "Result2");
        res.statusCode = 200;
    };
}