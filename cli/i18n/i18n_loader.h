#pragma once
#include <string>
#include <map>
#include <vector>

class I18n
{
public:
    bool load(const std::string& language);
    std::string tr(const std::string& key) const;
    std::string tr(const std::string& section, const std::string& key) const;
    std::vector<std::string> trList(const std::string& section, const std::string& key);

private:
    std::map<std::string, std::string> translations;
};
