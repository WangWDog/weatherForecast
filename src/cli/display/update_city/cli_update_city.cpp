#include "../../common/cli_getch.h"
#include <iostream>
#include <curl/curl.h>

#include "../../common/cli_clear_console.h"
#include "config_context.h"
#include "../../../config/config_key.h"
#include "../../../config/config_user.h"
#include "../../common/delay.h"
#include "weather_manager.h"
#include "../../i18n/i18n_loader.h"
#include "common/cli_context.h"

void showCityChoose(CliContext& cli) {

    auto& configKey = cli.config.key();
    auto& configUser = cli.config.user();
    auto& i18n = cli.i18n;
    auto& cacheManager = cli.cache;

    WeatherManager wm(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());

    std::string keyword;
    std::vector<CityResult> matches;

    while (true) {
        clearConsole();

        // 显示主菜单
        std::cout << i18n.tr("searchCity", "title") << "\n";  // 翻译 "主菜单 > 设置城市"

        if (!keyword.empty()) {
            matches = wm.searchCity(keyword, configUser.getLanguage());
            std::cout << " 🔎 " << keyword << i18n.tr("searchCity", "searching") << "\n";  // 翻译 "搜索结果"
            if (matches.empty()) {
                std::cout << "\t" << i18n.tr("searchCity", "not_found") << "\n";  // 翻译 "未找到匹配城市"
            } else {
                int size = matches.size();
                if (size > 9) size = 9;
                for (size_t i = 0; i < size; ++i) {
                    const auto& c = matches[i];
                    std::cout << "\t";
                    std::cout << i + 1 << ". " << c.name
                        << " | " << c.adm1 << " · " << c.adm2 << " · " << c.country
                        << " [ID: " << c.id << "]\n";
                }
                std::cout << i18n.tr("searchCity", "select_prompt") << "\n";  // 翻译 "输入编号选择城市，或继续输入关键字..."
            }
        }

        // 输入提示
        std::cout << i18n.tr("searchCity", "input_hint") << keyword;  // 翻译 "输入城市关键字（输入 : 返回主菜单）"

        char ch = cli_getch();
        if (ch == ':' || keyword == ":") {
            std::cout << "\n" << i18n.tr("searchCity", "cancelled") << std::endl;  // 翻译 "已取消设置，返回主菜单"
            delay_ms(2000);
            return;
        } else if (ch == '\n') {
            return;
        } else if (ch == '\b' && !keyword.empty()) {
            keyword.pop_back();  // 退格删除一个字符
        } else if (ch >= '0' && ch <= '9' && !matches.empty()) {
            int index = ch - '0';
            if (index >= 1 && index <= matches.size()) {
                const auto& selected = matches[index - 1];
                configUser.setCityName(selected.name);
                configUser.setCityId(selected.id);
                configUser.save();
                configUser.load();
                //由于信息与城市有关 所以换城市后要清除缓存
                cacheManager.clearAll();
                std::cout << "\n" << i18n.tr("searchCity", "success") << ": " << selected.name
                    << "（" << selected.adm1 << " · " << selected.country << "）" << std::endl;  // 翻译 "城市设置成功"
                return;
            }
        } else {
            keyword += ch;  // 将字符添加到关键字中
        }
    }
}
