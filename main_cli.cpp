#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <fstream>
#include <json.hpp>
#include <fcntl.h>
#include <io.h>
#include <conio.h>
#include <windows.h>
#include <curl/curl.h>
#include <fstream>

#include "cli_animation_loader.h"
#include "cli/displayUtils/cli_clear_console.h"
#include "config/config_key.h"
#include "config/config_loader.h"
#include "config/config_user.h"
#include "delay.h"
#include "updateCity.h"
#include "core/weather_manager.h"
#include "i18n/i18n_loader.h"
#include "lunar_api.h"
#include "config/config_key.h"
#include "doubao_translator.h"
#include "date_utils.h"  // 包含辅助函数头文件
#include "doubao_helper.h"  // 调用豆包函数
#include "core/CacheManager.h"





//#ifdef _WIN32
#include <windows.h>

#include "cli_application.h"
//#endif

using json = nlohmann::json;



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
        std::cout << "4. 🈯 " << i18n.tr("settings", "language") << "（" << configUser.getLanguage() << "） \n";
        std::cout << "5. 🔙 " << i18n.tr("settings", "back") << "\n";//输出五个菜单选项

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
            std::string lang;
            std::cout << i18n.tr("settings", "input_language");
            std::getline(std::cin, lang);

            if (lang != ":q") {
                configUser.setLanguage(lang);
                std::cout << i18n.tr("settings", "updated") << "\n";

                // 加载配置文件 cache.json
                std::ifstream configFile("cache.json");
                nlohmann::json configJson;
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
                cacheManager.clearCacheFile("cache.json");
                cacheManager.clearAllCache();

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
        else if (choice == "5") {
            configUser.save();
            std::cout << i18n.tr("settings", "saved_and_exit") << "\n";
            return;
        } else {
            std::cout << i18n.tr("settings", "invalid_option") << "\n";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}
// 显示当前日期





void displayWeatherCommander(ConfigUser &configUser, ConfigKey &configKey, I18n &i18n, bool forceRefresh) {
    WeatherManager manager(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());
    ForecastResult result;
    if (forceRefresh) {
        result = manager.get7DayForecast(configUser.getCityId(), configUser.getLanguage(), 0);
    } else {
        result = manager.get7DayForecast(configUser.getCityId(), configUser.getLanguage(),
                                         configUser.getCacheExpiry("daily_forecast"));
    }


    if (result.forecasts.empty()) {
        std::cout << i18n.tr("forecast", "fetch_failed") << std::endl;
        return;
    }

    displayWeather(result, i18n, configUser);
    std::cout << "\n" << i18n.tr("forecast", "commander_prompt_refresh") << "\n";
}


void showLifeIndices(ConfigUser &configUser, ConfigKey &configKey, I18n &i18n) {
    WeatherManager manager(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());

    // 强制刷新缓存
    CacheManager cacheManager(configUser.getConfigJson());
    cacheManager.clearAllCache();  // 清除所有缓存


    // 初次加载（尝试用缓存）
    auto result = manager.getLifeIndices(configUser.getCityId(), configUser.getCacheExpiry("weather_index"));

    if (result.indices.empty()) {
        std::cout << i18n.tr("life_index", "fetch_failed") << "\n";  // 使用翻译获取 "无法获取生活指数数据"
        return;
    }

    while (true) {
        clearConsole();

        // 缓存信息展示
        if (result.fromCache) {
            std::cout << i18n.tr("forecast", "from_cache") << "\n";  // 使用翻译获取 "当前数据来自缓存"
        } else {
            std::cout << i18n.tr("forecast", "from_network") << "\n";  // 使用翻译获取 "当前数据来自网络"
        }

        if (result.timestamp > 0) {
            char buf[64];
            std::tm *local = std::localtime(&result.timestamp);
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", local);
            std::cout << i18n.tr("forecast", "updated_time") << buf << "\n";  // 使用翻译获取 "数据更新时间"
        }

        // 生活指数展示
        std::cout << i18n.tr("life_index", "title") << configUser.getDefaultCity() << i18n.tr("life_index", "index_list") << "\n\n";  // 翻译 "城市：<city> 的生活指数如下："
        for (const auto &idx: result.indices) {
            std::cout << i18n.tr("life_index", "date") << ": " << idx.date << "\n"  // 翻译 "日期："
                      << i18n.tr("life_index", "type") << ": " << idx.name << "\n"  // 翻译 "类型："
                      << i18n.tr("life_index", "level") << ": " << idx.level << "（" << idx.category << "）\n"  // 翻译 "等级："
                      << i18n.tr("life_index", "suggestion") << ": " << idx.text << "\n"  // 翻译 "建议："
                      << "------------------------\n";
        }


        std::cout << "\n" << i18n.tr("life_index", "prompt_refresh") << "\n";  // 翻译 "按 R 刷新数据，任意其他键返回主菜单..."
        char ch = _getch();
        if (ch == 'R' || ch == 'r') {
            result = manager.getLifeIndices(configUser.getCityId(), 0); // 设置过期时间为 0 强制刷新
        } else {
            break;
        }
    }
}


void showCommandHelp() {
    std::cout << "Available commands:\n";
    std::cout << "  show_date        - Show current date and time\n";
    std::cout << "  show_date --all  - Show current time, date, and the Chinese Almanac\n";
    std::cout << "  show_forecast    - Show 7-day weather forecast\n";
    std::cout << "  show_life        - Show life indices for the city\n";
    std::cout << "  update_city      - Update the city setting\n";
    std::cout << "  update_settings  - Update user settings\n";
    std::cout << "  exit             - Exit the application\n";
}


void handleCommand(int argc, char *argv[], ConfigUser &configUser, ConfigKey &configKey, I18n &i18n) {
    std::string command = argv[1];

    if (command == "show_date") {
        bool showAll = false;
        if (argc > 2) {
            std::string type = argv[2];
            if (type.find("--all") != std::string::npos) {
                showAll = true; // 如果包含 --all，设置为 true
            }
        }
        // 判断是否传入 --all 参数
        std::string prompt = "当前日期和时间：";
        std::time_t t = std::time(nullptr);
        std::tm* timeInfo = std::localtime(&t);
        std::cout << prompt << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S") << std::endl;

        if (showAll) {
            // 如果 showAll 为 true，则还需展示黄历信息
            std::cout << "黄历信息：\n" << "今天是农历五月二十九，宜出行、结婚，不宜动土、安床。" << std::endl;
        }

    } else if (command == "show_forecast") {
        bool forceRefresh = false;
        if (argc > 2) {
            std::string type = argv[2];
            if (type.find("-R") != std::string::npos) {
                forceRefresh = true; // 如果包含 -R，设置为 true
            }
        }
        // 显示天气预报的代码
        std::cout << "显示7天天气预报" << std::endl;

    } else if (command == "show_life") {
        // 显示生活指数的代码
        std::cout << "显示生活指数" << std::endl;

    } else if (command == "update_city") {
        // 更新城市设置的代码
        std::cout << "更新城市设置" << std::endl;

    } else if (command == "update_settings") {
        // 更新用户设置的代码
        std::cout << "更新用户设置" << std::endl;

    } else if (command == "exit") {
        std::cout << i18n.tr("main_cli", "goodbye") << std::endl;
        delay_ms(2000);
        exit(0); // Exits the program

    } else {
        std::cout << "❌ Invalid command! Type 'help' for a list of commands." << std::endl;
    }
}




#include <iostream>
#include <string>
#include <cstdlib>  // 用于 std::exit()
#include <limits>   // 用于 std::numeric_limits

// int main(int argc, char *argv[]) {
// #ifdef _WIN32
//     SetConsoleOutputCP(CP_UTF8);
//     SetConsoleCP(CP_UTF8);
// #endif
//
//     ConfigUser configUser("configUser.json");
//     ConfigKey configKey("configKey.json");
//     I18n i18n;
//
//     if (!i18n.load("zh")) {  // 默认加载中文语言
//         std::cerr << "❌ 初始化失败，程序退出。\n";
//         return 1;
//     }
//
//     if (!configUser.load() || !configKey.load() || !i18n.load(configUser.getLanguage())) {
//         std::cerr << "❌ 初始化失败，程序退出。\n";
//         return 1;
//     }
//
//     std::cout << "Number of arguments: " << argc << std::endl;
//
//     // 输出每个参数
//     for (int i = 0; i < argc; ++i) {
//         std::cout << "Argument " << i << ": " << argv[i] << std::endl;
//     }
//
//     // 如果有命令行参数，则根据命令行参数执行相应操作
//     if (argc > 1) {
//         handleCommand(argc, argv, configUser, configKey, i18n);
//     } else {
//         // 没有命令行参数，则进入交互式菜单
//         showLoadingBar(i18n.tr("main_cli", "menu_loading"), 8, 40, "\033[38;5;117m");
//         while (true) {
//             clearConsole();
//
//             std::cout << "\n" << i18n.tr("main_cli", "menu_title") << "\n";
//             std::cout << "------------------------\n";
//             auto options = i18n.trList("main_cli", "menu_options");
//             for (size_t i = 0; i < options.size(); ++i) {
//                 std::cout << i<<". ";
//                 std::cout << options[i] << "\n";
//             }
//             std::cout << "------------------------\n";
//             std::cout << i18n.tr("main_cli", "prompt_input") << std::flush;
//             std::string command;
//             std::getline(std::cin, command);
//             clearConsole();
//
//
//             // 根据用户输入的命令执行不同的操作
//             if (command == "0")
//             {
//                 showAISuggestions(configUser,configKey,i18n);
//             }
//             else if (command == "1") {
//                 showCurrentDate(configUser, configKey, i18n, true);
//             } else if (command == "show_date --all") {
//                 showCurrentDate(configUser, configKey, i18n, true);
//                 std::cout << "黄历信息：\n" << "今天是农历五月二十九，宜出行、结婚，不宜动土、安床。" << std::endl;
//             } else if (command == "2") {
//                 std::cout << i18n.tr("weather_view", "forecast_title") << "\n";
//                 showWeatherForecast(configUser, configKey, i18n);
//             } else if (command == "3") {
//                 std::cout << i18n.tr("life_index", "title") << "\n";
//                 showLifeIndices(configUser, configKey, i18n);
//             } else if (command == "4") {
//                 std::cout << i18n.tr("city_update", "title") << "\n";
//                 updateCity(configUser, configKey, i18n);
//                 delay_ms(2000);
//             } else if (command == "5") {
//                 std::cout << i18n.tr("settings", "update_title") << "\n";
//                 updateUserSettings(configUser, i18n);
//             } else if (command == "6") {
//                 std::cout << i18n.tr("main_cli", "goodbye") << std::endl;
//                 delay_ms(5000);
//                 break;
//             } else {
//                 std::cout << i18n.tr("main_cli", "invalid_option") << std::endl;
//             }
//             // std::cout << "\n" << i18n.tr("main_cli", "back_to_menu");
//             // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//         }
//     }
//
//     return 0;
// }

int main(int argc, char* argv[]) {
    CliApplication app;
    app.run(argc, argv);
    return 0;
}