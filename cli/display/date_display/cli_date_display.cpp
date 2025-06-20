//
// Created by 13033 on 2025/6/16.
//
#include "cli_date_display.h"

#include <iostream>
#include <conio.h>

#include "doubao_translator.h"
#include "lunar_api.h"
#include "../../../core/CacheManager.h"
#include "../../common/cli_clear_console.h"
#include "common/cli_context.h"


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

void showCurrentDate(CliContext& ctx, bool showAll) {
    auto configUser = ctx.config.user();
    auto configKey = ctx.config.key();
    CacheManager cacheManager(configUser.getConfigJson());

    std::string lunarInfo;
    bool isFromCache = false;

    while (true) {
        clearConsole();
        std::time_t now = std::time(nullptr);

        lunarInfo = cacheManager.getCache("lunar_info");
        if (lunarInfo.empty()) {
            lunarInfo = getLunarInfo(ctx.config, ctx.i18n);
            cacheManager.setCache("lunar_info", lunarInfo);
            isFromCache = false;
        } else {
            isFromCache = true;
        }

        if (isFromCache) {
            std::cout << "(来自缓存)" << std::endl;
        } else {
            std::cout << "(来自网络)" << std::endl;
        }

        std::cout << "\t" << ctx.i18n.tr("date_view", "solar") << ": "
                  << std::put_time(std::localtime(&now), configUser.getDateFormateMenu().c_str()) << std::endl;

        if (showAll) {
            if (configUser.getLanguage() == "en") {
                std::cout << lunarInfo;
                std::cout << "Waiting for translation..." << std::endl;
                lunarInfo = translateWithDoubao(lunarInfo, "English", configKey);
                clearConsole();
                if (isFromCache) {
                    std::cout << "(来自缓存)" << std::endl;
                } else {
                    std::cout << "(来自网络)" << std::endl;
                }
                std::cout << "\t" << ctx.i18n.tr("date_view", "solar") << ": "
                          << std::put_time(std::localtime(&now), configUser.getDateFormateMenu().c_str()) << std::endl;
                std::cout << lunarInfo;
            } else {
                std::cout << lunarInfo;
            }
        }

        std::cout << std::flush;

        if (ctx.mode == CliMode::Interactive) {
            std::cout << i18n.tr("life_index", "prompt_refresh") << std::flush;
            char ch = _getch();
            if (ch == 'R' || ch == 'r') {
                cacheManager.clearCache("lunar_info");
                continue;
            }
        }
        break;
    }
}