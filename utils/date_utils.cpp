// date_utils.cpp
#include "date_utils.h"
#include "Config_Key.h"  // 包含 ConfigKey 类
#include "i18n_loader.h"  // 正确的相对路径

// 获取农历信息
std::string getLunarInfo(const ConfigKey& configKey, const std::string& language, I18n& i18n) {
    // 这里假设通过 I18n 对象从配置中获取农历信息
    return i18n.tr("lunar", "lunar_date");  // 返回农历信息
}

// 获取节气信息
std::string getSolarTerm(I18n& i18n) {
    // 假设通过 I18n 获取节气信息
    return i18n.tr("solar_term", "current_term");  // 返回节气名称
}

// 获取生肖信息
std::string getZodiacInfo(I18n& i18n) {
    // 假设通过 I18n 获取生肖信息
    return i18n.tr("zodiac", "current_zodiac");  // 返回生肖名称
}

size_t visualLength(const std::string &str) {
    size_t len = 0;
    for (unsigned char ch : str) {
        len += (ch >= 0x80) ? 2 : 1;  // 中文/emoji 宽度估算
    }
    return len;
}
std::string padRight(const std::string &str, size_t targetLen) {
    size_t visualLen = visualLength(str);
    if (visualLen >= targetLen) return str;
    return str + std::string(targetLen - visualLen, ' ');
}

std::string centerText(const std::string &text, size_t width) {
    size_t len = visualLength(text);
    if (len >= width) return text.substr(0, width);
    size_t padding = width - len;
    return std::string(padding / 2, ' ') + text + std::string(padding - padding / 2, ' ');
}