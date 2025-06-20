#pragma once
#include <string>
#include "../config/config_key.h"
#include "lunar_api.h"
#include "../core/weather_manager.h"

// 翻译函数声明（传入文本和 configKey）
std::string translateWithDoubao(const std::string& text, const std::string& targetLang, const ConfigKey& configKey);
void translateLunarDataIfEnglish(LunarData& data, const std::string& lang, const ConfigKey& configKey);
