#pragma once
#include <string>
#include <vector>

struct Forecast {
    std::string date;
    std::string text;
    std::string tempMax;
    std::string tempMin;
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
    explicit WeatherManager(std::string apiKey,std::string host);
    std::vector<Forecast> get7DayForecast(const std::string& locationId);
    std::vector<CityResult> searchCity(const std::string& keyword);  // <-- 新增
private:
    std::string apiKey;
    std::string host;
};