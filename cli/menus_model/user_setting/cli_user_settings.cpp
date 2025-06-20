//
// Created by 13033 on 2025/6/16.

#include "cli_user_settings.h"

#include <fstream>
#include <iostream>

#include "CacheManager.h"
#include "../common/cli_clear_console.h"


// 用户设置界面实现
void updateUserSettings(ConfigUser &configUser, I18n &i18n) {//configUser:封装用户设置。il8n:国际化翻译
    // 无限循环，直到用户选择退出
    while (true) {
        clearConsole();  // 清空控制台

        // 显示设置菜单标题
        std::cout << "🔧 " << i18n.tr("settings", "menu_title") << "\n";
        std::cout << "------------------------\n";

        // 显示各项设置选项及其当前值
        std::cout << "1. 📅 " << i18n.tr("settings", "date_format") << "（" << configUser.getDateFormateMenu() << "）\n";
        std::cout << "2. 🧭 " << i18n.tr("settings", "cache_life_index") << "（" << configUser.
                getCacheExpiry("life_index") << " 分钟）\n";
        std::cout << "3. 🌦 " << i18n.tr("settings", "cache_forecast") << "（" << configUser.
                getCacheExpiry("daily_forecast") << " 分钟）\n";
        std::cout << "4. 🈯 " << i18n.tr("settings", "language") << "（" << configUser.getLanguage() << "） \n";
        std::cout << "5. 🔙 " << i18n.tr("settings", "back") << "\n";  // 输出五个菜单选项

        // 提示用户输入选择
        std::cout << i18n.tr("settings", "prompt_input");
        std::string choice;
        std::getline(std::cin, choice);  // 获取用户输入

        // 处理日期格式设置
        if (choice == "1") {
            std::string fmt;
            std::cout << i18n.tr("settings", "input_date_format");
            std::getline(std::cin, fmt);

            // 检查是否取消操作
            if (fmt != ":q") {
                configUser.setDateFormateMenu(fmt);  // 更新日期格式
                std::cout << i18n.tr("settings", "updated") << "\n";
            } else {
                std::cout << i18n.tr("settings", "cancelled") << "\n";
            }
        }
        // 处理生活指数缓存时间设置，修改日期格式
        else if (choice == "2") {
            std::string input;
            std::cout << i18n.tr("settings", "input_cache_life");
            std::getline(std::cin, input);
            if (input != ":q") {
                try {
                    int mins = std::stoi(input);  // 转换输入为整数
                    configUser.setCacheExpiry("life_index", mins);  // 更新缓存时间
                    std::cout << i18n.tr("settings", "updated") << "\n";
                } catch (...) {  // 捕获所有转换异常
                    std::cout << i18n.tr("settings", "invalid_input") << "\n";
                }
            } else {
                std::cout << i18n.tr("settings", "cancelled") << "\n";
            }
        }  // 处理天气预报缓存时间设置，修改生活指数缓存时间
        else if (choice == "3") {
            std::string input;
            std::cout << i18n.tr("settings", "input_cache_forecast");
            std::getline(std::cin, input);
            if (input != ":q") {
                try {
                    int mins = std::stoi(input);
                    configUser.setCacheExpiry("daily_forecast", mins);
                    std::cout << i18n.tr("settings", "updated") << "\n";
                } catch (...) {
                    std::cout << i18n.tr("settings", "invalid_input") << "\n";
                }
            } else {
                std::cout << i18n.tr("settings", "cancelled") << "\n";
            }
        }
        // 处理语言设置
        else if (choice == "4") {
            std::string lang;
            std::cout << i18n.tr("settings", "input_language");
            std::getline(std::cin, lang);  // 设置新语言

            if (lang != ":q") {
                configUser.setLanguage(lang);
                std::cout << i18n.tr("settings", "updated") << "\n";

                // 加载缓存配置文件 cache.json
                std::ifstream configFile("cache.json");
                nlohmann::json configJson;

                // 检查文件是否成功打开
                if (configFile.is_open()) {
                    configFile >> configJson;  // 读取配置文件内容到 configJson
                    configFile.close();
                } else {
                    std::cerr << "无法打开配置文件 cache.json!" << std::endl;
                    return;
                }

                // 实例化 CacheManager 对象
                CacheManager cacheManager(configJson);  // 使用加载的配置文件对象初始化 CacheManager

                // 清空缓存文件和所有缓存，每次都会执行
                cacheManager.clearCacheFile("cache.json");  // 清除缓存文件
                cacheManager.clearAllCache();              // 清除内存缓存

                // 尝试加载新语言
                if (!i18n.load(lang)) {
                    std::cout << i18n.tr("settings", "language_load_fail") << "\n";
                } else {
                    std::cout << i18n.tr("settings", "language_load_success") << " " << lang << "\n";
                }
            } else {
                std::cout << i18n.tr("settings", "cancelled") << "\n";
            }
            continue;  // 跳过最后的cin.ignore()
        }
        // 退出设置界面
        else if (choice == "5") {
            configUser.save();  // 保存所有设置更改
            std::cout << i18n.tr("settings", "saved_and_exit") << "\n";
            return;  // 退出函数
        }
        // 处理无效输入
        else {
            std::cout << i18n.tr("settings", "invalid_option") << "\n";
        }

        // 清除输入缓冲区中的多余字符
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}
