#include "doubao_helper.h"
#include <curl/curl.h>
#include <json.hpp>
#include <iostream>

using json = nlohmann::json;

static size_t WriteCallback(char* ptr, size_t size, size_t nmemb, std::string* output) {
    output->append(ptr, size * nmemb);
    return size * nmemb;
}

std::string callDoubaoAI(const std::string& token, const std::string& endpointId, const std::string& fullPrompt) {
    // 初始化 CURL
    CURL* curl = curl_easy_init();
    if (!curl) {
        return "[CURL 初始化失败]";
    }

    std::string response;

    // 构建请求体
    json payload = {
        {"model", endpointId},
        {"messages", {
            {{"role", "system"}, {"content", "你是一个生活助手，擅长为用户提供生活建议。请用不超过100字的一段话返回 不要使用markdown格式，用\\n表示换行 每行不超过20字"}},
            {{"role", "user"}, {"content", fullPrompt}}
        }}
    };

    std::string body = payload.dump();

    // 构建请求头
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // 设置 CURL 选项
    curl_easy_setopt(curl, CURLOPT_URL, "https://ark.cn-beijing.volces.com/api/v3/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  // 忽略 SSL 验证
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");

    // 执行请求
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        return "[网络请求失败: " + std::string(curl_easy_strerror(res)) + "]";
    }

    // 解析响应
    try {
        json j = json::parse(response);
        if (j.contains("choices") && j["choices"].is_array() && j["choices"].size() > 0) {
            // 只返回 AI 的建议内容
            return j["choices"][0]["message"]["content"].get<std::string>();
        } else {
            return "[AI 响应格式不正确]";
        }
    } catch (const std::exception& e) {
        return "[解析响应失败] 错误信息: " + std::string(e.what());
    }
}
