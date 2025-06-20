#include "cli_animation_loader.h"
#include "delay.h"
#include <iostream>
#include <thread>

void showLoadingBar(const std::string& taskName, int steps, int delayPerStepMs, const std::string& color) {
    const int barWidth = 10;

    for (int i = 0; i <= steps; ++i) {
        float progress = static_cast<float>(i) / steps;
        int pos = static_cast<int>(barWidth * progress);

        std::cout << "\r" << color << taskName << "... ▓";
        for (int j = 1; j < barWidth; ++j) {
            if (j < pos)
                std::cout << "▓";
            else if (j == pos)
                std::cout << "▒";
            else
                std::cout << "░";
        }
        std::cout << " " << static_cast<int>(progress * 100) << "%" << "\033[0m" << std::flush;

        delay_ms(delayPerStepMs);
    }

    std::cout << "\n" << color << " 加载完成！\033[0m" << std::endl;
}

void showSpinner(const std::string& taskName, int durationMs, int frameDelayMs, const std::string& color) {
    const char frames[] = {'|', '/', '-', '\\'};
    const int frameCount = sizeof(frames) / sizeof(frames[0]);
    int steps = durationMs / frameDelayMs;

    for (int i = 0; i < steps; ++i) {
        char frame = frames[i % frameCount];
        std::cout << "\r" << color << taskName << "... " << frame << "\033[0m" << std::flush;
        delay_ms(frameDelayMs);
    }
    std::cout << "\r" << color << taskName << "完成！ ✅\033[0m" << std::endl;
}
