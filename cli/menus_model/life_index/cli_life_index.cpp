#include <conio.h>
#include <iostream>

#include "CacheManager.h"
#include "config_context.h"
#include "i18n_loader.h"
#include "weather_manager.h"
#include "../../common/cli_clear_console.h"

// 显示生活指数
void showLifeIndices(ConfigContext& cft,I18n& i18n) {
    // 从配置上下文中获取密钥配置和用户配置
    auto configKey = cft.key();
    auto configUser = cft.user();
    // 创建天气管理器实例，传入API密钥、主机地址和语言设置
    WeatherManager manager(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());

    // 强制刷新缓存，传入用户配置
    CacheManager cacheManager(configUser.getConfigJson());
    cacheManager.clearAllCache();  // 强制清除所有缓存数据，确保获取最新数据


    // 初次加载（尝试用缓存）
    auto result = manager.getLifeIndices(configUser.getCityId(), configUser.getCacheExpiry("weather_index"));

    // 如果获取的生活指数数据为空，显示获取失败信息
    if (result.indices.empty()) {
        std::cout << i18n.tr("life_index", "fetch_failed") << "\n";  // 使用翻译获取 "无法获取生活指数数据"
        return; // 直接退出函数
    }

    while (true) {
        clearConsole();  // 清空控制台

        // 缓存信息展示，显示数据来源信息（缓存/网络）
        if (result.fromCache) {
            std::cout << i18n.tr("forecast", "from_cache") << "\n";  // 使用翻译获取 "当前数据来自缓存"
        } else {
            std::cout << i18n.tr("forecast", "from_network") << "\n";  // 使用翻译获取 "当前数据来自网络"
        }

        // 如果存在有效时间戳
        if (result.timestamp > 0) {
            char buf[64];
            std::tm *local = std::localtime(&result.timestamp); // 转换为本地时间
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", local); // 格式化时间
            std::cout << i18n.tr("forecast", "updated_time") << buf << "\n";  // 使用翻译获取 "数据更新时间"
        }

        // 生活指数展示
        std::cout << i18n.tr("life_index", "title") << configUser.getDefaultCity() << i18n.tr("life_index", "index_list") << "\n\n";  // 翻译 "城市：<city> 的生活指数如下："

        // 遍历所有生活指数条目        for (const auto &idx: result.indices) {
            std::cout << i18n.tr("life_index", "date") << ": " << idx.date << "\n"  // 翻译 "日期："
                      << i18n.tr("life_index", "type") << ": " << idx.name << "\n"  // 翻译 "类型："
                      << i18n.tr("life_index", "level") << ": " << idx.level << "（" << idx.category << "）\n"  // 翻译 "等级："
                      << i18n.tr("life_index", "suggestion") << ": " << idx.text << "\n"  // 翻译 "建议："
                      << "------------------------\n";
        }


    // 用户操作提示
        std::cout << "\n" << i18n.tr("life_index", "prompt_refresh") << "\n";  // 翻译 "按 R 刷新数据，任意其他键返回主菜单..."

        // 等待用户按键
        char ch = _getch();  // 无回显获取单个字符

        // 判断用户输入
        if (ch == 'R' || ch == 'r') {
            // 强制刷新：设置过期时间为0立即从网络获取新数据
            result = manager.getLifeIndices(configUser.getCityId(), 0); // 设置过期时间为 0 强制刷新
        } else {
            break; // 退出循环返回主菜单
        }
    }
}
