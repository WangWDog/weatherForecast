#pragma once
#include <string>
#include <map>
#include <vector>

class I18n
{
public:
    bool load(const std::string& language);
    std::string tr(const std::string& key);
    std::string tr(const std::string& section, const std::string& key);
    std::vector<std::string> trList(const std::string& section, const std::string& key);

private:
    std::map<std::string, std::string> translations;
};
