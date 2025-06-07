#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <string>
#include <unordered_map>
#include <json.hpp>
#include <chrono>

class CacheManager {
public:
    CacheManager(const nlohmann::json& config);

    // 获取缓存过期时间
    int getCacheExpiry(const std::string& key) const;

    // 获取缓存数据
    std::string getCache(const std::string& key);

    // 设置缓存数据
    void setCache(const std::string& key, const std::string& value);

private:
    struct CacheEntry {
        std::string data;
        std::chrono::steady_clock::time_point timestamp;
    };

    nlohmann::json configJson; // 配置JSON
    std::unordered_map<std::string, CacheEntry> cache; // 缓存存储
};

#endif // CACHEMANAGER_H
