// cli_command_dispatcher.h
#pragma once

#include "config_context.h"
#include "i18n_loader.h"

class CliCommandDispatcher {
public:
    //使用初始化列表在实现中初始化成员变量
    CliCommandDispatcher(ConfigContext& ctx, I18n& i18n);
    void handle(int argc, char* argv[]);

private:
    //存储和访问应用程序配置信息
    ConfigContext& configContext;
    I18n& i18n;
    //显示命令帮助信息
    void showCommandHelp();
};
