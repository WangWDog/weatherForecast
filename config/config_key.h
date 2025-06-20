#pragma once
#include "base_config.h"
#include <string>
#include <../utils/json.hpp>

class ConfigKey : public BaseConfig {
public:// 显式构造函数（避免隐式转换）
    // path - 配置文件路径，默认值 "configKey.json"
    explicit ConfigKey(const std::string& path = "configKey.json");

    // API 密钥获取方法
    std::string getHFApiKey() const;        // 获取和风天气API密钥
    std::string getFreeApiKey() const;      // 获取免费API密钥
    std::string getHFHost() const;          // 获取和风天气API主机地址
    std::string getDoubaoEndpoint() const;  // 获取豆包AI服务端点ID
    std::string getDoubaoKey() const;       // 获取豆包AI服务API密钥
    std::string getQWeatherKey() const;     // 获取QWeather服务API密钥
    std::string getQWeatherHost() const;    // 获取QWeather服务主机地址

    // 设置方法
    void setApiKey(const std::string& key);  // 设置通用API密钥
    void setHost(const std::string& host);   // 设置通用主机地址

private:
    nlohmann::json configData;  // 🔥 必须有这行！存储配置数据的JSON对象
};
