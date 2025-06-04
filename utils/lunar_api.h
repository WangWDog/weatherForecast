#pragma once
#include <string>
#include "config_key.h"

struct LunarData {
    std::string solarDate;
    std::string week;
    std::string constellation;
    std::string festivals;
    std::string lunarYear;
    std::string lunar;
    std::string thisYear;
    std::string ganzhiYear;
    std::string jieqi;
    std::string yi;
    std::string ji;
    std::string weiyuShort;
    std::string weiyuLong;
};

std::string callLunarApi(ConfigKey& config_key);
LunarData parseLunarJson(const std::string& jsonStr);
void printLunarData(const LunarData& d);
