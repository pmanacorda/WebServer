#include "BaseController.h"

namespace Controllers{
    BaseController::BaseController() {}
    BaseController::~BaseController() {}

    std::string BaseController::readFile(const std::string& filename){
        std::ifstream file(filename);
        if(file.is_open()){
            std::stringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        }
        return "";
    }

    void BaseController::Run(Core::HttpRequest& req, Core::HttpResponse& res) {}
}