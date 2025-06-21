//
// Created by 13033 on 2025/6/16.
//

#include "cli_weather_display.h"

#include "../common/cli_getch.h"
#include <iostream>

#include "config_context.h"
#include "date_utils.h"
#include "i18n_loader.h"
#include "weather_manager.h"
#include "../common/cli_clear_console.h"
#include "common/cli_context.h"

void displayWeather(ForecastResult &result, I18n &i18n, ConfigUser &configUser, const std::string& unit);

void printLine() {
    std::cout << "+--------------+--------------+--------------+--------------+"
                 "--------------+--------------+--------------+--------------+\n";
}
void showWeatherForecast(CliContext& cli, const std::string& unit) {
    auto& i18n = cli.i18n;
    auto& configKey = cli.config.key();
    auto& configUser = cli.config.user();
    WeatherManager manager(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());

    auto result = manager.get7DayForecast(
        configUser.getCityId(),
        configUser.getLanguage(),
        unit,
        configUser.getCacheExpiry("daily_forecast"),
        cli.cache);

    if (result.forecasts.empty()) {
        std::cout << i18n.tr("forecast", "fetch_failed") << std::endl;  // 翻译 "fetch_failed"
        return;
    }

    while (true) {
        displayWeather(result, i18n, configUser, unit);
        std::cout << "\n" << i18n.tr("forecast", "prompt_refresh") << "\n";  // 翻译 "prompt_refresh"
        char ch = cli_getch();
        if (ch == 'R' || ch == 'r') {
            result = manager.get7DayForecast(
                configUser.getCityId(),
                configUser.getLanguage(),
                unit,
                0,
                cli.cache); // 强制刷新
        } else {
            break;
        }
    }
}

void displayWeather(ForecastResult &result, I18n &i18n, ConfigUser &configUser, const std::string& unit) {
    clearConsole();

    std::cout << (result.fromCache
                      ? i18n.tr("forecast", "from_cache")
                      : i18n.tr("forecast", "from_network"))
              << "\n";

    if (result.timestamp > 0) {
        char buf[64];
        std::tm *local = std::localtime(&result.timestamp);
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", local);
        std::cout << i18n.tr("forecast", "updated_time") << buf << "\n";
    }

    std::cout << "\n" << i18n.tr("forecast", "city") << configUser.getCityName()
              << "（ID: " << configUser.getCityId() << "）\n\n";
    std::cout << i18n.tr("forecast", "forecast_title") << "\n\n";

    printLine();

    bool imperial = (unit == "i" || unit == "imperial");
    std::string tempUnit = imperial ? i18n.tr("forecast", "temp_unit_f")
                                    : i18n.tr("forecast", "temp_unit_c");
    std::string precipUnit = imperial ? i18n.tr("forecast", "precip_unit_in")
                                      : i18n.tr("forecast", "precip_unit_mm");

    std::string tempHeader = i18n.tr("forecast", "temperature") + "(" + tempUnit + ")";
    std::string precipHeader = i18n.tr("forecast", "precip") + "(" + precipUnit + ")";

    std::cout << "| " << centerText(i18n.tr("forecast", "date"), 12)
              << " | " << centerText(i18n.tr("forecast", "text_day"), 12)
              << " | " << centerText(i18n.tr("forecast", "text_night"), 12)
              << " | " << centerText(tempHeader, 12)
              << " | " << centerText(i18n.tr("forecast", "wind_dir"), 12)
              << " | " << centerText(i18n.tr("forecast", "wind_scale"), 12)
              << " | " << centerText(precipHeader, 12)
              << " | " << centerText(i18n.tr("forecast", "humidity"), 12) << " |\n";
    printLine();

    for (const auto &f : result.forecasts) {

        // 拼接温度范围
        std::ostringstream temp;
        temp << f.tempMin << "~" << f.tempMax;

        std::ostringstream precipStr;
        precipStr << std::fixed << std::setprecision(1) << f.precip;

        // 安全转换湿度为整数字符串
        std::string humidityStr;
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
        row << "| " << centerText(f.date, 12)
            << " | " << centerText(f.textDay, 12)
            << " | " << centerText(f.textNight, 12)
            << " | " << centerText(temp.str(), 12)
            << " | " << centerText(f.windDirDay, 12)
            << " | " << centerText(f.windScaleDay, 12)
            << " | " << centerText(precipStr.str(), 12)
            << " | " << centerText(humidityStr, 12) << " |\n";

        std::cout << row.str();
    }
    printLine();
}


