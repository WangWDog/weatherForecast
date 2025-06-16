
#pragma once

#include "../config/config_user.h"
#include "../config/config_key.h"
#include "i18n/i18n_loader.h"

class CliMenuController {
public:
    CliMenuController(ConfigUser& user, ConfigKey& key, I18n& i18n);
    void run();

private:
    ConfigUser& configUser;
    ConfigKey& configKey;
    I18n& i18n;
};

