// cli_command_dispatcher.h
#pragma once

#include "config_context.h"
#include "i18n_loader.h"

class CliCommandDispatcher {
public:
    CliCommandDispatcher(ConfigContext& ctx, I18n& i18n);
    void handle(int argc, char* argv[]);

private:
    ConfigContext& configContext;
    I18n& i18n;
    void showCommandHelp();
};
