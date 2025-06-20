#pragma once
#include <string>

// 创建强类型的加载器样式枚举，展示每种样式在终端中的视觉效果
enum class LoaderStyle {
    BAR,    // ▓▓▓░░░░░░
    SPINNER // / - \ |
};

// 显示进度条动画
void showLoadingBar(
    const std::string& taskName,
    int steps = 10,
    int delayPerStepMs = 200,
    const std::string& color = "\033[36m" // 青色
);

// 显示旋转动画
void showSpinner(
    const std::string& taskName,
    int durationMs = 3000,
    int frameDelayMs = 100,
    const std::string& color = "\033[33m" // 黄色
);