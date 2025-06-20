#include "cli_menu_controller.h"

#include <iostream>

#include "../common/cli_animation_loader.h"
#include "../common/cli_clear_console.h"
#include "ai_advisor/cli_ai_advisor.h"
#include "../common/delay.h"
#include "weather_display/cli_weather_display.h"
#include "CacheManager.h"
#include "date_display/cli_date_display.h"
#include "life_index/cli_life_index.h"
#include "update_city/cli_update_city.h"
#include "user_setting/cli_user_settings.h"

// 初始化配置上下文和国际化翻译器
CliMenuController::CliMenuController(ConfigContext& configContext, I18n& translator)
    : configContext(configContext), i18n(translator) // 初始化成员变量
{
}

void CliMenuController::run()
{
    // 显示加载动画
    showLoadingBar(
        i18n.tr("main_cli", "menu_loading"),
        8,   // 动画帧数
        40,  // 进度条长度
        "\033[38;5;117m" // 浅蓝色
    );

    while (true)
    {
        clearConsole();
        // 显示菜单标题
        std::cout << "\n" << i18n.tr("main_cli", "menu_title") << "\n"; // "主菜单"
        std::cout << "------------------------\n";

        // 获取菜单选项的多语言列表
        auto options = i18n.trList("main_cli", "menu_options");

        // 遍历并显示所有菜单选项
        for (size_t i = 0; i < options.size(); ++i)
        {
            std::cout << i << ". " << options[i] << "\n";
        }
        std::cout << "------------------------\n";

        // 显示输入提示
        std::cout << i18n.tr("main_cli", "prompt_input") << std::flush; // "请选择操作："

        // 获取用户输入
        std::string command;
        std::getline(std::cin, command);
        clearConsole(); // 清屏准备显示功能界面

        // 处理用户选择
        if (command == "0") // AI 天气建议
        {
            showAISuggestions(configContext, i18n);
        }
        else if (command == "1") // 显示当前日期
        {
            showCurrentDate(configContext, i18n, true);
        }
        else if (command == "2") // 天气预报
        {
            // 显示子菜单标题
            std::cout << i18n.tr("weather_view", "forecast_title") << "\n"; // "天气预报"
            showWeatherForecast(configContext, i18n);
        }
        else if (command == "3") // 生活指数
        {
            std::cout << i18n.tr("life_index", "title") << "\n"; // "生活指数"
            showLifeIndices(configContext, i18n);
        }
        else if (command == "4") // 城市设置
        {
            std::cout << i18n.tr("city_update", "title") << "\n"; // "设置城市"
            showCityChoose(configContext, i18n);
            delay_ms(2000); // 操作完成后暂停2秒
        }
        else if (command == "5") // 用户设置
        {
            std::cout << i18n.tr("settings", "update_title") << "\n"; // "用户设置"
            updateUserSettings(configContext.user(), i18n);
        }
        else if (command == "6")
        {
            std::cout << i18n.tr("main_cli", "goodbye") << std::endl; // "感谢使用，再见！"
            delay_ms(5000); // 显示5秒退出消息
            break;
        }
        else // 无效输入处理
        {
            std::cout << i18n.tr("main_cli", "invalid_option") << std::endl; // "无效选项，请重新输入"
        }
    }
}