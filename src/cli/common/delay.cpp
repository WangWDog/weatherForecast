//
// Created by 13033 on 2025/6/3.
//

#include "delay.h"
#include <thread>
#include <chrono>

void delay_ms(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}