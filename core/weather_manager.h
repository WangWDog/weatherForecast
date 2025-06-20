#pragma once
#include <ctime>
#include <string>
#include <vector>

// 每日天气预报数据结构
struct DailyForecast {
    std::string date;          // 日期 (格式: YYYY-MM-DD)
    std::string textDay;       // 白天天气描述 (如: "晴")
    std::string textNight;     // 夜间天气描述 (如: "多云")
    std::string tempMin;       // 最低温度 (如: "18")
    std::string tempMax;       // 最高温度 (如: "25")
    std::string windDirDay;    // 白天风向 (如: "东南风")
    std::string windScaleDay;  // 白天风力等级 (如: "3级")
    std::string precip;        // 降水量 (毫米) (如: "0.0")
    std::string humidity;      // 湿度 (%) (如: "65")
};

// 城市搜索结果数据结构
struct CityResult {
    std::string name;    // 城市名称 (如: "北京")
    std::string id;      // 城市ID (气象API使用的编码) (如: "101010100")
    std::string country; // 国家名称 (如: "中国")
    std::string adm1;    // 一级行政区 (省/州) (如: "北京")
    std::string adm2;    // 二级行政区 (市/县) (如: "北京")
};

// 生活指数数据结构
struct LifeIndex {
    std::string date;     // 日期 (格式: YYYY-MM-DD)
    std::string name;     // 指数名称 (如: "紫外线指数")
    std::string level;    // 等级 (如: "中等")
    std::string category; // 类别 (如: "防护")
    std::string text;     // 详细描述和建议 (如: "建议涂抹SPF15防晒霜")
};

// 带元数据的生活指数结果
struct LifeIndexWithMeta {
    std::vector<LifeIndex> indices; // 生活指数列表
    bool fromCache = false;         // 数据是否来自缓存
    std::time_t timestamp = 0;      // 数据获取时间戳
};

// 带元数据的天气预报结果
struct ForecastResult {
    std::vector<DailyForecast> forecasts; // 天气预报列表
    bool fromCache = false;               // 数据是否来自缓存
    std::time_t timestamp = 0;            // 数据获取时间戳
};

// 当前天气数据
struct WeatherNowData {
    std::string temp;       // 当前温度 (如: "22")
    std::string text;       // 当前天气状况 (如: "晴")
    std::string windSpeed;  // 当前风速 (如: "10km/h")
    std::string humidity;   // 当前湿度 (%) (如: "65")
};

// 带元数据的当前天气结果
struct WeatherNow {
    WeatherNowData data;     // 天气数据
    bool success = false;    // 是否成功获取数据
    bool fromCache = false;  // 数据是否来自缓存
    std::time_t timestamp = 0; // 数据获取时间戳
};

// 天气管理类：负责与天气API交互
class WeatherManager {
public:
    // 构造函数：初始化API密钥、主机地址和语言
    // key: 天气API密钥
    // host: API服务主机地址
    // lang: 返回数据的语言 (默认中文)
    explicit WeatherManager(std::string key, std::string host, std::string lang = "zh");

    // 城市搜索功能
    // keyword: 搜索关键词 (如: "北京")
    // lang: 返回结果的显示语言
    // 返回: 匹配的城市列表
    std::vector<CityResult> searchCity(const std::string& keyword, const std::string& lang);

    // 获取生活指数
    // locationId: 城市ID
    // expiryMinutes: 缓存有效期(分钟)
    // 返回: 生活指数数据及元信息
    LifeIndexWithMeta getLifeIndices(const std::string& locationId, int expiryMinutes);

    // 获取7天天气预报
    // locationId: 城市ID
    // language: 返回数据的语言
    // cacheExpiryMinutes: 缓存有效期(分钟)
    // 返回: 天气预报数据及元信息
    ForecastResult get7DayForecast(const std::string& locationId, const std::string& language, int cacheExpiryMinutes);

    // 获取缓存的当前天气
    // locationId: 城市ID
    // 返回: 当前天气数据及元信息
    WeatherNow getCachedCurrentWeather(const std::string& locationId);

private:
    std::string apiKey;  // 天气API密钥
    std::string host;    // API服务主机地址
    std::string lang;    // 默认语言
};
