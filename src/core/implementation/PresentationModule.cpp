
#include "PresentationModule.h"
/*
    See: https://www.json.org/json-en.html
    This parser focuses on flat objects only
*/
std::vector<std::unordered_map<std::string, std::string>> JsonUtils::deserialize(std::string raw){
    std::vector<std::unordered_map<std::string, std::string>> res;
    std::unordered_map<std::string, std::string> obj;
    std::string key = "";
    std::string value = "";
    char buffer[100];
    int buffIndex = 0;
    for(int i=0; i<raw.size(); i++){
        switch(raw[i]){
            case '{':
            case ' ':
            case ':':
            case '[':
            case ']':
                break;
            case '"':
            case ',':
                if(buffIndex > 0){
                    if(key.size() == 0){
                        key = std::string(buffer, buffIndex);
                    }else{
                        value = std::string(buffer, buffIndex);;
                        obj.insert_or_assign(key, value);
                        key = "";
                        value = "";
                    }
                    buffIndex = 0;
                }
                break;
            case '}':
                res.push_back(obj);
                obj.clear();
                break;
            default:
                buffer[buffIndex++]=raw[i];
                break;
        }
    }
    return res;
}

std::string JsonUtils::serialize(std::vector<std::unordered_map<std::string, std::string>> input){
    std::string output = "[";
    bool firstItem = true;
    for (auto item : input){
        std::string inner = "";
        if(!firstItem) inner += ',';
        inner += '{';
        bool first = true;
        for(auto it = item.begin(); it != item.end(); it++){
            if(!first) inner += ',';
            std::stringstream kvp;
            kvp << '"' << it->first << '"' << ':' << '"' << it->second << '"';
            inner.append(kvp.str());
            first = false;
        }
        inner += '}';
        output += inner;
        firstItem = false;
    }
    output += ']';
    return output;
}