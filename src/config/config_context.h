#pragma once

#include "config_user.h"
#include "config_key.h"

class ConfigContext {
public:
    ConfigContext(const std::string& userPath = "configs/configUser.json",
                  const std::string& keyPath  = "configs/configKey.json");

    ConfigUser& user();
    ConfigKey& key();

private:
    ConfigUser configUser;
    ConfigKey configKey;
};
