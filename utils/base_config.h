#pragma once
#include <string>
#include <json.hpp>

class BaseConfig {
public:
    explicit BaseConfig(const std::string& path);
    virtual ~BaseConfig() = default;

    virtual bool load();
    virtual bool save();

protected:
    std::string configPath;
    nlohmann::json configJson;
};