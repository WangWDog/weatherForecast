#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <fcntl.h>
#include <io.h>

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

// 显示当前日期
void showCurrentDate()
{
    std::time_t now = std::time(nullptr);
    std::cout << "📅 当前日期与时间："
        << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << std::endl;
}
void showWeatherForecast(ConfigUser& configUser, ConfigKey& configKey)
{
    WeatherManager manager(configKey.getApiKey(), configKey.getHost(), configUser.getLanguage());
    auto forecasts = manager.get7DayForecast(configUser.getCityId());

    if (forecasts.empty()) {
        std::cout << "❌ 未能获取天气数据，请检查网络或 API 设置。" << std::endl;
        return;
    }

    std::cout << "📡 城市：" << configUser.getDefaultCity()
              << "（ID: " << configUser.getCityId() << "）\n\n";
    std::cout << "🗓️ 未来 7 天预报：\n\n";

    // 表头
    std::cout << "+------------+---------------+---------------+-------------+-----------+-----------+--------------+--------------+\n";
    std::cout << "|  📅 日期   |  ☀️ 白天天气     |  🌙 夜间天气  |  🌡 温度(℃)   |  🍃 风向  |  💨 风力  |  🌧 降水(mm)  |  💧 湿度(%)  |\n";
    std::cout << "+------------+---------------+---------------+-------------+-----------+-----------+--------------+--------------+\n";

    for (const auto& f : forecasts) {
        std::ostringstream temp;
        temp << f.tempMin << "~" << f.tempMax;

        std::cout << "| " << std::setw(10) << std::left << f.date << " "
                  << "| " << std::setw(16) << std::left << f.textDay << " "
                  << "| " << std::setw(13) << std::left << f.textNight << " "
                  << "| " << std::setw(12) << std::left << temp.str() << " "
                  << "| " << std::setw(9)  << std::left << f.windDirDay << " "
                  << "| " << std::setw(9)  << std::left << f.windScaleDay << " "
                  << "| " << std::setw(12) << std::left << f.precip << " "
                  << "| " << std::setw(12) << std::left << f.humidity << "|\n";
    }

    std::cout << "+------------+---------------+---------------+-------------+-----------+-----------+--------------+--------------+\n";
}
// 设置城市
// void updateCity(ConfigUser& configUser, ConfigKey& configKey)
// {
//     clearConsole();
//     std::cout << "📍 主菜单 > 设置城市\n";
//     std::cout << "🔍 请输入城市关键字（按 Enter 搜索，空行或 ESC 返回）：";
//
//     std::string newCity;
//     std::getline(std::cin, newCity);
//     if (newCity.empty())
//     {
//         std::cout << "↩️ 已取消设置，返回主菜单。" << std::endl;
//         return;
//     }
//
//     // 查询城市
//     WeatherManager wm(configKey.getApiKey(), configKey.getHost());
//     auto matches = wm.searchCity(newCity);
//
//     if (matches.empty())
//     {
//         std::cout << "❌ 未找到匹配的城市。" << std::endl;
//         return;
//     }
//
//     std::cout << "\n📍 主菜单 > 设置城市 > 选择城市\n";
//     std::cout << "🔎 匹配结果（共 " << matches.size() << " 个）：" << std::endl;
//
//     for (size_t i = 0; i < matches.size(); ++i)
//     {
//         const auto& c = matches[i];
//         std::cout << std::setw(2) << (i + 1) << ". " << c.name
//             << " | " << c.adm1 << " · " << c.adm2 << " · " << c.country
//             << " [ID: " << c.id << "]" << std::endl;
//     }
//
//     std::cout << "\n请输入城市编号以选择（空行返回）：";
//     std::string choiceStr;
//     std::getline(std::cin, choiceStr);
//
//     if (choiceStr.empty())
//     {
//         std::cout << "↩️ 未选择，返回主菜单。" << std::endl;
//         return;
//     }
//
//     int choice;
//     try
//     {
//         choice = std::stoi(choiceStr);
//     }
//     catch (...)
//     {
//         std::cout << "❌ 输入无效，返回主菜单。" << std::endl;
//         return;
//     }
//
//     if (choice <= 0 || choice > matches.size())
//     {
//         std::cout << "❌ 编号无效，返回主菜单。" << std::endl;
//         return;
//     }
//
//     const auto& selected = matches[choice - 1];
//     configUser.setDefaultCity(selected.name);
//     configUser.setCityId(selected.id);
//     configUser.save();
//     std::cout << "\n✅ 城市设置成功："
//         << selected.name << "（" << selected.adm1 << " · " << selected.country << "）" << std::endl;
// }
#ifdef _WIN32
#include <conio.h>
#endif

// void updateCity(ConfigUser& configUser, ConfigKey& configKey)


#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

#include <conio.h>
#include <windows.h>

#include <conio.h>
#include <windows.h>

#include <conio.h>
#include <windows.h>

#include <conio.h>
#include <windows.h>

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
                if (size > 9)
                {
                    size = 9;
                }
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
    SetConsoleOutputCP(CP_UTF8); // Windows 控制台显示 UTF-8
    SetConsoleCP(CP_UTF8);
#endif

    showLoadingBar("⚙️加载预设配置", 8, 4, "\033[38;5;117m");

    // BaseConfig config = new ("configUser.json");
    // if (!config.load()) {
    //     std::cerr << "配置加载失败，程序退出。" << std::endl;
    //     return 1;
    // }

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
        std::cout << "3.🚩 设置城市（当前：" << configUser.getDefaultCity() << "）\n";
        std::cout << "4.❌ 退出程序\n";
        std::cout << "--------------------------\n";
        std::cout << "请输入选项（1-4）：";

        std::string choice;
        std::getline(std::cin, choice);
        clearConsole();

        if (choice == "1")
        {
            showCurrentDate();
        }
        else if (choice == "2")
        {
            // showSpinner("正在获取天气信息", 2000, 100, "\033[36m");
            showWeatherForecast(configUser, configKey);
        }
        else if (choice == "3")
        {
            updateCity(configUser, configKey);
            delay_ms(2000);
            continue;
        }
        else if (choice == "4")
        {
            std::cout << "👋 再见！感谢使用天气CLI系统。" << std::endl;
            break;
        }
        else
        {
            std::cout << "❌ 无效选项，请输入 1 - 4。" << std::endl;
        }

        std::cout << "\n按 Enter 返回菜单...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return 0;
}
