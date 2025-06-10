#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <string>
#include <unordered_map>
#include <json.hpp>      // nlohmann/json 库
#include <chrono>
#include <ctime>         // 用于 time_t

using json = nlohmann::json;

class CacheManager {
public:
    // 构造函数：用配置初始化
    explicit CacheManager(const json& config);

    // 获取缓存过期时间（从配置里读，示例逻辑，需根据实际配置结构改）
    int getCacheExpiry(const std::string& key) const;

    // 获取缓存数据（带过期检查逻辑）
    std::string getCache(const std::string& key);

    // 设置缓存数据（记录时间戳）
    void setCache(const std::string& key, const std::string& value, int expiryMinutes = 0);

    // 清除所有缓存
    void clearAllCache();

    // 清除指定键的缓存
    void clearCache(const std::string& key);

    // 清空指定缓存文件（如果需要持久化，需结合文件操作实现）
    void clearCacheFile(const std::string& filename);

    // 🔥 检查缓存是否过期（核心方法）
    bool isCacheExpired(const std::string& key, int expiryMinutes) const;

    // 🔥 获取缓存时间戳
    time_t getCacheTimestamp(const std::string& key) const;

private:
    // 缓存项结构体：存数据和时间戳
    struct CacheEntry {
        std::string data;
        // 统一为 system_clock 的时间点
        std::chrono::system_clock::time_point timestamp;
    };
    json configJson;  // 配置内容
    std::unordered_map<std::string, CacheEntry> cache;  // 缓存存储（统一用这个）
};

#endif // CACHEMANAGER_H