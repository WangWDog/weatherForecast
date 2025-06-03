#pragma once
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

class WeatherManager {
public:
    explicit WeatherManager(std::string key, std::string host, std::string lang = "zh");
    std::vector<DailyForecast> get7DayForecast(const std::string& locationId);
    std::vector<CityResult> searchCity(const std::string& keyword);  // <-- 新增
private:
    std::string apiKey;
    std::string host;
    std::string lang;
};