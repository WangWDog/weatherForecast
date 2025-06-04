#include "ip_locator.h"
#include "json.hpp"  // nlohmann/json
#include <curl/curl.h>
#include <sstream>
#include <iostream>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

Location getLocationByIP() {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://ip-api.com/json/");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    Location loc;
    try {
        auto j = json::parse(readBuffer);
        if (j["status"] == "success") {
            loc.city = j["city"];
            loc.latitude = j["lat"];
            loc.longitude = j["lon"];
        } else {
            std::cerr << "Failed to get location: " << j["message"] << std::endl;
        }
    } catch (...) {
        std::cerr << "Failed to parse location JSON" << std::endl;
    }

    return loc;
}
