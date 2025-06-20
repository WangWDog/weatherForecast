// date_utils.cpp
#include "date_utils.h"
// #include "Config_Key.h"  // 包含 ConfigKey 类
#include "i18n_loader.h"  // 正确的相对路径
#include <string>
#include <codecvt>
#include <locale>

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

// 计算UTF-8字符串在终端中的视觉宽度
// 参数：utf8Str - UTF-8编码的字符串
// 返回值：终端中显示的视觉宽度（英文字符宽度为1，CJK字符和emoji宽度为2）
size_t visualLength(const std::string &utf8Str) {
    // 创建UTF-8到UTF-16/32的转换器（取决于wchar_t的大小）
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;

    // 将UTF-8字符串转换为宽字符串（支持Unicode）
    std::wstring wide = conv.from_bytes(utf8Str);

    size_t width = 0;  // 初始化视觉宽度计数器

    // 遍历宽字符串中的每个字符
    for (wchar_t wc : wide) {
        // 检查字符是否属于CJK统一汉字（中文、日文、韩文汉字）
        if ((wc >= 0x4E00 && wc <= 0x9FFF) ||
            // 检查是否在emoji范围内（Unicode 13.0标准）
            (wc >= 0x1F300 && wc <= 0x1FAD6) ||
            // 检查是否在CJK部首补充范围内
            (wc >= 0x2E80 && wc <= 0x2EFF)) {
            width += 2;  // CJK字符和emoji计为2个宽度
            } else {
                width += 1;  // 其他字符（拉丁字母、数字等）计为1个宽度
            }
    }
    return width;  // 返回总视觉宽度
}

// 在字符串右侧填充空格，使其达到指定视觉宽度
// 参数：str - 原始字符串，targetLen - 目标视觉宽度
// 返回值：填充后的字符串
std::string padRight(const std::string &str, size_t targetLen) {
    // 计算当前字符串的视觉宽度
    size_t visualLen = visualLength(str);

    // 如果已达到或超过目标宽度，直接返回原字符串
    if (visualLen >= targetLen) return str;

    // 计算需要填充的空格数量
    // 注意：每个空格宽度为1，所以直接填充(targetLen - visualLen)个空格
    return str + std::string(targetLen - visualLen, ' ');
}

// 将文本居中显示在指定宽度的空间中

std::string centerText(const std::string &text, size_t width) {
    // 计算文本的视觉宽度
    size_t len = visualLength(text);

    // 如果文本宽度超过目标宽度，截断文本（简单实现）
    if (len >= width) return text.substr(0, width);

    // 计算需要填充的总空格数
    size_t padding = width - len;

    // 左侧填充一半空格，右侧填充剩余空格
    // 注意：当总空格数为奇数时，右侧会比左侧多一个空格
    return std::string(padding / 2, ' ')  // 左侧空格
         + text                          // 原始文本
         + std::string(padding - padding / 2, ' ');  // 右侧空格
}