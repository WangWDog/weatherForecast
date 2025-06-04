#include "weather_manager.h"
#include <json.hpp>
#include <curl/curl.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <utility>
#include <ctime>

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

ForecastResult WeatherManager::get7DayForecast(const std::string& locationId,const std::string& language,int cacheExpiryMinutes) {
    ForecastResult result;
    const std::string cacheFile = "cache.json";
    json cache;
    std::ifstream fin(cacheFile);

    if (fin) {
        try {
            fin >> cache;
        } catch (...) {
            cache.clear();
        }
        fin.close();

        if (cache.contains("forecast") && cache["forecast"].contains("timestamp")) {
            std::time_t now = std::time(nullptr);
            std::time_t ts = cache["forecast"]["timestamp"].get<std::time_t>();

            if (difftime(now, ts) < cacheExpiryMinutes * 60) {
                result.fromCache = true;
                result.timestamp = ts;
                for (const auto& item : cache["forecast"]["data"]) {
                    DailyForecast f;
                    f.date = item["date"];
                    f.textDay = item["textDay"];
                    f.textNight = item["textNight"];
                    f.tempMax = item["tempMax"];
                    f.tempMin = item["tempMin"];
                    f.windDirDay = item["windDirDay"];
                    f.windScaleDay = item["windScaleDay"];
                    f.precip = item["precip"];
                    f.humidity = item["humidity"];
                    result.forecasts.push_back(f);
                }
                return result;
            }
        }
    }

    std::string url = "https://" + host + "/v7/weather/7d?location=" + locationId+"&lang="+language;
    std::string response;
    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("X-QW-Api-Key: " + apiKey).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            auto j = json::parse(response, nullptr, false);
            if (!j.is_discarded() && j["code"] == "200") {
                result.fromCache = false;
                result.timestamp = std::time(nullptr);
                for (const auto& day : j["daily"]) {
                    DailyForecast df;
                    df.date = day["fxDate"];
                    df.textDay = day["textDay"];
                    df.textNight = day["textNight"];
                    df.tempMax = day["tempMax"];
                    df.tempMin = day["tempMin"];
                    df.windDirDay = day["windDirDay"];
                    df.windScaleDay = day["windScaleDay"];
                    df.precip = day["precip"];
                    df.humidity = day["humidity"];
                    result.forecasts.push_back(df);
                }

                // 写入缓存
                cache["forecast"]["timestamp"] = result.timestamp;
                cache["forecast"]["data"] = json::array();
                for (const auto& f : result.forecasts) {
                    cache["forecast"]["data"].push_back({
                        {"date", f.date},
                        {"textDay", f.textDay},
                        {"textNight", f.textNight},
                        {"tempMax", f.tempMax},
                        {"tempMin", f.tempMin},
                        {"windDirDay", f.windDirDay},
                        {"windScaleDay", f.windScaleDay},
                        {"precip", f.precip},
                        {"humidity", f.humidity}
                    });
                }
                std::ofstream fout(cacheFile);
                if (fout) fout << cache.dump(4);
            }
        }
    }
    return result;
}

LifeIndexWithMeta WeatherManager::getLifeIndices(const std::string& locationId, int expiryMinutes) {
    LifeIndexWithMeta result;
    const std::string cacheFile = "cache.json";
    nlohmann::json cache;
    std::ifstream fin(cacheFile);

    if (fin) {
        try {
            fin >> cache;
        } catch (...) {
            cache.clear();
        }
        fin.close();

        if (cache.contains("life_index") && cache["life_index"].contains("timestamp")) {
            std::time_t now = std::time(nullptr);
            std::time_t ts = cache["life_index"]["timestamp"].get<std::time_t>();
            if (difftime(now, ts) < expiryMinutes * 60) {
                result.timestamp = ts;
                result.fromCache = true;
                for (const auto& item : cache["life_index"]["data"]) {
                    result.indices.push_back({
                        item["date"],
                        item["name"],
                        item["level"],
                        item["category"],
                        item["text"]
                    });
                }
                return result;
            }
        }
    }

    // 请求网络数据
    std::string url = "https://" + host + "/v7/indices/1d?type=0&location=" + locationId + "&lang=" + lang;
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
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
                result.timestamp = std::time(nullptr);
                for (const auto& item : j["daily"]) {
                    result.indices.push_back({
                        item.value("date", ""),
                        item.value("name", ""),
                        item.value("level", ""),
                        item.value("category", ""),
                        item.value("text", "")
                    });
                }
                // 写入缓存
                nlohmann::json cacheOut;
                cacheOut["life_index"]["timestamp"] = result.timestamp;
                cacheOut["life_index"]["data"] = nlohmann::json::array();
                for (const auto& idx : result.indices) {
                    cacheOut["life_index"]["data"].push_back({
                        {"date", idx.date},
                        {"name", idx.name},
                        {"level", idx.level},
                        {"category", idx.category},
                        {"text", idx.text}
                    });
                }
                std::ofstream fout(cacheFile);
                if (fout) fout << cacheOut.dump(4);
            }
        } catch (...) {}
    }

    return result;
}
