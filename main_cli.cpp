#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>

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

// 显示天气
void showWeatherForecast(const std::string& city, const std::string& apiKey)
{
    //WeatherManager manager(apiKey);
    //auto forecasts = manager.get7DayForecast(city);

    std::cout << "📡 城市：" << city << std::endl;
    std::cout << "🗓️ 未来7天天气预报：" << std::endl;
    // for (const auto& f : forecasts) {
    //     std::cout << f.date << "：" << f.text << "，最高" << f.tempMax << "°C，最低" << f.tempMin << "°C" << std::endl;
    // }
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

void updateCity(ConfigUser& configUser, ConfigKey& configKey)
{
    WeatherManager wm(configKey.getApiKey(), configKey.getHost());

    while (true) {
        clearConsole();
        std::cout << "📍 主菜单 > 设置城市\n";
        std::cout << "🔍 请输入城市关键字（输入 :q 返回主菜单）：";

        std::string newCity;
        std::getline(std::cin, newCity);

        if (newCity == ":q" || newCity == "q" || newCity == "exit") {
            std::cout << "↩️ 已取消设置，返回主菜单。" << std::endl;
            return;
        }

        if (newCity.empty()) {
            std::cout << "⚠️ 输入为空，请重新输入。" << std::endl;
            delay_ms(1200);
            continue;
        }

        auto matches = wm.searchCity(newCity);

        if (matches.empty()) {
            std::cout << "\n❌ 未找到匹配的城市，请重试..." << std::endl;
            delay_ms(1500);
            // continue;
        }

        std::cout << "\n📍 主菜单 > 设置城市 > 选择城市\n";
        std::cout << "🔎 匹配结果（共 " << matches.size() << " 个）：" << std::endl;

        for (size_t i = 0; i < matches.size(); ++i) {
            const auto& c = matches[i];
            std::cout << std::setw(2) << (i + 1) << ". " << c.name
                      << " | " << c.adm1 << " · " << c.adm2 << " · " << c.country
                      << " [ID: " << c.id << "]" << std::endl;
        }

        std::cout << "\n请输入城市编号以选择（空行返回主菜单）：";
        std::string choiceStr;
        std::getline(std::cin, choiceStr);

        if (choiceStr.empty()) {
            std::cout << "↩️ 未选择城市，返回主菜单。" << std::endl;
            return;
        }

        int choice;
        try {
            choice = std::stoi(choiceStr);
        } catch (...) {
            std::cout << "❌ 输入无效，返回主菜单。" << std::endl;
            return;
        }

        if (choice <= 0 || choice > matches.size()) {
            std::cout << "❌ 编号超出范围，返回主菜单。" << std::endl;
            return;
        }

        const auto& selected = matches[choice - 1];
        configUser.setDefaultCity(selected.name);
        configUser.setCityId(selected.id);
        configUser.save();
        std::cout << "\n✅ 城市设置成功："
                  << selected.name << "（" << selected.adm1 << " · " << selected.country << "）" << std::endl;
        return;
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
            showSpinner("正在获取天气信息", 2000, 100, "\033[36m");
            //showWeatherForecast(config.getDefaultCity(), config.getApiKey());
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
