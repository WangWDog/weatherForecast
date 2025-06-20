#include "doubao_manager.h"

using json = nlohmann::json;

DoubaoManager::DoubaoManager(std::string token, std::string endpoint)
    : token(std::move(token)), endpointId(std::move(endpoint)) {}

std::string DoubaoManager::request(const std::string& systemPrompt, const std::string& userPrompt) {
    json payload = {
        {"model", endpointId},
        {"messages", {
            {{"role", "system"}, {"content", systemPrompt}},
            {{"role", "user"}, {"content", userPrompt}}
        }}
    };

    std::string body = payload.dump();
    std::string response;
    std::vector<std::string> headers = {
        "Authorization: Bearer " + token,
        "Content-Type: application/json"
    };
    if (!HttpClient::post("https://ark.cn-beijing.volces.com/api/v3/chat/completions", body, response, headers)) {
        return "";
    }
    try {
        auto j = json::parse(response);
        if (j.contains("choices") && !j["choices"].empty()) {
            return j["choices"][0]["message"]["content"].get<std::string>();
        }
    } catch (...) {
    }
    return "";
}

std::string DoubaoManager::translate(const std::string& text, const std::string& targetLang) {
    std::string prompt = "请将以下内容翻译为" + targetLang + "：\n" + text;
    return request("You are a helpful assistant.", prompt);
}

std::string DoubaoManager::getAdvice(const std::string& prompt) {
    std::string sys = "你是一个生活助手，擅长为用户提供生活建议。请用不超过100字的一段话返回 不要使用markdown格式，用\\n表示换行 每行不超过20字";
    return request(sys, prompt);
}
