#pragma once

#include <string>

#include "config_context.h"
#include "../config/config_key.h"
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

std::string callLunarApi(std::string apikey);
LunarData parseLunarJson(const std::string& jsonStr);
std::string formatLunarInfo(const LunarData& d, I18n& i18n);

void printLunarData(const LunarData& d, const std::string& lang, const ConfigKey& configKey, const I18n& i18n);
