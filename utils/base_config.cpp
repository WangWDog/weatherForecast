#include "base_config.h"
#include <fstream>
#include <iostream>

BaseConfig::BaseConfig(const std::string& path) : configPath(path) {}

bool BaseConfig::load() {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "❌ 无法打开配置文件: " << configPath << std::endl;
        return false;
    }

    try {
        file >> configJson;
    } catch (const std::exception& e) {
        std::cerr << "❌ 配置文件解析错误: " << e.what() << std::endl;
        return false;
    }

    return true;
}

bool BaseConfig::save() {
    std::ofstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "❌ 无法写入配置文件: " << configPath << std::endl;
        return false;
    }

    try {
        file << configJson.dump(4);
    } catch (const std::exception& e) {
        std::cerr << "❌ 写入配置失败: " << e.what() << std::endl;
        return false;
    }

    return true;
}