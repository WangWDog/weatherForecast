
#include <string>
#include "config_context.h"
#include "doubao_helper.h"
#include "weather_manager.h"

std::string buildAISuggestionContent(ConfigContext& ctx) {

    auto& configKey = ctx.key();
    auto& configUser = ctx.user();

    // 调用天气和生活指数获取逻辑
    WeatherManager weatherManager(configKey.getHFApiKey(), configKey.getHFHost(), configUser.getLanguage());
    auto weatherResult = weatherManager.get7DayForecast(configUser.getCityId(), configUser.getLanguage(), configUser.getCacheExpiry("daily_forecast"));
    auto weather = weatherResult.forecasts[0];

    auto lifeIndex = weatherManager.getLifeIndices(configUser.getCityId(), configUser.getCacheExpiry("daily_forecast"));

    // 构造 AI Prompt
    std::ostringstream oss;
    oss << "请你用" << configUser.getLanguage() << "语言回答:现在用户所在城市是 "
        << configUser.getCityId() << "，当前气温为 " << weather.tempMax
        << "，天气状况为 " << weather.textDay << "，风速为 " << weather.windDirDay
        << "，湿度为 " << weather.humidity << "。\n";

    for (const auto& idx : lifeIndex.indices) {
        oss << "📅 " << idx.date << "\n"
            << "📌 类型：" << idx.name << "\n"
            << "📈 等级：" << idx.level << "（" << idx.category << "）\n"
            << "📖 建议：" << idx.text << "\n"
            << "------------------------\n";
    }

    auto suggestion = callDoubaoAI(configKey.getDoubaoKey(), configKey.getDoubaoEndpoint(), oss.str());
    return suggestion;
}
