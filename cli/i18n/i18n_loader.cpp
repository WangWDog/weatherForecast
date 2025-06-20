 #include "i18n_loader.h"
#include <fstream>
#include <iostream>
#include <direct.h>
#include <json.hpp>

using json = nlohmann::json;

bool I18n::load(const std::string& language) {
    translations.clear();
    std::string path = "i18n/lang_" + language + ".json";
    std::ifstream file(path);

    // 简洁输出：只显示语言代码和状态
    std::cout << "[I18n] 加载语言：" << language << std::endl;

    if (!file.is_open()) {
        char cwd[1024];
        _getcwd(cwd, sizeof(cwd));
        std::cerr << "❌ 无法打开语言文件: " << path << std::endl;
        std::cerr << "📂 当前工作目录: " << cwd << std::endl;
        return false;
    }

    json j;
    try {
        file >> j;
    } catch (const std::exception& e) {
        std::cerr << "❌ JSON 解析失败: " << e.what() << std::endl;
        return false;
    }

    // flatten 翻译内容
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

    std::cout << "[I18n] ✅ 语言文件加载成功（" << translations.size() << " 条目）" << std::endl;
    return true;
}
