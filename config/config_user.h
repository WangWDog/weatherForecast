#pragma once
#include "base_config.h"

class ConfigUser : public BaseConfig {
public:
    // 显式构造函数：指定配置文件路径（默认值："configUser.json"）
    explicit ConfigUser(const std::string& path = "configUser.json");

    // 获取默认城市名称
    std::string getDefaultCity() const;
    // 设置默认城市名称
    void setDefaultCity(const std::string& city);

    // 检查是否使用IP定位
    bool getUseIPLocation() const;
    // 设置是否使用IP定位
    void setUseIPLocation(bool useIP);

    // 获取温度单位（公制/英制）
    std::string getUnits() const;
    // 设置温度单位
    void setUnits(const std::string& units);

    // 获取当前语言设置
    std::string getLanguage() const;
    // 设置语言
    void setLanguage(const std::string& lang);

    // 获取特定缓存类型的过期时间（分钟）
    int getCacheExpiry(const std::string& key) const;
    // 设置特定缓存类型的过期时间
    void setCacheExpiry(const std::string& key, int minutes);

    // 获取日期格式字符串
    std::string getDateFormateMenu() const;
    // 设置日期格式
    void setDateFormateMenu(const std::string& dateFormate);

    // 获取城市ID（如气象API使用的编码）
    std::string getCityId() const;
    // 设置城市ID
    void setCityId(const std::string& id);

    // 获取城市名称（带fallback机制）
    std::string getCityName() const;
    // 获取位置ID（城市ID别名）
    std::string getLocationId() const;

};
