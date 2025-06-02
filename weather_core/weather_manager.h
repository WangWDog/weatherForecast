#pragma once
#include <string>

class WeatherManager {
public:
    WeatherManager();
    void setCity(const std::string &cityName);
    void updateWeather();
    void printWeather();
    std::string getWeatherText() const;

private:
    std::string city;
    std::string weatherText;
};
