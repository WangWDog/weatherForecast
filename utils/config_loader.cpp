#include "config_loader.h"
#include <fstream>
#include <iostream>

ConfigLoader::ConfigLoader(const std::string& path) : configPath(path) {}
std::string ConfigLoader::getCityId() const {
    return configJson.value("city_id", "");
}

void ConfigLoader::setCityId(const std::string& id) {
    configJson["city_id"] = id;
}
bool ConfigLoader::load() {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "无法打开配置文件: " << configPath << std::endl;
        return false;
    }

    try {
        file >> configJson;
    } catch (const std::exception& e) {
        std::cerr << "配置文件解析错误: " << e.what() << std::endl;
        return false;
    }

    return true;
}

bool ConfigLoader::save() const {
    std::ofstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "无法写入配置文件: " << configPath << std::endl;
        return false;
    }

    try {
        file << configJson.dump(4); // 美化缩进
    } catch (const std::exception& e) {
        std::cerr << "保存配置失败: " << e.what() << std::endl;
        return false;
    }

    return true;
}

std::string ConfigLoader::getDefaultCity() const {
    return configJson.value("default_city", "beijing");
}

void ConfigLoader::setDefaultCity(const std::string& city) {
    configJson["default_city"] = city;
}

bool ConfigLoader::getUseIPLocation() const {
    return configJson.value("use_ip_location", false);
}

void ConfigLoader::setUseIPLocation(bool useIP) {
    configJson["use_ip_location"] = useIP;
}

std::string ConfigLoader::getUnits() const {
    return configJson.value("units", "metric");
}

void ConfigLoader::setUnits(const std::string& units) {
    configJson["units"] = units;
}

std::string ConfigLoader::getLanguage() const {
    try
    {
        return configJson.value("language", "en");
    }catch (const std::exception& e)
    {
        std::cerr << "<UNK>: " << e.what() << std::endl;
    }
    return "en";
}

void ConfigLoader::setLanguage(const std::string& lang) {
    configJson["language"] = lang;
}

int ConfigLoader::getCacheExpiry(const std::string& key) const {
    if (configJson.contains("cache_expiry_minutes") && configJson["cache_expiry_minutes"].contains(key)) {
        return configJson["cache_expiry_minutes"][key].get<int>();
    }
    return 60; // 默认值
}

void ConfigLoader::setCacheExpiry(const std::string& key, int minutes) {
    configJson["cache_expiry_minutes"][key] = minutes;
}
