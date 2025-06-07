#include "doubao_helper.h"
#include <curl/curl.h>
#include <json.hpp>
#include <iostream>

using json = nlohmann::json;

static size_t WriteCallback(char* ptr, size_t size, size_t nmemb, std::string* output) {
    output->append(ptr, size * nmemb);
    return size * nmemb;
}

std::string callDoubaoAI(const std::string& token, const std::string& endpointId, const std::string& prompt) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        // ✅ 替换构造格式为新版 API 要求（包含 system 和 user 双角色）
        json payload = {
            {"model", endpointId},
            {"messages", {
                {{"role", "system"}, {"content", "你是一个生活助手，擅长为用户提供生活建议。"}},
                {{"role", "user"}, {"content", prompt}}
            }}
        };
        std::string body = payload.dump();

        // ✅ 修改为新版豆包接口
        curl_easy_setopt(curl, CURLOPT_URL, "https://ark.cn-beijing.volces.com/api/v3/chat/completions");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // ✅ 开启兼容选项以避免 SSL 报错（可选，根据平台决定）
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");  // ⚠️ 有些返回值默认是 gzip 压缩的

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            return "[网络错误: " + std::string(curl_easy_strerror(res)) + "]";
        }
    } else {
        return "[初始化 curl 失败]";
    }

    // ✅ 尝试解析新版返回格式
    try {
        json j = json::parse(response);
        if (j.contains("choices") && j["choices"].size() > 0) {
            return j["choices"][0]["message"]["content"].get<std::string>();
        } else {
            return "[AI 响应格式不正确]";
        }
    } catch (...) {
        return "[解析响应失败]";
    }
}
