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
#include "config/config_user.h"
#include "cli/common/delay.h"
#include "core/weather_manager.h"
#include "i18n/i18n_loader.h"
#include "lunar_api.h"
#include "config/config_key.h"
#include "date_utils.h"  // 包含辅助函数头文件
#include "core/doubao_manager.h"
#include "core/CacheManager.h"





//#ifdef _WIN32
#include <windows.h>

#include "cli_application.h"
// //#endif
//
// using json = nlohmann::json;
//
//
//
//
// // 显示当前日期
//
//
//
//
//
// // void displayWeatherCommander(ConfigUser &configUser, ConfigKey &configKey, I18n &i18n, bool forceRefresh) {
// //     WeatherManager manager(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());
// //     ForecastResult result;
// //     if (forceRefresh) {
// //         result = manager.get7DayForecast(configUser.getCityId(), configUser.getLanguage(), 0);
// //     } else {
// //         result = manager.get7DayForecast(configUser.getCityId(), configUser.getLanguage(),
// //                                          configUser.getCacheExpiry("daily_forecast"));
// //     }
// //
// //
// //     if (result.forecasts.empty()) {
// //         std::cout << i18n.tr("forecast", "fetch_failed") << std::endl;
// //         return;
// //     }
// //
// //     displayWeather(result, i18n, configUser);
// //     std::cout << "\n" << i18n.tr("forecast", "commander_prompt_refresh") << "\n";
// // }
//
//
//
//
// void showCommandHelp() {
//     std::cout << "Available commands:\n";
//     std::cout << "  show_date        - Show current date and time\n";
//     std::cout << "  show_date --all  - Show current time, date, and the Chinese Almanac\n";
//     std::cout << "  show_forecast    - Show 7-day weather forecast\n";
//     std::cout << "  show_life        - Show life indices for the city\n";
//     std::cout << "  update_city      - Update the city setting\n";
//     std::cout << "  update_settings  - Update user settings\n";
//     std::cout << "  exit             - Exit the application\n";
// }
//
//
// void handleCommand(int argc, char *argv[], ConfigUser &configUser, ConfigKey &configKey, I18n &i18n) {
//     std::string command = argv[1];
//
//     if (command == "show_date") {
//         bool showAll = false;
//         if (argc > 2) {
//             std::string type = argv[2];
//             if (type.find("--all") != std::string::npos) {
//                 showAll = true; // 如果包含 --all，设置为 true
//             }
//         }
//         // 判断是否传入 --all 参数
//         std::string prompt = "当前日期和时间：";
//         std::time_t t = std::time(nullptr);
//         std::tm* timeInfo = std::localtime(&t);
//         std::cout << prompt << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S") << std::endl;
//
//         if (showAll) {
//             // 如果 showAll 为 true，则还需展示黄历信息
//             std::cout << "黄历信息：\n" << "今天是农历五月二十九，宜出行、结婚，不宜动土、安床。" << std::endl;
//         }
//
//     } else if (command == "show_forecast") {
//         bool forceRefresh = false;
//         if (argc > 2) {
//             std::string type = argv[2];
//             if (type.find("-R") != std::string::npos) {
//                 forceRefresh = true; // 如果包含 -R，设置为 true
//             }
//         }
//         // 显示天气预报的代码
//         std::cout << "显示7天天气预报" << std::endl;
//
//     } else if (command == "show_life") {
//         // 显示生活指数的代码
//         std::cout << "显示生活指数" << std::endl;
//
//     } else if (command == "update_city") {
//         // 更新城市设置的代码
//         std::cout << "更新城市设置" << std::endl;
//
//     } else if (command == "update_settings") {
//         // 更新用户设置的代码
//         std::cout << "更新用户设置" << std::endl;
//
//     } else if (command == "exit") {
//         std::cout << i18n.tr("main_cli", "goodbye") << std::endl;
//         delay_ms(2000);
//         exit(0); // Exits the program
//
//     } else {
//         std::cout << "❌ Invalid command! Type 'help' for a list of commands." << std::endl;
//     }
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