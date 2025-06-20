
#include "config_key.h"

// 初始化配置路径，调用基类BaseConfig的构造函数，传递配置文件路径
ConfigKey::ConfigKey(const std::string& path) : BaseConfig(path) {}

// 获取和风天气API密钥
std::string ConfigKey::getHFApiKey() const {
    return configJson.value("key_hf", "");
}

// 获取免费API密钥
std::string ConfigKey::getFreeApiKey() const {
    return configJson.value("key_free", "");
}

// 设置通用API密钥
void ConfigKey::setApiKey(const std::string& key) {
    configJson["key"] = key;
}

// 获取和风天气API主机地址
std::string ConfigKey::getHFHost() const {
    return configJson.value("host_hf", "");
}

// 获取豆包AI服务的端点ID
std::string ConfigKey::getDoubaoEndpoint() const {

        return configJson.value("doubao_endpoint_id", "");
}

// 获取豆包AI服务的API密钥
std::string ConfigKey::getDoubaoKey() const {
    return configJson.value("doubao_key", "");
}

// 设置通用主机地址
void ConfigKey::setHost(const std::string& host) {
    configJson["host"] = host;
}

// 获取QWeather服务的API密钥（复用免费密钥）
std::string ConfigKey::getQWeatherKey() const {
    return configJson.value("key_free", "");
}

// 获取QWeather服务的主机地址（复用和风主机）
std::string ConfigKey::getQWeatherHost() const {
    return configJson.value("host_hf", "");
}
