#pragma once
#include <string>

// 前向声明，避免头文件耦合太深
class ConfigKey;
class I18n;

// 农历/节气/属相信息获取
std::string getLunarInfo(const ConfigKey& configKey, const std::string& language, I18n& i18n);
std::string getSolarTerm(I18n& i18n);
std::string getZodiacInfo(I18n& i18n);

// 文本排版相关（用于居中与对齐输出）
size_t visualLength(const std::string &str);
std::string padRight(const std::string &str, size_t targetLen);
std::string centerText(const std::string& text, size_t width);
