
#pragma once

#include "config_context.h"
#include "../config/config_user.h"
#include "../config/config_key.h"
#include "i18n/i18n_loader.h"

class CliMenuController {
public:
    CliMenuController(ConfigContext& config_context, I18n& i18n);
    void run();

private:
    ConfigContext& configContext;
    I18n& i18n;
};

