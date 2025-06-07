#pragma once
#include <ctime>
#include <string>
#include <vector>

struct DailyForecast {
    std::string date;
    std::string textDay;
    std::string textNight;
    std::string tempMin;
    std::string tempMax;
    std::string windDirDay;
    std::string windScaleDay;
    std::string precip;
    std::string humidity;
};

struct CityResult {
    std::string name;
    std::string id;
    std::string country;
    std::string adm1;
    std::string adm2;
};

struct LifeIndex {
    std::string date;
    std::string name;
    std::string level;
    std::string category;
    std::string text;
};

struct LifeIndexWithMeta {
    std::vector<LifeIndex> indices;
    bool fromCache = false;
    std::time_t timestamp = 0;
};

struct ForecastResult {
    std::vector<DailyForecast> forecasts;
    bool fromCache = false;
    std::time_t timestamp = 0;
};

struct WeatherNowData {
    std::string temp;  // 温度
    std::string text;  // 天气状况
    std::string windSpeed;  // 风速
    std::string humidity;  // 湿度
};


struct WeatherNow {
    WeatherNowData data;
    bool success = false;
    bool fromCache = false;
    std::time_t timestamp = 0;
};


class WeatherManager {
public:
    explicit WeatherManager(std::string key, std::string host, std::string lang = "zh");

    std::vector<DailyForecast> forecasts;
    std::vector<CityResult> searchCity(const std::string& keyword, const std::string& lang);
    LifeIndexWithMeta getLifeIndices(const std::string& locationId, int expiryMinutes);
    ForecastResult get7DayForecast(const std::string& locationId, const std::string& language, int cacheExpiryMinutes);
    WeatherNow getCachedCurrentWeather(const std::string& locationId);  // 新增方法

private:
    std::string apiKey;
    std::string host;
    std::string lang;
};
