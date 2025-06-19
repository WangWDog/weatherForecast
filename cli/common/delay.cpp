//使当前线程暂停执行一段指定的时间

#include "delay.h"
#include <thread>
#include <chrono>

void delay_ms(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}