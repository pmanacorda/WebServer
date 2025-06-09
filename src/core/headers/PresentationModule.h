#ifndef PRESENTATION_MODULE_H
#define PRESENTATION_MODULE_H
#include <unordered_map>
#include <string>
#include <sstream>
class JsonUtils{
    public:
    static std::unordered_map<std::string, std::string> deserialize(std::string);
    static std::string serialize(std::unordered_map<std::string, std::string>);
};
#endif