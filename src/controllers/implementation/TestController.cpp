#include "BaseController.h"

namespace Controllers{
    void TestController::Run(Core::HttpRequest& req, Core::HttpResponse& res){
        std::unordered_map<std::string, std::string> jsonObj;
        jsonObj["Data"] = "Result";
        jsonObj["Data2"] = "Result2";
        jsonObj["Data3"] = std::to_string(req.json.size());
        
        res.body.push_back(jsonObj);
        res.statusCode = 200;
    };
}