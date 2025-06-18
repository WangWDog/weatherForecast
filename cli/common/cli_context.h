#pragma once

#include "../config/config_context.h"
#include "../i18n/i18n_loader.h"

enum class CliMode {
    Interactive,
    Dispatcher
};

struct CliContext {
    ConfigContext& config;
    I18n& i18n;
    CliMode mode;

    CliContext(ConfigContext& config, I18n& i18n, CliMode mode = CliMode::Interactive)
        : config(config), i18n(i18n), mode(mode) {}
};