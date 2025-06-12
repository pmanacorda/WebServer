#include "BaseController.h"

namespace Controllers{
    void TestController::Run(Core::HttpRequest& req, Core::HttpResponse& res){
        std::unordered_map<std::string, std::string> jsonObj;
        jsonObj["Data"] = "Result";
        jsonObj["Data2"] = "Result2";
        jsonObj["Data3"] = std::to_string(req.json.size());

         std::unordered_map<std::string, std::string> jsonObj2;
        jsonObj2["Data"] = "Result";
        jsonObj2["Data2"] = "Result2";
        jsonObj2["Data3"] = std::to_string(req.json.size());
        
        res.body.push_back(jsonObj);
        res.body.push_back(jsonObj2);

        res.headers["Content-Type"] = "application/json";
        res.statusCode = 200;
    };
}