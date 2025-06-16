#include "base_config.h"
#include <fstream>
#include <iostream>

// 构造函数实现
BaseConfig::BaseConfig(const std::string& path) : configPath(path) {}

// load 函数实现
bool BaseConfig::load() {
    // 打开配置文件
    std::ifstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "❌ 无法打开配置文件: " << configPath << std::endl;
        return false;
    }

    try {
        file >> configJson;  // 读取 JSON 数据到 configJson
    } catch (const std::exception& e) {
        std::cerr << "❌ 配置文件解析错误: " << e.what() << std::endl;
        return false;
    }

    return true;
}

// save 函数实现
bool BaseConfig::save() {
    // 打开配置文件用于写入
    std::ofstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "❌ 无法写入配置文件: " << configPath << std::endl;
        return false;
    }

    try {
        // 将 configJson 以缩进格式写入文件
        file << configJson.dump(4);
    } catch (const std::exception& e) {
        std::cerr << "❌ 写入配置失败: " << e.what() << std::endl;
        return false;
    }

    return true;
}

// getConfigJson 函数实现
nlohmann::json BaseConfig::getConfigJson() const {
    return configJson;  // 返回 configJson
}
