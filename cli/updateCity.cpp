#include <conio.h>
#include <iostream>
#include <curl/curl.h>

#include "cli_clear_console.h"
#include "config_key.h"
#include "config_user.h"
#include "delay.h"
#include "weather_manager.h"

void updateCity(ConfigUser& configUser, ConfigKey& configKey)
{
    SetConsoleOutputCP(CP_UTF8);

    WeatherManager wm(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());

    std::string keyword;
    std::vector<CityResult> matches;

    while (true)
    {
        clearConsole();

        std::cout << "📍 主菜单 > 设置城市\n";

        if (!keyword.empty())
        {
            matches = wm.searchCity(keyword,configUser.getLanguage());
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
