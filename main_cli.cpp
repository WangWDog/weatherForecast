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
#include "doubao_translator.h"
#include "date_utils.h"  // 包含辅助函数头文件
#include "doubao_helper.h"  // 调用豆包函数
#include "CacheManager.h"





#ifdef _WIN32
#include <windows.h>
#endif

using json = nlohmann::json;

// 用于调用 API 获取农历、节气和黄历等信息
std::string getLunarInfo(ConfigKey &config_key, const std::string &lang, I18n &i18n) {
    std::string response = callLunarApi(config_key, lang); // 请求 API(Key and 语言，返回json

    if (response.empty()) {
        return "❌ 未获取到农历信息";//提示错误信息
    }

    try {
        auto j = nlohmann::json::parse(response);
        if (!j.contains("data")) return "❌ 响应数据无效";
        const auto &d = j["data"];//解析json
        std::ostringstream oss;

        auto printIfNotEmpty = [&](const std::string &emoji, const std::string &label, const std::string &key) {
            if (d.contains(key) && !d[key].get<std::string>().empty()) {
                oss << emoji << " " << label << "：" << d[key].get<std::string>() << "\n";
            }
        };//输出函数

        // 信息输出
        //printIfNotEmpty("\t📅", "公历", "Solar");
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

        std::string lunarInfo = oss.str();

        if (lang == "en") {
            lunarInfo = translateWithDoubao(lunarInfo, "英文", config_key);
        }//调用豆包翻译语言

        return lunarInfo;
    } catch (const std::exception &e) {
        return std::string("❌ JSON 解析失败：") + e.what();
    }
}


// 宽字符对齐工具函数（仅估算宽度）



void updateUserSettings(ConfigUser &configUser, I18n &i18n) {//configUser:封装用户设置。il8n:国际化翻译
    while (true) {
        clearConsole();
        std::cout << "🔧 " << i18n.tr("settings", "menu_title") << "\n";
        std::cout << "------------------------\n";
        std::cout << "1. 📅 " << i18n.tr("settings", "date_format") << "（" << configUser.getDateFormateMenu() << "）\n";
        std::cout << "2. 🧭 " << i18n.tr("settings", "cache_life_index") << "（" << configUser.
                getCacheExpiry("life_index") << " 分钟）\n";
        std::cout << "3. 🌦 " << i18n.tr("settings", "cache_forecast") << "（" << configUser.
                getCacheExpiry("daily_forecast") << " 分钟）\n";
        std::cout << "4. 🈯 " << i18n.tr("settings", "language") << "（" << configUser.getLanguage() << "） \n";
        std::cout << "5. 🔙 " << i18n.tr("settings", "back") << "\n";//输出五个菜单选项

        std::cout << i18n.tr("settings", "prompt_input");
        std::string choice;
        std::getline(std::cin, choice);//获取用户输入

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
        }//修改日期格式
        else if (choice == "2") {
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
        }//修改生活指数缓存时间
        else if (choice == "3") {
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
        }//修改天气预报缓存时间
        else if (choice == "4") {
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
        }//修改语言
        else if (choice == "5") {
            configUser.save();
            std::cout << i18n.tr("settings", "saved_and_exit") << "\n";
            return;
        } else {
            std::cout << i18n.tr("settings", "invalid_option") << "\n";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

using json = nlohmann::json;

std::vector<LifeIndex> getLifeIndices(ConfigUser& configUser, ConfigKey& configKey) {
    std::vector<LifeIndex> lifeIndices;

    // 构建 API 请求 URL
    std::string url = "http://api.weather.com/v1/lifeindex?city=" + configUser.getCityId() +
                      "&language=" + configUser.getLanguage() + "&apiKey=" + configKey.getHFApiKey();

    // 初始化 cURL 请求
    CURL* curl = curl_easy_init();
    if (curl) {
        // 设置请求 URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // 设置响应写入回调函数
        std::string response_data;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](char* ptr, size_t size, size_t nmemb, std::string* data) -> size_t {
            data->append(ptr, size * nmemb);
            return size * nmemb;
        });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        // 执行请求
        CURLcode res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            // 解析 JSON 响应
            try {
                json j = json::parse(response_data);

                // 假设 API 返回一个 "lifeIndices" 数组
                if (j.contains("lifeIndices")) {
                    for (const auto& item : j["lifeIndices"]) {
                        LifeIndex index;
                        index.date = item.value("date", "");
                        index.name = item.value("name", "");
                        index.level = item.value("level", "");
                        index.category = item.value("category", "");
                        index.text = item.value("text", "");

                        lifeIndices.push_back(index);
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "解析生活指数数据失败: " << e.what() << std::endl;
            }
        } else {
            std::cerr << "请求失败: " << curl_easy_strerror(res) << std::endl;
        }

        // 清理 cURL
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "无法初始化 cURL!" << std::endl;
    }

    return lifeIndices;
}

void showAISuggestions(ConfigUser& configUser, ConfigKey& configKey, I18n& i18n) {
    clearConsole();
    std::cout << "\t🌟 " << i18n.tr("ai_suggestion", "loading") << "\n";

    // 获取豆包参数
    std::string token = configKey.getDoubaoKey();  // 使用 configKey 获取正确的 API 密钥
    std::string endpointId = configKey.getDoubaoEndpoint();

    // 获取城市
    std::string locationId = configUser.getCityId();

    // 创建 WeatherManager 对象
    WeatherManager weatherManager(configKey.getHFApiKey(), configKey.getHFHost());  // 确保API密钥与 host 兼容

    // 获取当前天气数据
    auto weatherResult = weatherManager.get7DayForecast(locationId, configUser.getLanguage(), configUser.getCacheExpiry("daily_forecast"));
    auto weather = weatherResult.forecasts[0];
    // 从 weather 中获取数据
    std::string temp = weather.tempMax;  // 当前温度
    std::string condition = weather.textDay;  // 当前天气状况
    std::string windSpeed = weather.windDirDay;  // 获取实际风速数据，检查实际字段名
    std::string humidity = weather.humidity;  // 获取湿度数据，若有数据的话

    // 获取生活指数数据
    std::vector<LifeIndex> lifeIndices = getLifeIndices(configUser, configKey);  // 调用 getLifeIndices 获取生活指数

    // 构建生活指数的描述
    std::string lifeIndexSummary = "根据目前的生活指数，以下是一些重要信息：\n";
    for (const auto& idx : lifeIndices) {
        lifeIndexSummary += "📅 " + idx.date + "\n" +
                            "📌 类型：" + idx.name + "\n" +
                            "📈 等级：" + idx.level + "（" + idx.category + "）\n" +
                            "📖 建议：" + idx.text + "\n" +
                            "------------------------\n";
    }

    // 构建 AI 请求体，包含天气和生活指数的内容
    std::string fullPrompt = "现在用户所在城市是 " + locationId +
                             "，当前气温为 " + temp +
                             "，天气状况为 " + condition +
                             "，风速为 " + windSpeed +
                             "，湿度为 " + humidity + "。\n" +
                             lifeIndexSummary +  // 添加生活指数信息
                             "请根据这些信息提供穿衣建议、运动建议与出行建议。";

    std::cout << "构建的请求体： " << fullPrompt << std::endl;  // 输出查看请求体

    // 获取AI建议
    std::string suggestion = callDoubaoAI(token, endpointId, fullPrompt);

    // 输出 AI 给出的建议
    std::cout << "\n🤖 " << suggestion << std::endl;
    std::cout << "\n" << i18n.tr("main_cli", "return_hint");
    _getch();  // 等待任意键返回主菜单
}

// 显示当前日期
std::string fetchLunarInfoFromNetwork(ConfigKey& configKey, const std::string& language, I18n& i18n) {
    // 从网络获取农历信息的代码
    return getLunarInfo(configKey, language, i18n); // 假设 getLunarInfo 是现有的网络获取农历的函数
}
void showCurrentDate(ConfigUser &configUser, ConfigKey &configKey, I18n &i18n, bool showAll) {
    clearConsole(); // 清空控制台

    // 获取当前时间
    std::time_t now = std::time(nullptr);
    std::tm *currentTime = std::localtime(&now);

    // 缓存管理器实例
    CacheManager cacheManager(configUser.getConfigJson());


    // 检查缓存中是否有农历信息
    std::string lunarInfo = cacheManager.getCache("lunar_info");
    bool isFromCache = false;  // 用于标记数据来源

    if (lunarInfo.empty()) {
        // 如果缓存中没有数据或过期，从网络获取
        lunarInfo = fetchLunarInfoFromNetwork(configKey, configUser.getLanguage(), i18n);
        // 缓存农历信息
        cacheManager.setCache("lunar_info", lunarInfo);
        isFromCache = false;  // 数据来源是网络
    } else {
        isFromCache = true;  // 数据来源是缓存
    }

    // 显示公历时间
    std::cout << "\t" << i18n.tr("date_view", "solar") << ": "
              << std::put_time(std::localtime(&now), configUser.getDateFormateMenu().c_str()) << std::endl;

    if (isFromCache) {
        std::cout << "(来自缓存)" << std::endl;
    } else {
        std::cout << "(来自网络)" << std::endl;
    }

    if (showAll) {
        // 显示农历信息
        std::cout << lunarInfo;
    }

    std::cout << std::flush; // 强制刷新输出
}


void printLine() {
    std::cout << "+--------------+--------------+--------------+--------------+"
                 "--------------+--------------+--------------+--------------+\n";
}
void displayWeather(ForecastResult &result, I18n &i18n, ConfigUser &configUser) {
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

    std::cout << "\n" << i18n.tr("forecast", "city") << configUser.getDefaultCity()
              << "（ID: " << configUser.getCityId() << "）\n\n";
    std::cout << i18n.tr("forecast", "forecast_title") << "\n\n";

    printLine();
    std::cout << "| " << centerText(i18n.tr("forecast", "date"), 12)
              << " | " << centerText(i18n.tr("forecast", "text_day"), 12)
              << " | " << centerText(i18n.tr("forecast", "text_night"), 12)
              << " | " << centerText(i18n.tr("forecast", "temperature"), 12)
              << " | " << centerText(i18n.tr("forecast", "wind_dir"), 12)
              << " | " << centerText(i18n.tr("forecast", "wind_scale"), 12)
              << " | " << centerText(i18n.tr("forecast", "precip"), 12)
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




void displayWeatherCommander(ConfigUser &configUser, ConfigKey &configKey, I18n &i18n, bool forceRefresh) {
    WeatherManager manager(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());
    ForecastResult result;
    if (forceRefresh) {
        result = manager.get7DayForecast(configUser.getCityId(), configUser.getLanguage(), 0);
    } else {
        result = manager.get7DayForecast(configUser.getCityId(), configUser.getLanguage(),
                                         configUser.getCacheExpiry("daily_forecast"));
    }


    if (result.forecasts.empty()) {
        std::cout << i18n.tr("forecast", "fetch_failed") << std::endl;
        return;
    }

    displayWeather(result, i18n, configUser);
    std::cout << "\n" << i18n.tr("forecast", "commander_prompt_refresh") << "\n";
}

void showWeatherForecast(ConfigUser &configUser, ConfigKey &configKey, I18n &i18n) {
    WeatherManager manager(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());

    auto result = manager.get7DayForecast(configUser.getCityId(), configUser.getLanguage(),
                                          configUser.getCacheExpiry("daily_forecast"));

    if (result.forecasts.empty()) {
        std::cout << i18n.tr("forecast", "fetch_failed") << std::endl;
        return;
    }

    while (true) {
        displayWeather(result, i18n, configUser);
        std::cout << "\n" << i18n.tr("forecast", "prompt_refresh") << "\n";
        char ch = _getch();
        if (ch == 'R' || ch == 'r') {
            result = manager.get7DayForecast(configUser.getCityId(), configUser.getLanguage(), 0); // 强制刷新
        } else {
            break;
        }
    }
}


void showLifeIndices(ConfigUser &configUser, ConfigKey &configKey) {
    WeatherManager manager(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());

    // 初次加载（尝试用缓存）
    auto result = manager.getLifeIndices(configUser.getCityId(), configUser.getCacheExpiry("weather_index"));

    if (result.indices.empty()) {
        std::cout << "❌ 无法获取生活指数数据。\n";
        return;
    }

    while (true) {
        clearConsole();

        // 缓存信息展示
        if (result.fromCache) {
            std::cout << "📦 当前数据来自缓存。\n";
        } else {
            std::cout << "🌐 当前数据来自网络。\n";
        }

        if (result.timestamp > 0) {
            char buf[64];
            std::tm *local = std::localtime(&result.timestamp);
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", local);
            std::cout << "🕒 数据更新时间：" << buf << "\n";
        }

        // 生活指数展示
        std::cout << "📌 城市：" << configUser.getDefaultCity() << " 的生活指数如下：\n\n";
        for (const auto &idx: result.indices) {
            std::cout << "📅 日期：" << idx.date << "\n"
                    << "📌 类型：" << idx.name << "\n"
                    << "📈 等级：" << idx.level << "（" << idx.category << "）\n"
                    << "📖 建议：" << idx.text << "\n"
                    << "------------------------\n";
        }

        std::cout << "\n🔁 按 R 刷新数据，任意其他键返回主菜单...\n";
        char ch = _getch();
        if (ch == 'R' || ch == 'r') {
            result = manager.getLifeIndices(configUser.getCityId(), 0); // 设置过期时间为 0 强制刷新
        } else {
            break;
        }
    }
}

void showCommandHelp() {
    std::cout << "Available commands:\n";
    std::cout << "  show_date        - Show current date and time\n";
    std::cout << "  show_forecast    - Show 7-day weather forecast\n";
    std::cout << "  show_life        - Show life indices for the city\n";
    std::cout << "  update_city      - Update the city setting\n";
    std::cout << "  update_settings  - Update user settings\n";
    std::cout << "  exit             - Exit the application\n";
}

void handleCommand(int argc, char *argv[], ConfigUser &configUser, ConfigKey &configKey, I18n &i18n) {
    // 判断是否为 "show_date" 命令
    std::string command = argv[1];

    if (command == "show_date") {
        bool showAll = false;
        if (argc > 2) {
            std::string type = argv[2];
            if (type.find("--all") != std::string::npos) {
                showAll = true; // 如果包含 --all，设置为 true
            }
        }
        // 判断是否传入 --all 参数
        showCurrentDate(configUser, configKey, i18n, showAll); // 传递 showAll 参数
    } else if (command == "show_forecast") {
        bool forceRefresh = false;
        if (argc > 2) {
            std::string type = argv[2];
            if (type.find("-R") != std::string::npos) {
                forceRefresh = true; // 如果包含 --all，设置为 true
            }
        }
        displayWeatherCommander(configUser, configKey, i18n, forceRefresh);
    } else if (command == "show_life") {
        showLifeIndices(configUser, configKey);
    } else if (command == "update_city") {
        updateCity(configUser, configKey);
    } else if (command == "update_settings") {
        updateUserSettings(configUser, i18n);
    } else if (command == "exit") {
        std::cout << i18n.tr("main_cli", "goodbye") << std::endl;
        delay_ms(2000);
        exit(0); // Exits the program
    } else {
        showCommandHelp();
        std::cout << "❌ Invalid command! Type 'help' for a list of commands." << std::endl;
    }
}


int main(int argc, char *argv[]) {
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

    std::cout << "Number of arguments: " << argc << std::endl;

    // 输出每个参数
    for (int i = 0; i < argc; ++i) {
        std::cout << "Argument " << i << ": " << argv[i] << std::endl;
    }

    // 如果有命令行参数，则根据命令行参数执行相应操作
    if (argc > 1) {
        handleCommand(argc, argv, configUser, configKey, i18n);
    } else {
        // 没有命令行参数，则进入交互式菜单
        showLoadingBar("⚙️加载预设配置", 8, 40, "\033[38;5;117m");
        while (true) {
            clearConsole();

            std::cout << "\n" << i18n.tr("main_cli", "menu_title") << "\n";
            std::cout << "------------------------\n";
            auto options = i18n.trList("main_cli", "menu_options");
            for (size_t i = 0; i < options.size(); ++i) {
                std::cout << i << ". " << options[i] << "\n";
            }
            std::cout << "------------------------\n";
            std::cout << i18n.tr("main_cli", "prompt_input") << std::flush;
            std::string choice;
            std::getline(std::cin, choice);
            clearConsole();
            // 处理不同的 choice 选项
            if (choice == "0") {
                showAISuggestions(configUser, configKey, i18n);
                std::cout << "\n" << i18n.tr("main_cli", "return_hint");
                _getch();
            } else if (choice == "1") {
                std::cout << i18n.tr("date_view", "title") << "\n";
                showCurrentDate(configUser, configKey, i18n, true);
                std::cout << "\n" << i18n.tr("date_view", "return_hint");
                _getch();
            }else if (choice == "2") {
                showWeatherForecast(configUser, configKey, i18n);
            } else if (choice == "3") {
                showLifeIndices(configUser, configKey);
            } else if (choice == "4") {
                updateCity(configUser, configKey);
                delay_ms(2000);
            } else if (choice == "5") {
                updateUserSettings(configUser, i18n);
            } else if (choice == "6") {
                std::cout << i18n.tr("main_cli", "goodbye") << std::endl;
                delay_ms(2000);
                break;
            } else {
                std::cout << i18n.tr("main_cli", "invalid_option") << std::endl;
            }
            std::cout << "\n" << i18n.tr("main_cli", "back_to_menu");
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    return 0;
}
