#include "cli_application.h"
#include "dispatch_model/cli_dispatch_controller.h"
#include "display/cli_menu_controller.h"
#include <iostream>

#include "common/cli_context.h"

CliApplication::CliApplication()
    : ctx("configs/configUser.json", "configs/configKey.json") {
    const std::string lang = ctx.user().getLanguage();
    if (lang != "zh" && i18n.load(lang)) {
        std::cout << "✅ 已加载语言: " << lang << std::endl;
    } else if (i18n.load("zh")) {
        std::cout << "✅ 已回退至默认语言 zh" << std::endl;
    } else {
        std::cerr << "❌ 初始化失败，语言文件加载错误。\n";
        std::exit(1);
    }

}


void CliApplication::run(int argc, char* argv[]) {
    CacheManager cache = CacheManager("configs/cache.json");
    CliContext cliCtx(ctx, i18n,cache, argc > 1 ? CliMode::Dispatcher : CliMode::Interactive);

    if (cliCtx.mode == CliMode::Dispatcher) {
        CliDispatchController controller(cliCtx);
        controller.handle(argc, argv);
    } else {
        CliMenuController controller(cliCtx);
        controller.run();
    }
}
