// cli_application.cpp
#include "cli_application.h"
#include "menus_model/cli_menu_controller.h"
// #include "cli_command_dispatcher.h"  // 你之后可以加入
#include <iostream>

#include "dispatch_model/cli_menu_dispatch_controller.h"

CliApplication::CliApplication()
    : ctx("configUser.json", "configKey.json") {
    if (!i18n.load("zh") || !i18n.load(ctx.user().getLanguage())) {
        std::cerr << "❌ 初始化失败，语言文件加载错误。\n";
        std::exit(1);
    }
}

void CliApplication::run(int argc, char* argv[]) {
    if (argc > 1) {
        CliCommandDispatcher controller(ctx, i18n);
        controller.handle(argc,argv);
    } else {
        CliMenuController controller(ctx, i18n);  // 你之前已生成
        controller.run();
    }
}
