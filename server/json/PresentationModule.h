#ifndef PRESENTATION_MODULE_H
#define PRESENTATION_MODULE_H
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <format>
class JsonUtils{
    public:
    static std::vector<std::unordered_map<std::string, std::string>> deserialize(std::string);
    static std::string serialize(std::vector<std::unordered_map<std::string, std::string>>);
};
#endif