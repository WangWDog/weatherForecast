#include "CacheManager.h"
#include <chrono>
#include <fstream>
#include <iostream>

// 构造函数：初始化配置
CacheManager::CacheManager(const json& config)
    : configJson(config) {}

// 获取缓存过期时间：从配置读取，无则返回默认60分钟
int CacheManager::getCacheExpiry(const std::string& key) const {
    if (configJson.contains("cache_expiry_minutes") &&
        configJson["cache_expiry_minutes"].contains(key)) {
        return configJson["cache_expiry_minutes"][key].get<int>();
    }
    std::cerr << "Warning: Cache expiry for key '" << key
              << "' not found, using default 60 minutes." << std::endl;
    return 60;
}

void CacheManager::setCache(const std::string& key, const std::string& value, int expiryMinutes) {
    CacheEntry entry;
    entry.data = value;
    // 统一用 system_clock
    entry.timestamp = std::chrono::system_clock::now();
    cache[key] = entry;
    //std::cout << "[Cache DEBUG] 写入缓存: key=" << key
         //   << ", value=" << value
           // << ", 时间戳=" << std::chrono::system_clock::to_time_t(entry.timestamp) << std::endl;
}


std::string CacheManager::getCache(const std::string& key) {
    auto it = cache.find(key);
    if (it != cache.end()) {
        const auto& entry = it->second;
        auto elapsedMinutes = std::chrono::duration_cast<std::chrono::minutes>(
            // 同样用 system_clock
            std::chrono::system_clock::now() - entry.timestamp
        ).count();
        if (elapsedMinutes < getCacheExpiry(key)) {
            return entry.data;
        } else {
            cache.erase(it);
        }
    }
    return "";
}

// 清除所有缓存
void CacheManager::clearAllCache() {
    cache.clear();
}

// 清除指定键的缓存
void CacheManager::clearCache(const std::string& key) {
    cache.erase(key);
}

// 清空指定缓存文件（带错误处理）
void CacheManager::clearCacheFile(const std::string& filename) {
    std::ofstream file(filename, std::ofstream::trunc);
    if (file.is_open()) {
        file.close();
        std::cout << "成功清空文件: " << filename << std::endl;
    } else {
        std::cerr << "错误：无法打开文件 " << filename << std::endl;
    }
}

// 检查缓存是否过期（核心方法）
bool CacheManager::isCacheExpired(const std::string& key, int expiryMinutes) const {
    auto it = cache.find(key);
    if (it == cache.end()) return true;
    if (expiryMinutes <= 0) return true;

    // 现在两个时间点都是 system_clock::time_point，可以直接相减
    auto elapsedMinutes = std::chrono::duration_cast<std::chrono::minutes>(
        std::chrono::system_clock::now() - it->second.timestamp
    ).count();
    return elapsedMinutes >= expiryMinutes;
}


// 获取缓存时间戳（转换为time_t用于显示）
time_t CacheManager::getCacheTimestamp(const std::string& key) const {
    auto it = cache.find(key);
    if (it != cache.end()) {
        // 直接转换system_clock::time_point为time_t，无需强制类型转换
        return std::chrono::system_clock::to_time_t(it->second.timestamp);
    }
    return 0;
}