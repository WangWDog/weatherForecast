#include "lunar_api.h"
#include <iostream>
#include <json.hpp>
#include <curl/curl.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "doubao_translator.h"

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
std::string getTodayDateYYYYMMDD() {
    std::time_t now = std::time(nullptr);
    std::tm* local = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(local, "%Y%m%d");  // 输出格式如 20240605
    return oss.str();
}

std::string formatLunarInfo(const LunarData& d, I18n& i18n)
{
    std::ostringstream oss;

    auto label = [&](const std::string& key) {
        return i18n.tr("lunar", key);
    };

    if (!d.solarDate.empty())
        oss << "\t📅 " << label("solar") << "：" << d.solarDate << "（" << d.week << "）\n";
    if (!d.lunarYear.empty() || !d.lunar.empty())
        oss << "\t🌙 " << label("lunar_year") << "：" << d.lunarYear << " "
            << d.lunar << "（" << d.thisYear << "）\n";
    if (!d.ganzhiYear.empty() || !d.constellation.empty())
        oss << "\t📖 " << label("ganzhi_year") << "：" << d.ganzhiYear << " | "
            << label("constellation") << "：" << d.constellation << "\n";
    if (!d.festivals.empty())
        oss << "\t🎉 " << label("festival") << "：" << d.festivals << "\n";
    if (!d.jieqi.empty())
        oss << "\t🌾 " << label("jieqi") << "：" << d.jieqi << "\n";
    if (!d.yi.empty())
        oss << "\t✅ " << label("yi") << "：" << d.yi << "\n";
    if (!d.ji.empty())
        oss << "\t⚠️ " << label("ji") << "：" << d.ji << "\n";
    if (!d.weiyuShort.empty())
        oss << "\t💬 " << label("weiyu_short") << "：" << d.weiyuShort << "\n";
    if (!d.weiyuLong.empty())
        oss << "\t📖 " << label("weiyu_long") << "：" << d.weiyuLong << "\n";

    return oss.str();
}

std::string callLunarApi(ConfigKey& config_key, const std::string& lang)
{
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    std::string apiKey = config_key.getFreeApiKey();
   std::string date = getTodayDateYYYYMMDD();
    // ✅ 加入语言参数 lang
    std::string final_url = "https://api.shwgij.com/api/lunars/lunar?date=" + date +
                            "&key=" + apiKey +
                            "&lang=" + lang;  // 👈 就是这部分

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

void printLunarData(const LunarData& d, const std::string& lang, const ConfigKey& configKey, const I18n& i18n) {
    LunarData data = d;

    if (lang == "en") {
        translateLunarDataIfEnglish(data, lang, configKey);
    }

    // 所有标签文本通过 i18n 获取（从 lang_en.json 中定义）
    std::cout << i18n.tr("lunar", "solar_date") << ": " << data.solarDate << " (" << data.week << ")\n";
    std::cout << i18n.tr("lunar", "lunar_date") << ": " << data.lunarYear << " " << data.lunar
              << " (" << data.thisYear << ")\n";
    std::cout << i18n.tr("lunar", "ganzhi_year") << ": " << data.ganzhiYear
              << " | " << i18n.tr("lunar", "constellation") << ": " << data.constellation << "\n";
    std::cout << i18n.tr("lunar", "festivals") << ": " << data.festivals << "\n";
    std::cout << i18n.tr("lunar", "jieqi") << ": " << data.jieqi << "\n";
    std::cout << i18n.tr("lunar", "yi") << ": " << data.yi << "\n";
    std::cout << i18n.tr("lunar", "ji") << ": " << data.ji << "\n";
    std::cout << i18n.tr("lunar", "weiyu_short") << ": " << data.weiyuShort << "\n";
    std::cout << i18n.tr("lunar", "weiyu_long") << ": " << data.weiyuLong << "\n";
}

