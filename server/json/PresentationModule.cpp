#include "PresentationModule.h"

/*
    See: https://www.json.org/json-en.html
    This parser focuses on flat objects and flat lists of flat objects only
    Supports:
    - Single flat object: {"key": "value", "key2": "value2"}
    - Array of flat objects: [{"key": "value"}, {"key2": "value2"}]
*/
std::vector<std::unordered_map<std::string, std::string>> JsonUtils::deserialize(std::string raw){
    std::vector<std::unordered_map<std::string, std::string>> res;
    std::unordered_map<std::string, std::string> obj;
    std::string key = "";
    std::string value = "";
    char buffer[100];
    int buffIndex = 0;
    
    for(size_t i = 0; i < raw.size(); i++){
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
                    if(key.empty()){
                        key = std::string(buffer, buffIndex);
                    }else{
                        value = std::string(buffer, buffIndex);
                        obj.insert_or_assign(std::move(key), std::move(value));
                        key.clear();
                        value.clear();
                    }
                    buffIndex = 0;
                }
                break;
            case '}':
                res.push_back(std::move(obj));
                obj.clear();
                break;
            default:
                buffer[buffIndex++] = raw[i];
                break;
        }
    }
    return res;
}

std::string JsonUtils::serialize(std::vector<std::unordered_map<std::string, std::string>> input){
    std::string output = "[";
    bool firstItem = true;
    
    for (const auto& item : input){
        std::string inner = "";
        if(!firstItem) inner += ',';
        inner += '{';
        bool first = true;
        
        for(const auto& [key, val] : item){
            if(!first) inner += ',';
            inner += "\"" + key + "\":\"" + val + "\"";
            first = false;
        }
        inner += '}';
        output += inner;
        firstItem = false;
    }
    output += ']';
    return output;
}