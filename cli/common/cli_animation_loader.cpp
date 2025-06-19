#include "cli_animation_loader.h"
#include "delay.h"
#include <iostream>
#include <thread>

// 显示加载进度条函数
// 参数：任务名称, 总步数, 每步延时(毫秒), ANSI颜色代码
void showLoadingBar(const std::string& taskName, int steps, int delayPerStepMs, const std::string& color) {
    const int barWidth = 10; // 进度条可视宽度（10个字符）

    for (int i = 0; i <= steps; ++i) {
        // 计算当前进度百分比 (0.0~1.0)
        float progress = static_cast<float>(i) / steps;
        // 计算进度条填充位置
        int pos = static_cast<int>(barWidth * progress);

        // 开始输出进度条
        std::cout << "\r" << color << taskName << "... ▓";
            // 构建进度条主体 (barWidth-1 因为起始字符已输出)
            for (int j = 1; j < barWidth; ++j) {
            if (j < pos)
                std::cout << "▓";
            else if (j == pos)
                std::cout << "▒";
            else
                std::cout << "░";
        }

        // 输出百分比和颜色重置
        std::cout << " " << static_cast<int>(progress * 100) << "%" << "\033[0m" << std::flush;  // 立即刷新输出缓冲区

        delay_ms(delayPerStepMs);
    }

    // 加载完成提示
    std::cout << "\n" << color << " 加载完成！\033[0m" << std::endl;
}

// 显示旋转动画函数
// 参数：任务名称, 总持续时间(毫秒), 帧间隔(毫秒), ANSI颜色代码
void showSpinner(const std::string& taskName, int durationMs, int frameDelayMs, const std::string& color) {
    const char frames[] = {'|', '/', '-', '\\'};  // 旋转动画的四个帧
    const int frameCount = sizeof(frames) / sizeof(frames[0]);  // 计算帧数
    int steps = durationMs / frameDelayMs;  // 计算总帧数

    for (int i = 0; i < steps; ++i) {  // 遍历每一帧
        char frame = frames[i % frameCount];  // 循环选择动画帧
        std::cout << "\r" << color << taskName << "... " << frame << "\033[0m" << std::flush;
        delay_ms(frameDelayMs);
    }
    // 完成提示（覆盖最后旋转帧）
    std::cout << "\r" << color << taskName << "完成！ ✅\033[0m" << std::endl;
}
