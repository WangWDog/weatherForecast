#include "weather_manager.h"
#include <iostream>
#include <curl/curl.h>
#include "../utils/json.hpp"  // nlohmann/json.hpp，路径按需调整

using json = nlohmann::json;

// 写回调函数，将 curl 下载的数据写入 std::string
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

WeatherManager::WeatherManager() {}

void WeatherManager::setCity(const std::string &cityName) {
    city = cityName;
}

void WeatherManager::updateWeather() {
    std::string apiKey = "YOUR_API_KEY";  // 可以从 config.json 中读取
    std::string apiUrl = "https://api.openweathermap.org/data/2.5/weather?q=" +
                         city + "&appid=" + apiKey + "&units=metric&lang=zh_cn";

    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            try {
                auto json_obj = json::parse(response);
                std::string description = json_obj["weather"][0]["description"];
                double temp = json_obj["main"]["temp"];

                weatherText = "天气：" + description + "，温度：" + std::to_string(temp) + "℃";
            } catch (...) {
                weatherText = "天气解析失败";
            }
        } else {
            weatherText = "天气获取失败";
        }
    }
}

void WeatherManager::printWeather() {
    std::cout << weatherText << std::endl;
}

std::string WeatherManager::getWeatherText() const {
    return weatherText;
}
