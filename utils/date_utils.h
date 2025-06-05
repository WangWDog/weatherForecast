// date_utils.h
#pragma once
#include <string>


class ConfigKey;
class I18n;

std::string getLunarInfo(const ConfigKey& configKey, const std::string& language, I18n& i18n);
std::string getSolarTerm(I18n& i18n);
std::string getZodiacInfo(I18n& i18n);
