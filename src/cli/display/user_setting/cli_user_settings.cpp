//
// Created by 13033 on 2025/6/16.

#include "cli_user_settings.h"

#include <fstream>
#include <iostream>

#include "CacheManager.h"
#include "../common/cli_clear_console.h"

void updateUserSettings(ConfigUser &configUser, I18n &i18n) {//configUser:封装用户设置。il8n:国际化翻译
    while (true) {
        clearConsole();
        std::cout << "🔧 " << i18n.tr("settings", "menu_title") << "\n";
        std::cout << "------------------------\n";
        std::cout << "1. 📅 " << i18n.tr("settings", "date_format") << "（" << configUser.getDateFormateMenu() << "）\n";
        std::cout << "2. 🧭 " << i18n.tr("settings", "cache_life_index") << "（" << configUser.
                getCacheExpiry("life_index") << " 分钟）\n";
        std::cout << "3. 🌦 " << i18n.tr("settings", "cache_forecast") << "（" << configUser.
                getCacheExpiry("daily_forecast") << " 分钟）\n";
        std::cout << "4. 🌡 " << i18n.tr("settings", "units") << "（" << configUser.getUnits() << "）\n";
        std::cout << "5. 🈯 " << i18n.tr("settings", "language") << "（" << configUser.getLanguage() << "） \n";
        std::cout << "6. 🔙 " << i18n.tr("settings", "back") << "\n";//输出菜单选项

        std::cout << i18n.tr("settings", "prompt_input");
        std::string choice;
        std::getline(std::cin, choice);//获取用户输入

        if (choice == "1") {
            std::string fmt;
            std::cout << i18n.tr("settings", "input_date_format");
            std::getline(std::cin, fmt);
            if (fmt != ":q") {
                configUser.setDateFormateMenu(fmt);
                std::cout << i18n.tr("settings", "updated") << "\n";
            } else {
                std::cout << i18n.tr("settings", "cancelled") << "\n";
            }
        }//修改日期格式
        else if (choice == "2") {
            std::string input;
            std::cout << i18n.tr("settings", "input_cache_life");
            std::getline(std::cin, input);
            if (input != ":q") {
                try {
                    int mins = std::stoi(input);
                    configUser.setCacheExpiry("life_index", mins);
                    std::cout << i18n.tr("settings", "updated") << "\n";
                } catch (...) {
                    std::cout << i18n.tr("settings", "invalid_input") << "\n";
                }
            } else {
                std::cout << i18n.tr("settings", "cancelled") << "\n";
            }
        }//修改生活指数缓存时间
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
        }//修改天气预报缓存时间
        else if (choice == "4") {
            std::string u;
            std::cout << i18n.tr("settings", "input_units");
            std::getline(std::cin, u);
            if (u != ":q") {
                configUser.setUnits(u);
                std::cout << i18n.tr("settings", "updated") << "\n";
            } else {
                std::cout << i18n.tr("settings", "cancelled") << "\n";
            }
        }
        else if (choice == "5")
        {
            std::string lang;
            std::cout << i18n.tr("settings", "input_language");
            std::getline(std::cin, lang);
            if (lang != ":q") {
                configUser.setLanguage(lang);
                std::cout << i18n.tr("settings", "updated") << "\n";

                // ✅ 正确地使用路径初始化 CacheManager
                CacheManager cacheManager("configs/cache.json");
                cacheManager.clearAll();

                if (!i18n.load(lang)) {
                    std::cout << i18n.tr("settings", "language_load_fail") << "\n";
                } else {
                    std::cout << i18n.tr("settings", "language_load_success") << " " << lang << "\n";
                }
            } else {
                std::cout << i18n.tr("settings", "cancelled") << "\n";
            }
            continue;
        }//修改语言
        else if (choice == "6") {
            configUser.save();
            std::cout << i18n.tr("settings", "saved_and_exit") << "\n";
            return;
        } else {
            std::cout << i18n.tr("settings", "invalid_option") << "\n";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}
