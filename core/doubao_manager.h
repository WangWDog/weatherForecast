#pragma once
#include <string>
#include "../utils/http_client.h"
#include "../utils/json.hpp"

class DoubaoManager {
public:
    DoubaoManager(std::string token, std::string endpoint);
    std::string translate(const std::string& text, const std::string& targetLang);
    std::string getAdvice(const std::string& prompt);
private:
    std::string token;
    std::string endpointId;
    std::string request(const std::string& systemPrompt, const std::string& userPrompt);
};
