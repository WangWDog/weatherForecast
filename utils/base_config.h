#pragma once
#include <string>
#include <json.hpp>

class BaseConfig {
public:
    // 构造函数声明
    explicit BaseConfig(const std::string& path);

    // 虚拟函数声明
    virtual bool load();
    virtual bool save();

    // 获取配置 JSON 的方法
    nlohmann::json getConfigJson() const;

protected:
    std::string configPath;        // 配置文件路径
    nlohmann::json configJson;     // 配置内容（JSON 格式）
};
