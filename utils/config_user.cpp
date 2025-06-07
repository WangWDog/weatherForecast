#include "config_user.h"

ConfigUser::ConfigUser(const std::string& path) : BaseConfig(path) {}

std::string ConfigUser::getDefaultCity() const {
    return configJson.value("default_city", "beijing");
}

void ConfigUser::setDefaultCity(const std::string& city) {
    configJson["default_city"] = city;
}

bool ConfigUser::getUseIPLocation() const {
    return configJson.value("use_ip_location", false);
}

void ConfigUser::setUseIPLocation(bool useIP) {
    configJson["use_ip_location"] = useIP;
}

std::string ConfigUser::getUnits() const {
    return configJson.value("units", "metric");
}


void ConfigUser::setUnits(const std::string& units) {
    configJson["units"] = units;
}

std::string ConfigUser::getLanguage() const {
    return configJson.value("language", "zh");
}

void ConfigUser::setLanguage(const std::string& lang) {
    configJson["language"] = lang;
}
std::string ConfigUser::getDateFormateMenu() const {
    return configJson.value("date_formate_menu", "%Y-%m-%d %H:%M:%S");
}

void ConfigUser::setDateFormateMenu(const std::string& dateFormate) {
    configJson["date_formate_menu"] = dateFormate;
}

int ConfigUser::getCacheExpiry(const std::string& key) const {
    if (configJson.contains("cache_expiry_minutes") && configJson["cache_expiry_minutes"].contains(key)) {
        return configJson["cache_expiry_minutes"][key].get<int>();
    }
    return 60;
}

void ConfigUser::setCacheExpiry(const std::string& key, int minutes) {
    configJson["cache_expiry_minutes"][key] = minutes;
}

std::string ConfigUser::getCityId() const {
    return configJson.value("city_id", "");
}

void ConfigUser::setCityId(const std::string& id) {
    configJson["city_id"] = id;
}

std::string ConfigUser::getCityName() const {
    if (configJson.contains("location") && configJson["location"].contains("name")) {
        return configJson["location"]["name"].get<std::string>();
    }
    // 若不存在则 fallback 到 default_city
    return configJson.value("default_city", "北京");
}

std::string ConfigUser::getLocationId() const {
    return configJson["city_id"].get<std::string>();
}

