#pragma once

#include "common/cli_context.h"

class CliDispatchController {
public:
    CliDispatchController(CliContext& cliContext)
        : cliContext(cliContext) {}
    // 主命令分发处理函数
    void handle(int argc, char* argv[]);

    // 显示帮助信息
    void showCommandHelp();

private:
    CliContext& cliContext;
};