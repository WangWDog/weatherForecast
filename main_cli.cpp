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

#include "cli_animation_loader.h"
#include "cli_clear_console.h"
#include "config_key.h"
#include "config_loader.h"
#include "config_user.h"
#include "delay.h"
#include "weather_manager.h"

#ifdef _WIN32
#include <windows.h>
#endif

using json = nlohmann::json;

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

// 显示当前日期
void showCurrentDate()
{
    ConfigUser config;
    config.load();
    std::string format = config.getDateFormateMenu();

    while (!_kbhit()) {  // 如果没有按键按下，就刷新时间
        clearConsole();  // 你已有的函数，用于清屏
        std::time_t now = std::time(nullptr);
        std::cout << "📍 主菜单 > 当前日期时间\n";
        std::cout << "\t 📅:"
                  << std::put_time(std::localtime(&now), format.c_str()) << std::endl;
        std::cout << "按任意键返回主菜单……";
        Sleep(1000);  // 每秒刷新一次（Windows）
    }

    _getch(); // 清除按下的键
}

void showWeatherForecast(ConfigUser& configUser, ConfigKey& configKey)
{
    WeatherManager manager(configKey.getApiKey(), configKey.getHost(), configUser.getLanguage());

    auto result = manager.get7DayForecast(configUser.getCityId(), configUser.getCacheExpiry("daily_forecast"));

    if (result.forecasts.empty()) {
        std::cout << "❌ 未能获取天气数据，请检查网络或 API 设置。" << std::endl;
        return;
    }

    while (true) {
        clearConsole();

        std::cout << (result.fromCache ? "📦 当前数据来自缓存。\n" : "🌐 当前数据来自网络。\n");
        if (result.timestamp > 0) {
            char buf[64];
            std::tm* local = std::localtime(&result.timestamp);
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", local);
            std::cout << "🕒 数据更新时间：" << buf << "\n";
        }

        std::cout << "\n📡 城市：" << configUser.getDefaultCity()
                  << "（ID: " << configUser.getCityId() << "）\n\n";
        std::cout << "📆 未来 7 天预报：\n\n";

        std::cout << "+------------+--------------+--------------+-------------+----------+--------+------------+----------+\n";
        std::cout << "| 📅 日期     | ☀️ 白天天气    | 🌙 夜间天气    | 🌡 温度(℃)     | 🍃 风向    | 💨 风力  | 🌧 降水(mm)   | 💧 湿度(%) |\n";
        std::cout << "+------------+--------------+--------------+-------------+----------+--------+------------+----------+\n";

        for (const auto& f : result.forecasts) {
            std::ostringstream temp;
            temp << f.tempMin << "~" << f.tempMax;

            std::cout << "| " << padRight(f.date, 10) << " "
                      << "| " << padRight(f.textDay, 12) << " "
                      << "| " << padRight(f.textNight, 12) << " "
                      << "| " << padRight(temp.str(), 11) << " "
                      << "| " << padRight(f.windDirDay, 8) << " "
                      << "| " << padRight(f.windScaleDay, 6) << " "
                      << "| " << padRight(f.precip, 10) << " "
                      << "| " << padRight(f.humidity, 8) << "|\n";
        }

        std::cout << "+------------+--------------+--------------+-------------+----------+--------+------------+----------+\n";
        std::cout << "\n🔁 按 R 刷新数据，任意其他键返回主菜单...\n";

        char ch = _getch();
        if (ch == 'R' || ch == 'r')
        {
            result = manager.get7DayForecast(configUser.getCityId(), 0); // 设置过期时间为 0 强制刷新
        }else
        {
            break;
        }
    }
}



void showLifeIndices(ConfigUser& configUser, ConfigKey& configKey)
{
    WeatherManager manager(configKey.getApiKey(), configKey.getHost(), configUser.getLanguage());

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


void updateCity(ConfigUser& configUser, ConfigKey& configKey)
{
    SetConsoleOutputCP(CP_UTF8);

    WeatherManager wm(configKey.getApiKey(), configKey.getHost(), configUser.getLanguage());

    std::string keyword;
    std::vector<CityResult> matches;

    while (true)
    {
        clearConsole();

        std::cout << "📍 主菜单 > 设置城市\n";

        if (!keyword.empty())
        {
            matches = wm.searchCity(keyword);
            std::cout << " 🔎 " << keyword << "搜索结果\n";
            if (matches.empty())
            {
                std::cout << " \t ❌ 未找到匹配城市\n";
            }
            else
            {
                int size = matches.size();
                if (size > 9) size = 9;
                for (size_t i = 0; i < size; ++i)
                {
                    const auto& c = matches[i];
                    std::cout << " \t";
                    std::cout << i + 1 << ". " << c.name
                        << " | " << c.adm1 << " · " << c.adm2 << " · " << c.country
                        << " [ID: " << c.id << "]\n";
                }
                std::cout << "👉 输入编号选择城市，或继续输入关键字...\n";
            }
        }

        std::cout << "🔍 输入城市关键字（输入 : 返回主菜单）" << keyword;

        char ch = _getch();
        if (ch == ':' || keyword == ":")
        {
            std::cout << "\n↩️ 已取消设置，返回主菜单。" << std::endl;
            delay_ms(2000);
            return;
        }
        else if (ch == '\n')
        {
            return;
        }
        else if (ch == '\b' && !keyword.empty())
        {
            keyword.pop_back();
        }
        else if (ch >= '0' && ch <= '9' && !matches.empty())
        {
            int index = ch - '0';
            if (index >= 1 && index <= matches.size())
            {
                const auto& selected = matches[index - 1];
                configUser.setDefaultCity(selected.name);
                configUser.setCityId(selected.id);
                configUser.save();
                std::cout << "\n✅ 城市设置成功：" << selected.name
                    << "（" << selected.adm1 << " · " << selected.country << "）" << std::endl;
                return;
            }
        }
        else
        {
            keyword += ch;
        }
    }
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    showLoadingBar("⚙️加载预设配置", 8, 4, "\033[38;5;117m");

    ConfigUser configUser("configUser.json");
    ConfigKey configKey("configKey.json");

    if (!configUser.load())
    {
        std::cerr << "configUser.json 配置加载失败，程序退出。" << std::endl;
        return 1;
    }
    if (!configKey.load())
    {
        std::cerr << "configKey.json 配置加载失败，程序退出。" << std::endl;
        return 1;
    }

    while (true)
    {
        clearConsole();

        std::cout << "\n🏠 天气 CLI 系统菜单\n";
        std::cout << "--------------------------\n";
        std::cout << "1.🗓️ 显示当前日期\n";
        std::cout << "2.☁️ 显示天气预报\n";
        std::cout << "3.📋 查看生活指数\n";
        std::cout << "4.🚩 设置城市（当前：" << configUser.getDefaultCity() << "）\n";
        std::cout << "5.❌ 退出程序\n";
        std::cout << "--------------------------\n";
        std::cout << "请输入选项（1-5）：";

        std::string choice;
        std::getline(std::cin, choice);
        clearConsole();

        if (choice == "1")
        {
            showCurrentDate();
            continue;
        }
        else if (choice == "2")
        {
            showWeatherForecast(configUser, configKey);
            continue;
        }
        else if (choice == "3")
        {
            showLifeIndices(configUser, configKey);
            continue;
        }
        else if (choice == "4")
        {
            updateCity(configUser, configKey);
            delay_ms(2000);
            continue;
        }
        else if (choice == "5")
        {
            std::cout << "👋 再见！感谢使用天气CLI系统。" << std::endl;
            break;
        }
        else
        {
            std::cout << "❌ 无效选项，请输入 1 - 5。" << std::endl;
        }

        std::cout << "\n按 Enter 返回菜单...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return 0;
}
