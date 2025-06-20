#include <conio.h>
#include <iostream>

#include "CacheManager.h"
#include "config_context.h"
#include "i18n_loader.h"
#include "weather_manager.h"
#include "../../common/cli_clear_console.h"
#include "common/cli_context.h"

void showLifeIndices(CliContext& cli) {
    auto& configKey = cli.config.key();
    auto& configUser = cli.config.user();
    auto& i18n = cli.i18n;

    WeatherManager manager(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());

    auto result = manager.getLifeIndices(configUser.getCityId(), configUser.getCacheExpiry("weather_index"),cli.cache);

    if (result.indices.empty()) {
        std::cout << i18n.tr("life_index", "fetch_failed") << "\n";  // 使用翻译获取 "无法获取生活指数数据"
        return;
    }

    while (true) {
        clearConsole();

        // 缓存信息展示
        if (result.fromCache) {
            std::cout << i18n.tr("forecast", "from_cache") << "\n";  // 使用翻译获取 "当前数据来自缓存"
        } else {
            std::cout << i18n.tr("forecast", "from_network") << "\n";  // 使用翻译获取 "当前数据来自网络"
        }

        if (result.timestamp > 0) {
            char buf[64];
            std::tm *local = std::localtime(&result.timestamp);
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", local);
            std::cout << i18n.tr("forecast", "updated_time") << buf << "\n";  // 使用翻译获取 "数据更新时间"
        }

        // 生活指数展示
        std::cout << i18n.tr("life_index", "title") << configUser.getCityName() << i18n.tr("life_index", "index_list") << "\n\n";  // 翻译 "城市：<city> 的生活指数如下："
        for (const auto &idx: result.indices) {
            std::cout << i18n.tr("life_index", "date") << ": " << idx.date << "\n"  // 翻译 "日期："
                      << i18n.tr("life_index", "type") << ": " << idx.name << "\n"  // 翻译 "类型："
                      << i18n.tr("life_index", "level") << ": " << idx.level << "（" << idx.category << "）\n"  // 翻译 "等级："
                      << i18n.tr("life_index", "suggestion") << ": " << idx.text << "\n"  // 翻译 "建议："
                      << "------------------------\n";
        }


        std::cout << "\n" << i18n.tr("life_index", "prompt_refresh") << "\n";  // 翻译 "按 R 刷新数据，任意其他键返回主菜单..."
        char ch = _getch();
        if (ch == 'R' || ch == 'r') {
            result = manager.getLifeIndices(configUser.getCityId(), 0,cli.cache); // 设置过期时间为 0 强制刷新
        } else {
            break;
        }
    }
}
