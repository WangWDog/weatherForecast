#include "CacheManager.h"
#include <chrono>
#include <fstream>  // 添加文件流头文件
#include <iostream>

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

// 清除所有缓存
void CacheManager::clearAllCache() {
    cache.clear();
}

// 清除指定键的缓存
void CacheManager::clearCache(const std::string& key) {
    auto it = cache.find(key);
    if (it != cache.end()) {
        cache.erase(it); // 删除指定键的缓存
    }
}

// 清空指定缓存文件内容
void CacheManager::clearCacheFile(const std::string& filename) {
    // 打开文件并清空内容
    std::ofstream file(filename, std::ofstream::trunc);

    if (file.is_open()) {
        file.close();
        std::cout << "文件 " << filename << " 已清空!" << std::endl;
    } else {
        std::cerr << "无法打开文件 " << filename << "!" << std::endl;
    }
}
