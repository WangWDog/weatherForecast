// cli_application.cpp
#include "cli_application.h"
#include "menus_model/cli_menu_controller.h"
// #include "cli_command_dispatcher.h"
#include <iostream>

#include "dispatch_model/cli_menu_dispatch_controller.h"

CliApplication::CliApplication()
    : ctx("configUser.json", "configKey.json") {  // 初始化配置上下文（用户配置和API密钥配置）
    // 尝试加载语言文件
    if (!i18n.load("zh") || !i18n.load(ctx.user().getLanguage())) {  // 先加载中文，再加载用户配置的语言
        std::cerr << "❌ 初始化失败，语言文件加载错误。\n";  // 显示错误信息
        std::exit(1);  // 退出程序（错误代码1）
    }
}

// 主运行函数：根据参数选择运行模式
void CliApplication::run(int argc, char* argv[]) {
    // 检查命令行参数数量
    if (argc > 1) {  // 如果有额外参数
        // 命令行模式（未来实现）
        CliCommandDispatcher controller(ctx, i18n);  // 创建命令分发器
        controller.handle(argc, argv);               // 处理命令行参数
    } else {
        // 交互式菜单模式
        CliMenuController controller(ctx, i18n);  // 创建菜单控制器
        controller.run();                         // 运行菜单系统
    }
}
