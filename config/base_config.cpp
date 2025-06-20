#include "base_config.h"
#include <fstream>
#include <iostream>

// 构造函数实现
BaseFile::BaseFile(const std::string& path) : filePath(path) {}

// load 函数实现
bool BaseFile::load() {
    // 打开配置文件
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "❌ 无法打开配置文件: " << filePath << std::endl;
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
bool BaseFile::save() {
    // 打开配置文件用于写入
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "❌ 无法写入配置文件: " << filePath << std::endl;
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
nlohmann::json BaseFile::getConfigJson() const {
    return configJson;  // 返回 configJson
}
