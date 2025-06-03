#include "weather_manager.h"
#include <json.hpp>
#include <curl/curl.h>
#include <sstream>
#include <iostream>
#include <utility>

using json = nlohmann::json;

WeatherManager::WeatherManager(std::string key,std::string host) : apiKey(std::move(key)),host(std::move(host)) {}

static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}
std::vector<CityResult> WeatherManager::searchCity(const std::string& keyword) {
    std::vector<CityResult> results;
    std::string response;
    std::string url = "https://" + host + "/geo/v2/city/lookup?location=";

    CURL* curl = curl_easy_init();
    if (curl) {
        char* encodedKeyword = curl_easy_escape(curl, keyword.c_str(), keyword.length());
        // std::cout << encodedKeyword << std::endl;
        if (!encodedKeyword) {
            std::cerr << "URL 编码失败" << std::endl;
            curl_easy_cleanup(curl);
            return results;
        }

        std::string fullUrl = url + encodedKeyword;
        std::cout << fullUrl << std::endl;
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
        std::cout << response << std::endl;
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
