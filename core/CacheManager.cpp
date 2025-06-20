#include "CacheManager.h"
#include <fstream>
#include <chrono>
#include <ctime>
#include <iostream>

using json = nlohmann::json;
using namespace std::chrono;

CacheManager::CacheManager(const std::string& path) : BaseFile(path) {
    load();  // 初始化时加载文件
}

void CacheManager::setCache(const std::string& key, const json& data, bool autoSave) {
    configJson[key] = {
        {"data", data},
        {"timestamp", std::time(nullptr)}
    };
    if (autoSave) save();
}

json CacheManager::getCache(const std::string& key) const {
    if (configJson.contains(key) && configJson[key].contains("data")) {
        return configJson[key]["data"];
    }
    return json();  // 空对象
}

bool CacheManager::isValid(const std::string& key, int expiryMinutes) const {
    if (!configJson.contains(key) || !configJson[key].contains("timestamp")) return false;
    time_t ts = configJson[key]["timestamp"];
    return (std::time(nullptr) - ts) < (expiryMinutes * 60);
}

time_t CacheManager::getTimestamp(const std::string& key) const {
    if (configJson.contains(key) && configJson[key].contains("timestamp")) {
        return configJson[key]["timestamp"];
    }
    return 0;
}

void CacheManager::clear(const std::string& key) {
    configJson.erase(key);
    save();
}

void CacheManager::clearAll() {
    configJson.clear();
    save();
}
