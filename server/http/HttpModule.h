#include <string>
#include <unordered_map>
#include <vector>

#ifndef HTTP_MODULE_H
#define HTTP_MODULE_H
namespace Core {
    enum HttpMethod {
        GET,
        POST,
        DELETE_
    };

    class HttpRequest {
    public:
        HttpRequest();

        HttpMethod method;
        std::string path;
        std::string body;
        std::vector<std::unordered_map<std::string, std::string>> json;
        std::unordered_map<std::string, std::string> headers;
        std::unordered_map<std::string, std::string> parameters;
    };

    class HttpResponse {
    public:
        HttpResponse(); 
        static std::string getStatusStr(int status);

        int statusCode;
        std::string text;
        std::vector<std::unordered_map<std::string, std::string>> body;
        std::unordered_map<std::string, std::string> headers;
    };
}
#endif
