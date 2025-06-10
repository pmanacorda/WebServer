#include "HttpModule.h"

namespace Core {
    HttpRequest::HttpRequest() {
        method = GET;
        path = body = "";
        json.clear();
        headers.clear();
        parameters.clear();
    }

    HttpResponse::HttpResponse() {
        statusCode = 200;
        text = "";
        body.clear();
        headers.clear();
    }

    std::string HttpResponse::getStatusStr(int status) {
        switch (status) {
            case 200: return "OK";
            case 400: return "Bad Request";
            case 404: return "Not Found";
            case 500: return "Internal Server Error";
            default: return "Unknown";
        }
    }
}
