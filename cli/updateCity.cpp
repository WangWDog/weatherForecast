#include <conio.h>
#include <iostream>
#include <curl/curl.h>

#include "cli_clear_console.h"
#include "config_key.h"
#include "config_user.h"
#include "delay.h"
#include "weather_manager.h"
#include "i18n/i18n_loader.h"

void updateCity(ConfigUser& configUser, ConfigKey& configKey, I18n& i18n) {
    SetConsoleOutputCP(CP_UTF8);

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

        char ch = _getch();
        if (ch == ':' || keyword == ":") {
            std::cout << "\n" << i18n.tr("searchCity", "cancelled") << std::endl;  // 翻译 "已取消设置，返回主菜单"
            delay_ms(2000);
            return;
        } else if (ch == '\n') {
            return;
        } else if (ch == '\b' && !keyword.empty()) {
            keyword.pop_back();
        } else if (ch >= '0' && ch <= '9' && !matches.empty()) {
            int index = ch - '0';
            if (index >= 1 && index <= matches.size()) {
                const auto& selected = matches[index - 1];
                configUser.setDefaultCity(selected.name);
                configUser.setCityId(selected.id);
                configUser.save();
                std::cout << "\n" << i18n.tr("searchCity", "success") << ": " << selected.name
                    << "（" << selected.adm1 << " · " << selected.country << "）" << std::endl;  // 翻译 "城市设置成功"
                return;
            }
        } else {
            keyword += ch;
        }
    }
}
