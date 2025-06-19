#include "i18n_loader.h"
#include <fstream>
#include <iostream>
#include <direct.h>
#include <json.hpp>

using json = nlohmann::json;

// load 方法：加载语言文件
bool I18n::load(const std::string& language) {
    // 构建语言文件路径：i18n/lang_<语言代码>.json
    std::string path = "i18n/lang_" + language + ".json";
    // 创建文件输入流
    std::ifstream file(path);

    // 调试输出：打印正在加载的文件路径
    std::cout << "[DEBUG] 正在加载语言文件路径: " << path << std::endl;

    // 检查文件是否成功打开
    if (!file.is_open()) {
        char cwd[1024];
        _getcwd(cwd, sizeof(cwd)); // Windows获取当前工作目录
        // 输出详细的错误信息
        std::cerr << "❌ 无法打开语言文件: " << path << std::endl;
        std::cerr << "当前工作目录: " << cwd << std::endl;
        return false; // 返回加载失败
    }

    json j;
    try {
        file >> j; // 从文件流解析JSON数据
    } catch (const std::exception& e) {
        // JSON解析失败处理
        std::cerr << "❌ JSON 解析失败: " << e.what() << std::endl;
        return false; // 返回加载失败
    }

    translations.clear(); // 清空现有的翻译映射

    // 递归扁平化嵌套结构，生成类似 "main_cli.prompt_input" 的键
    std::function<void(const std::string&, const json&)> flatten;
    flatten = [&](const std::string& prefix, const json& value) {
        if (value.is_object()) {
            // 遍历JSON对象的所有键值对
            for (auto it = value.begin(); it != value.end(); ++it) {

                // 构建新的键名：prefix + "." + key
                std::string newKey = prefix.empty() ? it.key() : prefix + "." + it.key();
                // 递归处理嵌套值
                flatten(newKey, it.value());
            }
        } else {
            // 处理叶子节点（非对象值）：如果是字符串直接获取，否则转换为JSON字符串
            translations[prefix] = value.is_string() ? value.get<std::string>() : value.dump();
        }
    };

    flatten("", j); // 从根节点开始扁平化处理
    return true; // 返回加载成功
}

// tr 方法：获取翻译文本
std::string I18n::tr(const std::string& key) const{
    // 在翻译映射中查找键
    auto it = translations.find(key);
    // 如果找到返回翻译文本，否则返回缺失标记
    return it != translations.end() ? it->second : "[missing: " + key + "]";
}

std::string I18n::tr(const std::string& section, const std::string& key) const {
    // 组合section和key为完整键名
    std::string fullKey = section + "." + key;
    // 调用单参数版本进行查找
    return tr(fullKey);
}

std::vector<std::string> I18n::trList(const std::string& section, const std::string& key) {
    std::vector<std::string> result;
    auto it = translations.find(section + "." + key);
    if (it != translations.end()) {
        try {
            // 尝试将存储的字符串解析为JSON数组
            json parsed = json::parse(it->second);
            if (parsed.is_array()) {
                for (auto& val : parsed) {
                    if (val.is_string()) result.push_back(val.get<std::string>());
                }
            }
        } catch (...) {
            // JSON解析失败处理
            std::cerr << "[warn] trList failed to parse key: " << section + "." + key << std::endl;
        }
    } else {
        // 键名缺失处理
        std::cerr << "[missing: " << section + "." + key << "]" << std::endl;
    }
    return result; // 返回结果列表（可能为空）
}
