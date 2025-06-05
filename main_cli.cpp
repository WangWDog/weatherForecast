#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <json.hpp>
#include <fcntl.h>
#include <io.h>
#include <conio.h>
#include <windows.h>
#include <curl/curl.h>

#include "cli_animation_loader.h"
#include "cli_clear_console.h"
#include "config_key.h"
#include "config_loader.h"
#include "config_user.h"
#include "delay.h"
#include "updateCity.h"
#include "weather_manager.h"
#include "i18n/i18n_loader.h"
#include "lunar_api.h"
#include "config_key.h"

#ifdef _WIN32
#include <windows.h>
#endif

using json = nlohmann::json;

// 回调函数：将 HTTP 响应的内容写入到字符串中
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// 用于调用 API 获取农历、节气和黄历等信息
std::string getLunarInfo(ConfigKey& config_key, const std::string& lang, I18n& i18n) {
     std::string response = callLunarApi(config_key, lang);  // 请求 API

    if (response.empty()) {
        return "❌ 未获取到农历信息";
    }

    try {
        auto j = nlohmann::json::parse(response);
        if (!j.contains("data")) return "❌ 响应数据无效";

        const auto& d = j["data"];
        std::ostringstream oss;

        auto printIfNotEmpty = [&](const std::string& emoji, const std::string& label, const std::string& key) {
            if (d.contains(key) && !d[key].get<std::string>().empty()) {
                oss << emoji << " " << label << "：" << d[key].get<std::string>() << "\n";
            }
        };

        // 信息输出
        printIfNotEmpty("\t📅", "公历", "Solar");
        printIfNotEmpty("\t📆", "星期", "Week");
        printIfNotEmpty("\t🌙", "农历", "Lunar");
        printIfNotEmpty("\t🧧", "农历年份", "LunarYear");
        printIfNotEmpty("\t🐉", "属相", "ThisYear");
        printIfNotEmpty("\t📜", "干支年", "GanZhiYear");
        printIfNotEmpty("\t🎈", "节日", "Festivals");
        printIfNotEmpty("\t🌾", "节气", "JieQi1");
        printIfNotEmpty("\t✅", "宜", "YiDay");
        printIfNotEmpty("\t⚠️", "忌", "JiDay");
        printIfNotEmpty("\t💬", "微语·短", "WeiYu_s");
        printIfNotEmpty("\t📖", "微语·长", "WeiYu_l");


        return oss.str();

    } catch (const std::exception& e) {
        return std::string("❌ JSON 解析失败：") + e.what();
    }
}




// 宽字符对齐工具函数（仅估算宽度）
size_t visualLength(const std::string& str)
{
    size_t len = 0;
    for (unsigned char ch : str)
    {
        len += (ch >= 0x80) ? 2 : 1;
    }
    return len;
}

std::string padRight(const std::string& str, size_t targetLen)
{
    size_t visualLen = visualLength(str);
    if (visualLen >= targetLen) return str;
    return str + std::string(targetLen - visualLen, ' ');
}


void updateUserSettings(ConfigUser& configUser, I18n& i18n)
{
    while (true) {
        clearConsole();
        std::cout << "🔧 " << i18n.tr("settings", "menu_title") << "\n";
        std::cout << "--------------------------\n";
        std::cout << "1. 📅 " << i18n.tr("settings", "date_format") << "（" << configUser.getDateFormateMenu() << "）\n";
        std::cout << "2. 🧭 " << i18n.tr("settings", "cache_life_index") << "（" << configUser.getCacheExpiry("life_index") << " 分钟）\n";
        std::cout << "3. 🌦 " << i18n.tr("settings", "cache_forecast") << "（" << configUser.getCacheExpiry("daily_forecast") << " 分钟）\n";
        std::cout << "4. 🈯 " << i18n.tr("settings", "language") << "（" << configUser.getLanguage() << "） \n";
        std::cout << "5. 🔙 " << i18n.tr("settings", "back") << "\n";
        std::cout << i18n.tr("settings", "prompt_input");

        std::string choice;
        std::getline(std::cin, choice);

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

        } else if (choice == "2") {
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

        } else if (choice == "3") {
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

        } else if (choice == "4") {
            std::string lang;
            std::cout << i18n.tr("settings", "input_language");
            std::getline(std::cin, lang);
            if (lang != ":q") {
                configUser.setLanguage(lang);
                std::cout << i18n.tr("settings", "updated") << "\n";
                if (!i18n.load(lang)) {
                    std::cout << i18n.tr("settings", "language_load_fail") << "\n";
                } else {
                    std::cout << i18n.tr("settings", "language_load_success") << " " << lang << "\n";
                }
            } else {
                std::cout << i18n.tr("settings", "cancelled") << "\n";
            }
            continue;
        } else if (choice == "5") {
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
void showCurrentDate(ConfigUser& configUser, ConfigKey& configKey, I18n& i18n)
{
    clearConsole();

    std::time_t now = std::time(nullptr);

    std::cout << i18n.tr("date_view", "title") << "\n";  // 📍 主菜单 > 当前日期时间
    std::cout << "\t" << i18n.tr("date_view", "solar") << ": "
              << std::put_time(std::localtime(&now), configUser.getDateFormateMenu().c_str()) << std::endl;

    // ✅ 加入农历黄历显示（已国际化）
    std::string lunarInfo = getLunarInfo(configKey, configUser.getLanguage(), i18n);
    std::cout << lunarInfo << std::endl;

    std::cout << "\n" << i18n.tr("date_view", "return_hint");  // 按任意键返回主菜单……
    _getch();  // 等待用户按键
}




void showWeatherForecast(ConfigUser& configUser, ConfigKey& configKey, I18n& i18n)
{
    WeatherManager manager(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());

    auto result = manager.get7DayForecast(configUser.getCityId(), configUser.getLanguage(),configUser.getCacheExpiry("daily_forecast"));

    if (result.forecasts.empty()) {
        std::cout << i18n.tr("forecast", "fetch_failed") << std::endl;
        return;
    }

    while (true) {
        clearConsole();

        std::cout << (result.fromCache
                      ? i18n.tr("forecast", "from_cache")
                      : i18n.tr("forecast", "from_network"))
                  << "\n";

        if (result.timestamp > 0) {
            char buf[64];
            std::tm* local = std::localtime(&result.timestamp);
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", local);
            std::cout << i18n.tr("forecast", "updated_time") << buf << "\n";
        }

        std::cout << "\n" << i18n.tr("forecast", "city") << configUser.getDefaultCity()
                  << "（ID: " << configUser.getCityId() << "）\n\n";
        std::cout << i18n.tr("forecast", "forecast_title") << "\n\n";

        std::cout << "+------------+--------------+--------------+-------------+----------+--------+------------+----------+\n";
        std::cout << "| " << padRight(i18n.tr("forecast", "date"), 10)
                  << " | " << padRight(i18n.tr("forecast", "text_day"), 12)
                  << " | " << padRight(i18n.tr("forecast", "text_night"), 12)
                  << " | " << padRight(i18n.tr("forecast", "temperature"), 11)
                  << " | " << padRight(i18n.tr("forecast", "wind_dir"), 8)
                  << " | " << padRight(i18n.tr("forecast", "wind_scale"), 6)
                  << " | " << padRight(i18n.tr("forecast", "precip"), 10)
                  << " | " << padRight(i18n.tr("forecast", "humidity"), 8) << " |\n";
        std::cout << "+------------+--------------+--------------+-------------+----------+--------+------------+----------+\n";

        for (const auto& f : result.forecasts) {
            std::ostringstream temp;
            temp << f.tempMin << "~" << f.tempMax;

            std::cout << "| " << padRight(f.date, 10)
                      << " | " << padRight(f.textDay, 12)
                      << " | " << padRight(f.textNight, 12)
                      << " | " << padRight(temp.str(), 11)
                      << " | " << padRight(f.windDirDay, 8)
                      << " | " << padRight(f.windScaleDay, 6)
                      << " | " << padRight(f.precip, 10)
                      << " | " << padRight(f.humidity, 8) << " |\n";
        }

        std::cout << "+------------+--------------+--------------+-------------+----------+--------+------------+----------+\n";
        std::cout << "\n" << i18n.tr("forecast", "prompt_refresh") << "\n";

        char ch = _getch();
        if (ch == 'R' || ch == 'r') {
            result = manager.get7DayForecast(configUser.getCityId(),configUser.getLanguage(), 0);  // 强制刷新
        } else {
            break;
        }
    }
}




void showLifeIndices(ConfigUser& configUser, ConfigKey& configKey)
{
    WeatherManager manager(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());

    // 初次加载（尝试用缓存）
    auto result = manager.getLifeIndices(configUser.getCityId(), configUser.getCacheExpiry("weather_index"));

    if (result.indices.empty())
    {
        std::cout << "❌ 无法获取生活指数数据。\n";
        return;
    }

    while (true)
    {
        clearConsole();

        // 缓存信息展示
        if (result.fromCache)
        {
            std::cout << "📦 当前数据来自缓存。\n";
        }
        else
        {
            std::cout << "🌐 当前数据来自网络。\n";
        }

        if (result.timestamp > 0)
        {
            char buf[64];
            std::tm* local = std::localtime(&result.timestamp);
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", local);
            std::cout << "🕒 数据更新时间：" << buf << "\n";
        }

        // 生活指数展示
        std::cout << "📌 城市：" << configUser.getDefaultCity() << " 的生活指数如下：\n\n";
        for (const auto& idx : result.indices)
        {
            std::cout << "📅 日期：" << idx.date << "\n"
                << "📌 类型：" << idx.name << "\n"
                << "📈 等级：" << idx.level << "（" << idx.category << "）\n"
                << "📖 建议：" << idx.text << "\n"
                << "--------------------------\n";
        }

        std::cout << "\n🔁 按 R 刷新数据，任意其他键返回主菜单...\n";
        char ch = _getch();
        if (ch == 'R' || ch == 'r')
        {
            result = manager.getLifeIndices(configUser.getCityId(), 0); // 设置过期时间为 0 强制刷新
        }
        else
        {
            break;
        }
    }
}
int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    ConfigUser configUser("configUser.json");
    ConfigKey configKey("configKey.json");
    I18n i18n;

    if (!configUser.load() || !configKey.load() || !i18n.load(configUser.getLanguage())) {
        std::cerr << "❌ 初始化失败，程序退出。\n";
        return 1;
    }
    showLoadingBar("⚙️加载预设配置", 8, 40, "\033[38;5;117m");
    while (true)
    {
        clearConsole();

        std::cout << "\n" << i18n.tr("main_cli","menu_title") << "\n";
        std::cout << "--------------------------\n";
        auto options = i18n.trList("main_cli","menu_options");
        for (size_t i = 0; i < options.size(); ++i) {
            std::cout << i + 1 << ". " << options[i] << "\n";
        }
        std::cout << "--------------------------\n";
        std::cout << i18n.tr("main_cli","prompt_input") << std::flush;

        std::string choice;
        std::getline(std::cin, choice);
        clearConsole();

        if (choice == "1") {
            showCurrentDate(configUser,configKey,i18n);
        } else if (choice == "2") {
            showWeatherForecast(configUser, configKey, i18n);
        } else if (choice == "3") {
            showLifeIndices(configUser, configKey);
        } else if (choice == "4") {
            updateCity(configUser, configKey);
            delay_ms(2000);
        } else if (choice == "5") {
            updateUserSettings(configUser,i18n);
        } else if (choice == "6") {
            std::cout << i18n.tr("main_cli","goodbye") << std::endl;
            delay_ms(2000);
            break;
        } else {
            std::cout << i18n.tr("main_cli","invalid_option") << std::endl;
        }
        std::cout << "\n" << i18n.tr("main_cli","back_to_menu");
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return 0;
}