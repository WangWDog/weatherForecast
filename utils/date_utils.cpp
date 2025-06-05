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
