#include "cli_menu_dispatch_controller.h"
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include "common/delay.h"
#include "menus_model/weather_display/cli_weather_display.h"
#include "menus_model/life_index/cli_life_index.h"
#include "menus_model/date_display/cli_date_display.h"
#include "menus_model/update_city/cli_update_city.h"
#include "menus_model/user_setting/cli_user_settings.h"

// 构造函数：初始化配置上下文和多语言支持对象
CliCommandDispatcher::CliCommandDispatcher(ConfigContext& ctx, I18n& i18n)
    : configContext(ctx), i18n(i18n) {}

// 显示命令帮助信息
void CliCommandDispatcher::showCommandHelp() {
    std::cout << "Available commands:\n";
    std::cout << "  show_date        - Show current date and time\n";
    std::cout << "  show_date --all  - Show current time, date, and the Chinese Almanac\n";
    std::cout << "  show_forecast    - Show 7-day weather forecast\n";
    std::cout << "  show_life        - Show life indices for the city\n";
    std::cout << "  update_city      - Update the city setting\n";
    std::cout << "  update_settings  - Update user settings\n";
    std::cout << "  exit             - Exit the application\n";
}

// 命令处理函数：根据命令行参数执行对应操作
void CliCommandDispatcher::handle(int argc, char* argv[]) {
    // 如果没有提供命令（只有程序名），则显示帮助
    if (argc < 2) {
        showCommandHelp();
        return;
    }

    // 获取第一个参数（命令）
    std::string command = argv[1];

    if (command == "show_date") {
        bool showAll = false;
        // 检查是否有额外参数
        if (argc > 2) {
            std::string type = argv[2];
            // 如果第二个参数包含"--all"，则设置显示完整信息
            if (type.find("--all") != std::string::npos) {
                showAll = true;
            }
        }
        // 调用显示当前日期的函数
        showCurrentDate(configContext, i18n, showAll);

    // 处理"show_forecast"命令：显示天气预报
    } else if (command == "show_forecast") {
        showWeatherForecast(configContext, i18n);

    // 处理"show_life"命令：显示生活指数
    } else if (command == "show_life") {
        showLifeIndices(configContext, i18n);

    // 处理"update_city"命令：更新城市
    } else if (command == "update_city") {
        showCityChoose(configContext, i18n);

    // 处理"update_settings"命令：更新用户设置
    } else if (command == "update_settings") {
        updateUserSettings(configContext.user(), i18n);

    // 处理"exit"命令：退出程序
    } else if (command == "exit") {
        // 再见~（皮一下看会不会被发现QAQ）
        std::cout << i18n.tr("main_cli", "goodbye~") << std::endl;
        delay_ms(2000);
        exit(0);
        // 退出程序

    // 无效命令处理
    } else {
        std::cout << "❌ Invalid command! Type 'help' for a list of commands." << std::endl;
    }
}
