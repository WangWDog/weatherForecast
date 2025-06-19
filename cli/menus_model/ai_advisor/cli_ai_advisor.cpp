//
// Created by 13033 on 2025/6/16.
//

#include "cli_ai_advisor.h"

#include <iostream>

#include "doubao_helper.h"
#include "weather_manager.h"
#include "../common/cli_clear_console.h"

void showAISuggestions(ConfigContext& ctx, I18n& i18n) {
    clearConsole();  // 清空控制台屏幕
    auto configKey = ctx.key();  // 获取API密钥配置
    auto configUser = ctx.user();  // 获取用户配置

    std::cout << "\t🌟 " << i18n.tr("ai_suggestion", "getting") << "\n";


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
    LifeIndexWithMeta lifeIndex = weatherManager.getLifeIndices(locationId,configUser.getCacheExpiry("daily_forecast"));
    // 构建生活指数的描述
    std::string lifeIndexSummary = "根据目前的生活指数，以下是一些重要信息：\n";
    for (const auto& idx : lifeIndex.indices) {
        lifeIndexSummary += "📅 " + idx.date + "\n" +
                            "📌 类型：" + idx.name + "\n" +
                            "📈 等级：" + idx.level + "（" + idx.category + "）\n" +
                            "📖 建议：" + idx.text + "\n" +
                            "------------------------\n";
    }

    // 构建 AI 请求体，包含天气和生活指数的内容
    std::string fullPrompt = "请你用"+configUser.getLanguage()+"语言回答:现在用户所在城市是 " + locationId +
                             "，当前气温为 " + temp +
                             "，天气状况为 " + condition +
                             "，风速为 " + windSpeed +
                             "，湿度为 " + humidity + "。\n" +
                             lifeIndexSummary +  // 添加生活指数信息
                             "请根据这些信息提供穿衣建议、运动建议与出行建议。";

    //std::cout << "构建的请求体： " << fullPrompt << std::endl;  // 输出查看请求体
    // 获取AI建议
    std::string suggestion = callDoubaoAI(token, endpointId, fullPrompt);

    // 输出 AI 给出的建议
    std::cout << "\n🤖 " << suggestion << std::endl;
    std::cout << "\n";
    std::cout << i18n.tr("ai_suggestion", "back_hint");

    std::cout << std::flush; // 强制刷新输出
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
