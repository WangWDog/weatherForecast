//
// Created by 13033 on 2025/6/16.
//

#include "cli_ai_advisor.h"

#include <iostream>

#include "doubao_helper.h"
#include "weather_manager.h"
#include "../common/cli_clear_console.h"
#include "common/cli_context.h"
#include "context_builder/buildAISuggestionContent.h"

void showAISuggestions(CliContext& ctx) {
    clearConsole();
    std::cout << "\t🌟 " << ctx.i18n.tr("ai_suggestion", "getting") << "\n";
    std::string suggestion = buildAISuggestionContent(ctx.config);
    clearConsole();
    std::cout << "\n🤖 " << suggestion << "\n";

    // 仅在交互模式下提示返回
    if (ctx.mode == CliMode::Interactive) {
        std::cout << ctx.i18n.tr("ai_suggestion", "back_hint") << std::flush;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}
