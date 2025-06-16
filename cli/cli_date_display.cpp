//
// Created by 13033 on 2025/6/16.
//
#include "cli_date_display.h"

#include <iostream>

#include "doubao_translator.h"
#include "lunar_api.h"
#include "../core/CacheManager.h"
#include "displayUtils/cli_clear_console.h"


std::string getLunarInfo(ConfigContext& cft,I18n &i18n) {
    auto config_key = cft.key();
    auto config_user = cft.user();
    std::string response = callLunarApi(config_key.getFreeApiKey()); // 请求 API(Key and 语言，返回json

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
        return lunarInfo;
    } catch (const std::exception &e) {
        return std::string("❌ JSON 解析失败：") + e.what();
    }
}

void showCurrentDate(ConfigContext& config_context, I18n& i18n, bool showAll) {
    auto configUser = config_context.user();
    auto configKey = config_context.key();
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
        lunarInfo = getLunarInfo(config_context, i18n);
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
        if (configUser.getLanguage() == "en") {
            // 先输出中文黄历信息
            std::cout << lunarInfo;

            std::cout << "Waiting for translation..." << std::endl;
            lunarInfo = translateWithDoubao(lunarInfo, "English", configKey);
            clearConsole();

            // 再次显示公历时间和数据来源
            std::cout << "\t" << i18n.tr("date_view", "solar") << ": "
                      << std::put_time(std::localtime(&now), configUser.getDateFormateMenu().c_str()) << std::endl;
            if (isFromCache) {
                std::cout << "(来自缓存)" << std::endl;
            } else {
                std::cout << "(来自网络)" << std::endl;
            }

            // 输出翻译后的黄历信息
            std::cout << lunarInfo;
        } else {
            // 非英文语言，直接输出黄历信息
            std::cout << lunarInfo;
        }
    }
    std::cout << std::flush; // 强制刷新输出
    std::cout << i18n.tr("date_view", "return_hint")<<std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}