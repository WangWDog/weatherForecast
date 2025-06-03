#pragma once
#include <string>
#include "json.hpp"

class ConfigLoader {
public:
    explicit ConfigLoader(const std::string& config_path = "config.json");

    bool load();           // 加载配置
    bool save() const;     // 保存配置

    // 基本字段访问器
    std::string getDefaultCity() const;
    void setDefaultCity(const std::string& city);

    bool getUseIPLocation() const;
    void setUseIPLocation(bool useIP);

    std::string getUnits() const;
    void setUnits(const std::string& units);

    std::string getLanguage() const;
    void setLanguage(const std::string& lang);

    // 嵌套字段：每类天气的缓存时间（分钟）
    int getCacheExpiry(const std::string& key) const;
    void setCacheExpiry(const std::string& key, int minutes);

private:
    std::string configPath;
    nlohmann::json configJson;
};
