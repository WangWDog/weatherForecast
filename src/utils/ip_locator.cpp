#include "ip_locator.h"
#include "json.hpp"  // nlohmann/json
#include <sstream>
#include <iostream>
#include "http_client.h"

using json = nlohmann::json;

Location getLocationByIP() {
    std::string readBuffer;
    HttpClient::get("http://ip-api.com/json/", readBuffer);

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
