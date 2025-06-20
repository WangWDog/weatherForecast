#pragma once

#include "config_user.h"
#include "config_key.h"

class ConfigContext {
public:
    // 构造函数：初始化配置上下文
    // 参数：
    //   userPath - 用户配置文件的路径（默认值："configUser.json"）
    //   keyPath  - API密钥配置文件的路径（默认值："configKey.json"）
    ConfigContext(const std::string& userPath = "configUser.json",
                  const std::string& keyPath  = "configKey.json");

    // 获取用户配置对象的引用
    ConfigUser& user();

    // 获取API密钥配置对象的引用
    ConfigKey& key();

private:
    ConfigUser configUser;  // 用户配置对象（存储语言、城市ID等用户设置）
    ConfigKey configKey;    // API密钥配置对象（存储天气服务的API密钥等敏感信息）
};
