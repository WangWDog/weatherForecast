#pragma once
#include "base_config.h"

class ConfigKey : public BaseConfig {
public:
    explicit ConfigKey(const std::string& path = "configKey.json");

    std::string getHFApiKey() const;
    void setApiKey(const std::string& key);

    std::string getFreeApiKey() const;
    std::string getHFHost() const;
    void setHost(const std::string& host);
};