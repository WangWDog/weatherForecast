
#include "config_context.h"

ConfigContext::ConfigContext(const std::string& userPath, const std::string& keyPath)
    : configUser(userPath),  // 初始化configUser成员，传递userPath参数
      configKey(keyPath)    // 初始化configKey成员，传递keyPath参数
{
    // 加载用户配置
    configUser.load();  // 调用ConfigUser类的load()方法加载配置文件

    // 加载密钥配置
    configKey.load();   // 调用ConfigKey类的load()方法加载配置文件
}

// 获取用户配置的引用
ConfigUser& ConfigContext::user() {
    return configUser;  // 返回configUser成员的引用
}

// 获取密钥配置的引用
ConfigKey& ConfigContext::key() {
    return configKey;  // 返回configKey成员的引用
}
