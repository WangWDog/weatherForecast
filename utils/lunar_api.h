#pragma once

#include <string>
#include "config_key.h"
#include "i18n_loader.h"

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

std::string callLunarApi(ConfigKey& config_key, const std::string& lang);
LunarData parseLunarJson(const std::string& jsonStr);
std::string formatLunarInfo(const LunarData& d, I18n& i18n);
std::string getLunarInfo(ConfigKey& config_key, const std::string& lang, I18n& i18n);

void printLunarData(const LunarData& d);
