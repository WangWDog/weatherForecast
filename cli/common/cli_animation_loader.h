#pragma once
#include <string>

enum class LoaderStyle {
    BAR,    // ▓▓▓░░░░░░
    SPINNER // / - \ |
};

void showLoadingBar(
    const std::string& taskName,
    int steps = 10,
    int delayPerStepMs = 200,
    const std::string& color = "\033[36m" // 青色
);

void showSpinner(
    const std::string& taskName,
    int durationMs = 3000,
    int frameDelayMs = 100,
    const std::string& color = "\033[33m" // 黄色
);