#pragma once
#include "base_config.h"
#include "json.hpp"

class CacheManager : public BaseFile {
public:
    CacheManager(const std::string& path);

    // 设置缓存（含时间戳）
    void setCache(const std::string& key, const nlohmann::json& data, bool autoSave = true);

    // 获取缓存内容
    nlohmann::json getCache(const std::string& key) const;

    // 判断是否存在未过期缓存
    bool isValid(const std::string& key, int expiryMinutes) const;

    // 获取缓存时间戳
    time_t getTimestamp(const std::string& key) const;

    // 清除缓存
    void clear(const std::string& key);
    void clearAll();
};
