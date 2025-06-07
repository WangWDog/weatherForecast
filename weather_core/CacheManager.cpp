#include "CacheManager.h"
#include <chrono>

CacheManager::CacheManager(const nlohmann::json& config) : configJson(config) {}

int CacheManager::getCacheExpiry(const std::string& key) const {
    if (configJson.contains("cache_expiry_minutes") && configJson["cache_expiry_minutes"].contains(key)) {
        return configJson["cache_expiry_minutes"][key].get<int>();
    }
    return 60; // 默认缓存过期时间为60分钟
}

std::string CacheManager::getCache(const std::string& key) {
    auto it = cache.find(key);
    if (it != cache.end()) {
        auto& entry = it->second;
        // 检查缓存是否过期
        if (std::chrono::duration_cast<std::chrono::minutes>(std::chrono::steady_clock::now() - entry.timestamp).count() < getCacheExpiry(key)) {
            return entry.data;
        } else {
            cache.erase(it); // 如果缓存过期，删除缓存
        }
    }
    return "";
}

void CacheManager::setCache(const std::string& key, const std::string& value) {
    CacheEntry entry;
    entry.data = value;
    entry.timestamp = std::chrono::steady_clock::now();
    cache[key] = entry;
}
