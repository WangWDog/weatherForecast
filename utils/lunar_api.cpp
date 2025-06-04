#include "lunar_api.h"
#include <iostream>
#include <json.hpp>
#include <curl/curl.h>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string callLunarApi(ConfigKey& config_key) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    std::string apiKey = config_key.getFreeApiKey();
    std::string date = "20240601";  // 可改为动态
    std::string final_url = "https://api.shwgij.com/api/lunars/lunar?date=" + date + "&key=" + apiKey;

   // std::cout << "🌐 请求 URL: " << final_url << std::endl;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, final_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "❌ cURL 请求失败：" << curl_easy_strerror(res) << std::endl;
            readBuffer.clear();
        }

        curl_easy_cleanup(curl);
    }

    return readBuffer;
}

LunarData parseLunarJson(const std::string& jsonStr) {
    LunarData data;
    try {
        auto j = json::parse(jsonStr);
        if (j.contains("data")) {
            const auto& d = j["data"];
            data.solarDate     = d.value("Solar", "");
            data.week          = d.value("Week", "");
            data.constellation = d.value("Constellation", "");
            data.festivals     = d.value("Festivals", "");
            data.lunarYear     = d.value("LunarYear", "");
            data.lunar         = d.value("Lunar", "");
            data.thisYear      = d.value("ThisYear", "");
            data.ganzhiYear    = d.value("GanZhiYear", "");
            data.jieqi         = d.value("JieQi1", "");
            data.yi            = d.value("YiDay", "");
            data.ji            = d.value("JiDay", "");
            data.weiyuShort    = d.value("WeiYu_s", "");
            data.weiyuLong     = d.value("WeiYu_l", "");
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ JSON 解析失败：" << e.what() << std::endl;
    }
    return data;
}

void printLunarData(const LunarData& d) {
    std::cout << "📅 公历日期：" << d.solarDate << "（" << d.week << "）\n";
    std::cout << "🌙 农历：" << d.lunarYear << " " << d.lunar << "（" << d.thisYear << "）\n";
    std::cout << "📖 干支年：" << d.ganzhiYear << " | 星座：" << d.constellation << "\n";
    std::cout << "🎉 节日：" << d.festivals << "\n";
    std::cout << "🌾 节气：" << d.jieqi << "\n";
    std::cout << "✅ 宜：" << d.yi << "\n";
    std::cout << "⚠️ 忌：" << d.ji << "\n";
    std::cout << "💬 微语短句：" << d.weiyuShort << "\n";
    std::cout << "💬 微语长句：" << d.weiyuLong << "\n";
}
