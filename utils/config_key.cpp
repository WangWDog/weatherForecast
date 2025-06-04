#include "config_key.h"

ConfigKey::ConfigKey(const std::string& path) : BaseConfig(path) {}

std::string ConfigKey::getHFApiKey() const {
    return configJson.value("key_hf", "");
}
std::string ConfigKey::getFreeApiKey() const {
    return configJson.value("key_free", "");
}

void ConfigKey::setApiKey(const std::string& key) {
    configJson["key"] = key;
}

std::string ConfigKey::getHFHost() const {
    return configJson.value("host_hf", "");
}

void ConfigKey::setHost(const std::string& host) {
    configJson["host"] = host;
}