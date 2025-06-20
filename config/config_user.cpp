#include "config_user.h"

// 构造函数：初始化配置文件路径
ConfigUser::ConfigUser(const std::string& path)
    : BaseConfig(path) {}  // 调用基类构造函数传递路径

// 获取默认城市名称
std::string ConfigUser::getDefaultCity() const {
    // 从JSON获取"default_city"字段，不存在则返回"beijing"
    return configJson.value("default_city", "beijing");
}

// 设置默认城市名称
void ConfigUser::setDefaultCity(const std::string& city) {
    configJson["default_city"] = city;  // 设置JSON字段
}

// 检查是否使用IP定位
bool ConfigUser::getUseIPLocation() const {
    // 获取"use_ip_location"字段，默认false
    return configJson.value("use_ip_location", false);
}

// 设置是否使用IP定位
void ConfigUser::setUseIPLocation(bool useIP) {
    configJson["use_ip_location"] = useIP;  // 设置JSON字段
}

// 获取温度单位（公制/英制）
std::string ConfigUser::getUnits() const {
    // 获取"units"字段，默认公制("metric")
    return configJson.value("units", "metric");
}

// 设置温度单位
void ConfigUser::setUnits(const std::string& units) {
    configJson["units"] = units;  // 设置JSON字段
}

// 获取语言设置
std::string ConfigUser::getLanguage() const {
    // 获取"language"字段，默认中文("zh")
    return configJson.value("language", "zh");
}

// 设置语言
void ConfigUser::setLanguage(const std::string& lang) {
    configJson["language"] = lang;  // 设置JSON字段
}

// 获取日期格式
std::string ConfigUser::getDateFormateMenu() const {
    // 获取"date_formate_menu"字段，默认格式"%Y-%m-%d %H:%M:%S"
    return configJson.value("date_formate_menu", "%Y-%m-%d %H:%M:%S");
}

// 设置日期格式
void ConfigUser::setDateFormateMenu(const std::string& dateFormate) {
    configJson["date_formate_menu"] = dateFormate;  // 设置JSON字段
}

// 获取指定缓存类型的过期时间（分钟）
int ConfigUser::getCacheExpiry(const std::string& key) const {
    // 检查嵌套JSON结构是否存在
    if (configJson.contains("cache_expiry_minutes") &&
        configJson["cache_expiry_minutes"].contains(key)) {
        // 返回指定缓存类型的过期时间
        return configJson["cache_expiry_minutes"][key].get<int>();
    }
    return 60;  // 默认60分钟
}

// 设置指定缓存类型的过期时间
void ConfigUser::setCacheExpiry(const std::string& key, int minutes) {
    // 设置嵌套JSON结构中的值
    configJson["cache_expiry_minutes"][key] = minutes;
}

// 获取城市ID
std::string ConfigUser::getCityId() const {
    // 获取"city_id"字段，不存在则返回空字符串
    return configJson.value("city_id", "");
}

// 设置城市ID
void ConfigUser::setCityId(const std::string& id) {
    configJson["city_id"] = id;  // 设置JSON字段
}

// 获取城市名称（带fallback机制）
std::string ConfigUser::getCityName() const {
    // 检查嵌套结构 location → name
    if (configJson.contains("location") &&
        configJson["location"].contains("name")) {
        return configJson["location"]["name"].get<std::string>();
    }
    // 回退到默认城市名称，默认"北京"
    return configJson.value("default_city", "北京");
}

// 获取位置ID（城市ID别名）
std::string ConfigUser::getLocationId() const {
    // 直接获取"city_id"字段，假设一定存在
    return configJson["city_id"].get<std::string>();
}
