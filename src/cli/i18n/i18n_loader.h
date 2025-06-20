#pragma once
#include <string>
#include <map>
#include <vector>

class I18n
{
public:
    // 加载指定语言的翻译资源
    bool load(const std::string& language);
    // 根据键获取翻译文本
    std::string tr(const std::string& key) const;
    // 根据区域和键获取翻译文本
    std::string tr(const std::string& section, const std::string& key) const;
    // 获取一组相关翻译文本
    std::vector<std::string> trList(const std::string& section, const std::string& key);

private:
    // 翻译存储
    std::map<std::string, std::string> translations;
};
