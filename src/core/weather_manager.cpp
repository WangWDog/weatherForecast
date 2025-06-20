#include "weather_manager.h"
#include <json.hpp>
#include <curl/curl.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <utility>
#include <ctime>

#include "CacheManager.h"

using json = nlohmann::json;

WeatherManager::WeatherManager(std::string key, std::string host, std::string lang)
    : apiKey(std::move(key)), host(std::move(host)), lang(std::move(lang)) {}

static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

std::vector<CityResult> WeatherManager::searchCity(const std::string& keyword,const std::string& lang) {
    std::vector<CityResult> results;
    std::string response;
    std::string url = "https://" + host + "/geo/v2/city/lookup?location=";

    CURL* curl = curl_easy_init();
    if (curl) {
        char* encodedKeyword = curl_easy_escape(curl, keyword.c_str(), keyword.length());
        if (!encodedKeyword) {
            std::cerr << "URL 编码失败" << std::endl;
            curl_easy_cleanup(curl);
            return results;
        }

        std::string fullUrl = url + encodedKeyword +"&lang="+ lang;
        curl_free(encodedKeyword);
        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("X-QW-Api-Key: " + apiKey).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "请求失败: " << curl_easy_strerror(res) << std::endl;
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    try {
        auto j = json::parse(response);
        if (j.contains("location")) {
            for (const auto& item : j["location"]) {
                CityResult city;
                city.name = item.value("name", "");
                city.id = item.value("id", "");
                city.country = item.value("country", "");
                city.adm1 = item.value("adm1", "");
                city.adm2 = item.value("adm2", "");
                results.push_back(city);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "解析JSON失败: " << e.what() << std::endl;
    }

    return results;
}
ForecastResult WeatherManager::get7DayForecast(
    const std::string& locationId,
    const std::string& language,
    const std::string& unit,
    int cacheExpiryMinutes,
    CacheManager& cache // ✅ 外部传入 cache
) {
    ForecastResult result;
    std::string cacheKey = "forecast_" + language + "_" + unit;

    // 1. 缓存可用：直接使用
    if (cache.isValid(cacheKey, cacheExpiryMinutes)) {
        result.fromCache = true;
        result.timestamp = cache.getTimestamp(cacheKey);

        for (const auto& item : cache.getCache(cacheKey)) {
            DailyForecast f;
            f.date = item.value("date", "");
            f.textDay = item.value("textDay", "");
            f.textNight = item.value("textNight", "");
            f.tempMax = item.value("tempMax", "");
            f.tempMin = item.value("tempMin", "");
            f.windDirDay = item.value("windDirDay", "");
            f.windScaleDay = item.value("windScaleDay", "");
            f.precip = item.value("precip", "");
            f.humidity = item.value("humidity", "");
            result.forecasts.push_back(f);
        }

        return result;
    }
    char simple = unit[0];
    // 2. 请求网络数据
    std::string url = "https://" + host + "/v7/weather/7d?location=" + locationId + "&lang=" + language + "&unit=" + simple;
    std::string response;
    CURL* curl = curl_easy_init();

    if (!curl) return result;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("X-QW-Api-Key:" + apiKey).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "❌ 天气请求失败，状态码: " << res << std::endl;
        return result;
    }

    // 3. 解析并保存
    try {
        auto j = nlohmann::json::parse(response);
        if (j.contains("code") && j["code"] == "200" && j.contains("daily")) {
            result.fromCache = false;
            result.timestamp = std::time(nullptr);

            nlohmann::json dataJson = nlohmann::json::array();

            for (const auto& day : j["daily"]) {
                DailyForecast df;
                df.date = day.value("fxDate", "");
                df.textDay = day.value("textDay", "");
                df.textNight = day.value("textNight", "");
                df.tempMax = day.value("tempMax", "");
                df.tempMin = day.value("tempMin", "");
                df.windDirDay = day.value("windDirDay", "");
                df.windScaleDay = day.value("windScaleDay", "");
                df.precip = day.value("precip", "");
                df.humidity = day.value("humidity", "");

                result.forecasts.push_back(df);

                dataJson.push_back({
                    {"date", df.date},
                    {"textDay", df.textDay},
                    {"textNight", df.textNight},
                    {"tempMax", df.tempMax},
                    {"tempMin", df.tempMin},
                    {"windDirDay", df.windDirDay},
                    {"windScaleDay", df.windScaleDay},
                    {"precip", df.precip},
                    {"humidity", df.humidity}
                });
            }

            cache.setCache(cacheKey, dataJson);  // ✅ 使用传入的 cache 保存
        }
    } catch (...) {
        std::cerr << "❌ JSON解析失败: forecast 响应异常" << std::endl;
    }

    return result;
}

LifeIndexWithMeta WeatherManager::getLifeIndices(
    const std::string& locationId,
    int expiryMinutes,
    CacheManager& cache  // 👈 使用外部传入的 cache
) {
    LifeIndexWithMeta result;

    // 1. 如果缓存可用，优先读取
    if (cache.isValid("life_index", expiryMinutes)) {
        result.fromCache = true;
        result.timestamp = cache.getTimestamp("life_index");

        for (const auto& item : cache.getCache("life_index")) {
            result.indices.push_back({
                item.value("date", ""),
                item.value("name", ""),
                item.value("level", ""),
                item.value("category", ""),
                item.value("text", "")
            });
        }
        return result;
    }

    // 2. 否则发起网络请求
    std::string url = "https://" + host + "/v7/indices/1d?type=0&location=" + locationId + "&lang=" + lang;
    std::string response;
    CURL* curl = curl_easy_init();

    if (!curl) return result;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("X-QW-Api-Key: " + apiKey).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) return result;

    try {
        auto j = nlohmann::json::parse(response);
        if (j.contains("daily")) {
            result.fromCache = false;
            result.timestamp = std::time(nullptr);

            nlohmann::json dataJson = nlohmann::json::array();
            for (const auto& item : j["daily"]) {
                LifeIndex idx {
                    item.value("date", ""),
                    item.value("name", ""),
                    item.value("level", ""),
                    item.value("category", ""),
                    item.value("text", "")
                };
                result.indices.push_back(idx);

                dataJson.push_back({
                    {"date", idx.date},
                    {"name", idx.name},
                    {"level", idx.level},
                    {"category", idx.category},
                    {"text", idx.text}
                });
            }

            cache.setCache("life_index", dataJson);  // ✅ 使用 ctx.cache 写入
        }
    } catch (...) {
        std::cerr << "❌ JSON解析失败: life_index 响应异常" << std::endl;
    }

    return result;
}


