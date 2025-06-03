#include "config_key.h"

ConfigKey::ConfigKey(const std::string& path) : BaseConfig(path) {}

std::string ConfigKey::getApiKey() const {
    return configJson.value("key", "");
}

void ConfigKey::setApiKey(const std::string& key) {
    configJson["key"] = key;
}

std::string ConfigKey::getHost() const {
    return configJson.value("host", "");
}

void ConfigKey::setHost(const std::string& host) {
    configJson["host"] = host;
}