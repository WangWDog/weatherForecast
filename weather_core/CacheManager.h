#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <string>
#include <unordered_map>
#include <json.hpp>  // 使用 nlohmann::json 库
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

    // 清除所有缓存
    void clearAllCache();

    // 清除指定键的缓存
    void clearCache(const std::string& key);

    // 清空指定缓存文件内容
    void clearCacheFile(const std::string &filename);

private:
    // 缓存项结构体，存储数据和时间戳
    struct CacheEntry {
        std::string data;  // 缓存数据
        std::chrono::steady_clock::time_point timestamp;  // 缓存时间戳
    };

    nlohmann::json configJson; // 配置JSON
    std::unordered_map<std::string, CacheEntry> cache; // 缓存存储
};

#endif // CACHEMANAGER_H
