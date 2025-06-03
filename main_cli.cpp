#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>

#include "cli_animation_loader.h"
#include "cli_clear_console.h"
#include "config_loader.h"
#include "weather_manager.h"  // 假设你在 weather_core 中定义好了

#ifdef _WIN32
    #include <windows.h>
#endif

// 显示当前日期
void showCurrentDate() {
    std::time_t now = std::time(nullptr);
    std::cout << "📅 当前日期与时间："
              << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << std::endl;
}

// 显示天气
void showWeatherForecast(const std::string& city, const std::string& apiKey) {
    //WeatherManager manager(apiKey);
    //auto forecasts = manager.get7DayForecast(city);

    std::cout << "📡 城市：" << city << std::endl;
    std::cout << "🗓️ 未来7天天气预报：" << std::endl;
    // for (const auto& f : forecasts) {
    //     std::cout << f.date << "：" << f.text << "，最高" << f.tempMax << "°C，最低" << f.tempMin << "°C" << std::endl;
    // }
}

// 设置城市
void updateCity(ConfigLoader& config) {
    std::string newCity;
    std::cout << "请输入新城市名称：";
    std::getline(std::cin, newCity);
    if (!newCity.empty()) {
        config.setDefaultCity(newCity);
        config.save();
        std::cout << "✅ 城市已更新为：" << newCity << std::endl;
    } else {
        std::cout << "❌ 输入为空，未做修改。" << std::endl;
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // Windows 控制台显示 UTF-8
#endif

    showLoadingBar("⚙️加载预设配置", 8, 400, "\033[38;5;117m");

    ConfigLoader config("config.json");
    if (!config.load()) {
        std::cerr << "配置加载失败，程序退出。" << std::endl;
        return 1;
    }

    while (true) {
        clearConsole();

        std::cout << "\n🏠 天气 CLI 系统菜单\n";
        std::cout << "--------------------------\n";
        std::cout << "1.🗓️ 显示当前日期\n";
        std::cout << "2.☁️ 显示天气预报\n";
        std::cout << "3.🚩 设置城市（当前：" << config.getDefaultCity() << "）\n";
        std::cout << "4.❌ 退出程序\n";
        std::cout << "--------------------------\n";
        std::cout << "请输入选项（1-4）：";

        std::string choice;
        std::getline(std::cin, choice);
        clearConsole();

        if (choice == "1") {
            showCurrentDate();
        } else if (choice == "2") {
            showSpinner("正在获取天气信息", 2000, 100, "\033[36m");
            //showWeatherForecast(config.getDefaultCity(), config.getApiKey());
        } else if (choice == "3") {
            updateCity(config);
        } else if (choice == "4") {
            std::cout << "👋 再见！感谢使用天气CLI系统。" << std::endl;
            break;
        } else {
            std::cout << "❌ 无效选项，请输入 1 - 4。" << std::endl;
        }

        std::cout << "\n按 Enter 返回菜单...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
4
    return 0;
}