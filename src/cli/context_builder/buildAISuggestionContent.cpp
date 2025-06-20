
#include <iostream>
#include <string>
#include "config_context.h"
#include "../core/doubao_manager.h"
#include "weather_manager.h"
#include "common/cli_context.h"

std::string buildAISuggestionContent(CliContext& cli) {

    auto& configKey = cli.config.key();
    auto& configUser = cli.config.user();

    // 调用天气和生活指数获取逻辑
    WeatherManager weatherManager(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());
    std::cout << "\t🌟 " << cli.i18n.tr("ai_suggestion", "getting_weather_reference") << "\n";

    auto weatherResult = weatherManager.get7DayForecast(
        configUser.getCityId(),
        configUser.getLanguage(),
        configUser.getUnits(),
        configUser.getCacheExpiry("daily_forecast"),
        cli.cache);
    auto weather = weatherResult.forecasts[0];
    std::cout << "\t🌟 " << cli.i18n.tr("ai_suggestion", "getting_life_index_reference") << "\n";
    auto lifeIndex = weatherManager.getLifeIndices(configUser.getCityId(), configUser.getCacheExpiry("daily_forecast"),cli.cache);

    // 构造 AI Prompt
    std::ostringstream oss;
    std::string unit = configUser.getUnits();
    bool imperial = (unit == "imperial" || unit == "i");
    std::string tempUnit = imperial ? "℉" : "℃";
    oss << "请你用" << configUser.getLanguage() << "语言回答:现在用户所在城市是 "
        << configUser.getCityId() << "，当前气温为 " << weather.tempMax << tempUnit
        << "，天气状况为 " << weather.textDay << "，风向为 " << weather.windDirDay
        << "，风力为 " << weather.windScaleDay
        << "，湿度为 " << weather.humidity << "%。请使用以上单位给出生活建议。\n";

    for (const auto& idx : lifeIndex.indices) {
        oss << "📅 " << idx.date << "\n"
            << "📌 类型：" << idx.name << "\n"
            << "📈 等级：" << idx.level << "（" << idx.category << "）\n"
            << "📖 建议：" << idx.text << "\n"
            << "------------------------\n";
    }
    std::cout << "\t🌟 " << cli.i18n.tr("ai_suggestion", "loading") << "\n";
    DoubaoManager doubao(configKey.getDoubaoKey(), configKey.getDoubaoEndpoint());
    auto suggestion = doubao.getAdvice(oss.str());
    return suggestion;
}
