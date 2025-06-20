//
// Created by 13033 on 2025/6/16.
//

#include "cli_weather_display.h"

#include <conio.h>
#include <iostream>

#include "config_context.h"
#include "date_utils.h"
#include "i18n_loader.h"
#include "weather_manager.h"
#include "../common/cli_clear_console.h"

// 显示天气
void displayWeather(ForecastResult &result, I18n &i18n, ConfigUser &configUser);

void printLine() {
    std::cout << "+--------------+--------------+--------------+--------------+"
                 "--------------+--------------+--------------+--------------+\n";
}
void showWeatherForecast(ConfigContext& config_context, I18n &i18n) {
    // 获取配置信息
    auto configKey = config_context.key();
    auto configUser = config_context.user();

    // 创建天气管理器实例
    WeatherManager manager(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());

    auto result = manager.get7DayForecast(configUser.getCityId(), configUser.getLanguage(),
                                          configUser.getCacheExpiry("daily_forecast")  // 获取缓存过期时间
                                         );

    // 检查是否获取到数据
    if (result.forecasts.empty()) {
        std::cout << i18n.tr("forecast", "fetch_failed") << std::endl;  // 翻译 "fetch_failed"
        return;
    }

    while (true) {
        displayWeather(result, i18n, configUser);  // 显示天气数据

        // 提示用户刷新或退出
        std::cout << "\n" << i18n.tr("forecast", "prompt_refresh") << "\n";  // 翻译 "prompt_refresh"
        char ch = _getch();
        if (ch == 'R' || ch == 'r') {
            // 强制刷新（设置过期时间为0）
            result = manager.get7DayForecast(configUser.getCityId(), configUser.getLanguage(), 0); // 强制刷新
        } else {
            break;
        }
    }
}

void displayWeather(ForecastResult &result, I18n &i18n, ConfigUser &configUser) {
    clearConsole();

    // 显示数据来源（缓存/网络）
    std::cout << (result.fromCache
                      ? i18n.tr("forecast", "from_cache")  // "来自缓存"
                      : i18n.tr("forecast", "from_network")) // "来自网络"
              << "\n";

    // 显示数据更新时间
    if (result.timestamp > 0) {
        char buf[64];
        std::tm *local = std::localtime(&result.timestamp);  // 转换为本地时间
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", local);  // 格式化为字符串
        std::cout << i18n.tr("forecast", "updated_time") << buf << "\n";  // "数据更新时间"
    }

    // 显示城市信息
    std::cout << "\n" << i18n.tr("forecast", "city") << configUser.getDefaultCity()
              << "（ID: " << configUser.getCityId() << "）\n\n";

    // 显示标题
    std::cout << i18n.tr("forecast", "forecast_title") << "\n\n";  // "7天天气预报"

    printLine();
    std::cout << "| " << centerText(i18n.tr("forecast", "date"), 12)         // "日期"
              << " | " << centerText(i18n.tr("forecast", "text_day"), 12)    // "白天天气"
              << " | " << centerText(i18n.tr("forecast", "text_night"), 12)  // "夜间天气"
              << " | " << centerText(i18n.tr("forecast", "temperature"), 12) // "温度(℃)"
              << " | " << centerText(i18n.tr("forecast", "wind_dir"), 12)    // "风向"
              << " | " << centerText(i18n.tr("forecast", "wind_scale"), 12)  // "风力等级"
              << " | " << centerText(i18n.tr("forecast", "precip"), 12)      // "降水量(mm)"
              << " | " << centerText(i18n.tr("forecast", "humidity"), 12)    // "湿度(%)"
              << " |\n";
    printLine();

    // 遍历每天的天气预报数据
    for (const auto &f : result.forecasts) {

        // 拼接温度范围
        std::ostringstream temp;
        temp << f.tempMin << "~" << f.tempMax;  // 温度范围格式化为"最低~最高"

        // 降水量格式化（保留1位小数）
        std::ostringstream precipStr;
        precipStr << std::fixed << std::setprecision(1) << f.precip;

        // 处理湿度值，安全转换湿度为整数字符串
        std::string humidityStr;
        // 重新初始化字符串流（确保状态正确）
        temp.str("");         // 清空内容
        temp.clear();         // 重置状态标志
        precipStr.str("");
        precipStr.clear();
        humidityStr = "--";
        try {
            humidityStr = std::to_string(std::stoi(f.humidity));  // "86.0" -> 86 -> "86"
        } catch (...) {
            humidityStr = "--";  // 若转换失败，例如内容不是数字，则显示为 "--"
        }

        // 重新格式化温度和降水量
        // 数据行（循环中使用）
        // 温度范围拼接
        temp << f.tempMin << "~" << f.tempMax;

        // 降水量格式化
        precipStr << std::fixed << std::setprecision(1) << f.precip;

        // 湿度处理（避免小数）
        try {
            humidityStr = std::to_string(std::stoi(f.humidity));
        } catch (...) {}

        std::ostringstream row;
        row << "| " << centerText(f.date, 12)          // 日期（居中）
            << " | " << centerText(f.textDay, 12)      // 白天天气
            << " | " << centerText(f.textNight, 12)    // 夜间天气
            << " | " << centerText(temp.str(), 12)     // 温度范围
            << " | " << centerText(f.windDirDay, 12)   // 风向
            << " | " << centerText(f.windScaleDay, 12) // 风力等级
            << " | " << centerText(precipStr.str(), 12) // 降水量
            << " | " << centerText(humidityStr, 12)    // 湿度
            << " |\n";

        // 输出该行数据
        std::cout << row.str();
    }
    printLine();
}
