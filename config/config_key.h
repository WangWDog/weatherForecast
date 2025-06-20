#pragma once
#include "base_config.h"
#include <string>
#include <../utils/json.hpp>

class ConfigKey : public BaseFile {
public:
    explicit ConfigKey(const std::string& path = "configKey.json");

    std::string getHFApiKey() const;
    std::string getFreeApiKey() const;
    std::string getHFHost() const;
    std::string getDoubaoEndpoint() const;
    std::string getDoubaoKey() const;
    std::string getQWeatherKey() const;
    std::string getQWeatherHost() const;

    void setApiKey(const std::string& key);
    void setHost(const std::string& host);

private:
    nlohmann::json configData;  // 🔥 必须有这行！
};
