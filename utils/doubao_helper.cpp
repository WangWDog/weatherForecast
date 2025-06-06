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

        std::string data = json{
            {"model", endpointId},
            {"messages", {
                {{"role", "user"}, {"content", prompt}}
            }}
        }.dump();

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.doubao.com/v1/chat/completions");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            return "[网络错误: " + std::string(curl_easy_strerror(res)) + "]";
        }
    } else {
        return "[初始化 curl 失败]";
    }

    try {
        json j = json::parse(response);
        return j["choices"][0]["message"]["content"].get<std::string>();
    } catch (...) {
        return "[解析响应失败]";
    }
}
