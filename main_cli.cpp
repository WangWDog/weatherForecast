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

#include "cli/common/cli_animation_loader.h"
#include "cli/common/cli_clear_console.h"
#include "config/config_key.h"
#include "config/config_loader.h"
#include "config/config_user.h"
#include "cli/common/delay.h"
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




// 显示当前日期





// void displayWeatherCommander(ConfigUser &configUser, ConfigKey &configKey, I18n &i18n, bool forceRefresh) {
//     WeatherManager manager(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());
//     ForecastResult result;
//     if (forceRefresh) {
//         result = manager.get7DayForecast(configUser.getCityId(), configUser.getLanguage(), 0);
//     } else {
//         result = manager.get7DayForecast(configUser.getCityId(), configUser.getLanguage(),
//                                          configUser.getCacheExpiry("daily_forecast"));
//     }
//
//
//     if (result.forecasts.empty()) {
//         std::cout << i18n.tr("forecast", "fetch_failed") << std::endl;
//         return;
//     }
//
//     displayWeather(result, i18n, configUser);
//     std::cout << "\n" << i18n.tr("forecast", "commander_prompt_refresh") << "\n";
// }




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
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
    CliApplication app;
    app.run(argc, argv);
    return 0;
}