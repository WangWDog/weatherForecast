#include <curl/curl.h>
#include <json.hpp>
#include <fstream>
#include <iostream>
#include "../config/config_key.h"
#include "doubao_translator.h"
#include "../core/weather_manager.h"


using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}
void translateLunarDataIfEnglish(LunarData& data, const std::string& lang, const ConfigKey& configKey) {
    if (lang != "en") return;

    auto t = [&](std::string& field) {
        if (!field.empty()) {
            std::string translated = translateWithDoubao(field, "英文", configKey);
            if (!translated.empty()) field = translated;
        }
    };

    t(data.festivals);
    t(data.yi);
    t(data.ji);
    t(data.weiyuShort);
    t(data.weiyuLong);
}

std::string translateWithDoubao(const std::string& text, const std::string& targetLang, const ConfigKey& configKey) {
    std::string apiKey = configKey.getDoubaoKey();
    std::string endpointId = configKey.getDoubaoEndpoint();

    if (apiKey.empty() || endpointId.empty()) {
        std::cerr << "❌ Doubao 配置缺失。" << std::endl;
        return text;
    }

    // 拼接翻译提示词
    std::string prompt = "请将以下内容翻译为" + targetLang + "：\n" + text;

    // 构造请求 JSON
    json payload = {
        {"model", endpointId},
        {"messages", {
            {{"role", "system"}, {"content", "You are a helpful assistant. 请在json字串中每个值前加入\\t"}},
            {{"role", "user"}, {"content", prompt}}
        }}
    };

    std::string response;
    CURL* curl = curl_easy_init();
    if (!curl) return text;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, "https://ark.cn-beijing.volces.com/api/v3/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    std::string body = payload.dump();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // 可选
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "❌ Doubao 请求失败: " << curl_easy_strerror(res) << std::endl;
        return text;
    }

    // 解析返回内容
    try {
        auto j = json::parse(response);
        if (j.contains("choices")) {
            return j["choices"][0]["message"]["content"];
        }
    } catch (...) {
        std::cerr << "❌ 返回 JSON 解析失败" << std::endl;
    }

    return text;
}
