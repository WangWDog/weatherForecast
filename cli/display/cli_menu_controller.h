
#pragma once

#include "config_context.h"
#include "../i18n/i18n_loader.h"
#include "common/cli_context.h"

class CliMenuController {
public:
    void run();
    CliMenuController(CliContext& cliContext)
    : cliContext(cliContext)
    {
    }

private:
    CliContext& cliContext;
};

