#include <conio.h>
#include <iostream>
#include <curl/curl.h>

#include "../../common/cli_clear_console.h"
#include "config_context.h"
#include "../../../config/config_key.h"
#include "../../../config/config_user.h"
#include "../../common/delay.h"
#include "weather_manager.h"
#include "../../i18n/i18n_loader.h"

// 显示城市选择界面
void showCityChoose(ConfigContext& config_context, I18n& i18n) {
    SetConsoleOutputCP(CP_UTF8);  // 设置控制台编码为 UTF-8

    // 从配置上下文中获取API密钥配置和用户配置
    auto configKey = config_context.key();
    auto configUser = config_context.user();

    // 创建天气管理器实例（传入API密钥、主机地址和语言）
    WeatherManager wm(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());

    std::string keyword;                // 存储用户输入的关键字
    std::vector<CityResult> matches;    // 存储匹配的城市结果

    // 处理城市搜索和选择
    while (true) {
        clearConsole();  // 清空控制台

        // 显示主菜单
        std::cout << i18n.tr("searchCity", "title") << "\n";  // 翻译 "主菜单 > 设置城市"

        if (!keyword.empty()) {
            // 调用天气管理器的搜索方法获取匹配城市
            matches = wm.searchCity(keyword, configUser.getLanguage());

            // 显示搜索状态
            std::cout << " 🔎 " << keyword << i18n.tr("searchCity", "searching") << "\n";  // 翻译 "搜索结果"

            // 处理搜索结果
            if (matches.empty()) {
                // 没有找到匹配城市
                std::cout << "\t" << i18n.tr("searchCity", "not_found") << "\n";  // 翻译 "未找到匹配城市"
            } else {
                // 限制最多显示9个结果（控制台空间有限）
                int size = matches.size();
                if (size > 9) size = 9;

                // 遍历并显示搜索结果
                for (size_t i = 0; i < size; ++i) {
                    const auto& c = matches[i];  // 获取城市结果
                    std::cout << "\t";           // 缩进排版
                    std::cout << i + 1 << ". " << c.name
                        << " | " << c.adm1 << " · " << c.adm2 << " · " << c.country
                        << " [ID: " << c.id << "]\n";  // 显示城市层级信息
                }
                // 选择提示
                std::cout << i18n.tr("searchCity", "select_prompt") << "\n";  // 翻译 "输入编号选择城市，或继续输入关键字..."
            }
        }

        // 输入提示
        std::cout << i18n.tr("searchCity", "input_hint") << keyword;  // 翻译 "输入城市关键字（输入 : 返回主菜单）"

        // 显示输入提示（当前已输入的关键字）
        char ch = _getch();
        if (ch == ':' || keyword == ":") {
            std::cout << "\n" << i18n.tr("searchCity", "cancelled") << std::endl;  // 翻译 "已取消设置，返回主菜单"
            delay_ms(2000);  // 延迟2秒让用户看到提示
            return;
        } else if (ch == '\n') {
            return;               // 回车键处理
        } else if (ch == '\b' && !keyword.empty()) {
            keyword.pop_back();  // 退格删除一个字符
        } else if (ch >= '0' && ch <= '9' && !matches.empty()) {   // 数字选择：用户选择搜索结果
            int index = ch - '0';  // 字符转数字

            // 验证选择是否有效
            if (index >= 1 && index <= matches.size()) {
                const auto& selected = matches[index - 1];  // 获取选中的城市

                // 更新用户配置
                configUser.setDefaultCity(selected.name);  // 设置默认城市名
                configUser.setCityId(selected.id);         // 设置城市ID
                configUser.save();                         // 保存配置到文件

                // 显示成功消息
                std::cout << "\n" << i18n.tr("searchCity", "success") << ": " << selected.name
                    << "（" << selected.adm1 << " · " << selected.country << "）" << std::endl;  // 翻译 "城市设置成功"
                return;
            }
        } else {
            keyword += ch;  // 将字符添加到关键字中
        }
    }
}
