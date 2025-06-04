#include "i18n_loader.h"
#include <fstream>
#include <iostream>
#include <direct.h>
#include <json.hpp>

using json = nlohmann::json;

bool I18n::load(const std::string& language) {
    std::string path = "i18n/lang_" + language + ".json";
    std::ifstream file(path);

    // 调试输出：打印正在加载的文件路径
    std::cout << "[DEBUG] 正在加载语言文件路径: " << path << std::endl;

    if (!file.is_open()) {
        char cwd[1024];
        _getcwd(cwd, sizeof(cwd));
        std::cerr << "❌ 无法打开语言文件: " << path << std::endl;
        std::cerr << "当前工作目录: " << cwd << std::endl;
        return false;
    }

    json j;
    try {
        file >> j;
    } catch (const std::exception& e) {
        std::cerr << "❌ JSON 解析失败: " << e.what() << std::endl;
        return false;
    }

    translations.clear();

    // 递归扁平化嵌套结构，生成类似 "main_cli.prompt_input" 的键
    std::function<void(const std::string&, const json&)> flatten;
    flatten = [&](const std::string& prefix, const json& value) {
        if (value.is_object()) {
            for (auto it = value.begin(); it != value.end(); ++it) {
                std::string newKey = prefix.empty() ? it.key() : prefix + "." + it.key();
                flatten(newKey, it.value());
            }
        } else {
            translations[prefix] = value.is_string() ? value.get<std::string>() : value.dump();
        }
    };

    flatten("", j);
    return true;
}

std::string I18n::tr(const std::string& key) {
    auto it = translations.find(key);
    return it != translations.end() ? it->second : "[missing: " + key + "]";
}

std::string I18n::tr(const std::string& section, const std::string& key) {
    std::string fullKey = section + "." + key;
    return tr(fullKey);
}

std::vector<std::string> I18n::trList(const std::string& section, const std::string& key) {
    std::vector<std::string> result;
    auto it = translations.find(section + "." + key);
    if (it != translations.end()) {
        try {
            json parsed = json::parse(it->second);
            if (parsed.is_array()) {
                for (auto& val : parsed) {
                    if (val.is_string()) result.push_back(val.get<std::string>());
                }
            }
        } catch (...) {
            std::cerr << "[warn] trList failed to parse key: " << section + "." + key << std::endl;
        }
    } else {
        std::cerr << "[missing: " << section + "." + key << "]" << std::endl;
    }
    return result;
}
