#pragma once

#include "CacheManager.h"
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
    CacheManager& cache;

    CliContext(ConfigContext& config, I18n& i18n, CacheManager& cache,CliMode mode = CliMode::Interactive)
        : config(config), i18n(i18n),cache(cache), mode(mode) {}
};