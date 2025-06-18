#include "cli_application.h"
#include "dispatch_model/cli_dispatch_controller.h"
#include "display/cli_menu_controller.h"
#include <iostream>

#include "common/cli_context.h"

CliApplication::CliApplication()
    : ctx("configUser.json", "configKey.json") {
    if (!i18n.load("zh") || !i18n.load(ctx.user().getLanguage())) {
        std::cerr << "❌ 初始化失败，语言文件加载错误。\n";
        std::exit(1);
    }
}

void CliApplication::run(int argc, char* argv[]) {
    CliContext cliCtx(ctx, i18n, argc > 1 ? CliMode::Dispatcher : CliMode::Interactive);

    if (cliCtx.mode == CliMode::Dispatcher) {
        CliDispatchController controller(cliCtx);
        controller.handle(argc, argv);
    } else {
        CliMenuController controller(cliCtx);
        controller.run();
    }
}
