// cli_application.h
#pragma once

#include "i18n/i18n_loader.h"
#include "config_context.h"

class CliApplication {
public:
    CliApplication();
    void run(int argc, char* argv[]);

private:
    ConfigContext ctx;
    I18n i18n;
};
