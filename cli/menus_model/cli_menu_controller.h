#pragma once

#include "config_context.h"
#include "../i18n/i18n_loader.h"

class CliMenuController {
    //使用依赖注入模式，不复制对象（引用传递）
    CliMenuController(ConfigContext& config_context, I18n& i18n);
    void run();

private:
    // 存储构造时传入的引用
    ConfigContext& configContext;
    I18n& i18n;
};
