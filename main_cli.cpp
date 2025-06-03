//
// Created by 13033 on 2025/6/2.
//

#include <iostream>
#include <string>
 #ifdef _WIN32
    #include <windows.h> // Windows 专用设置
#endif
#include "ip_locator.h"
// #include "main_cli.h"


#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"
#define BLINK   "\033[5m"

int main() {
    // std::string city;
    // std::cout << "请输入城市名：";
    // std::cin >> city;

    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // Windows 专用设置
    #endif
    const char* emoji = "😊";
    std::cout << "😊 Hello, 中文 Emoji! 🚀"<< std::endl;
    std::cout << BLUE << BLINK << "蓝色闪烁" << RESET << std::endl;
    std::cout <<emoji<< std::endl;
    // Location auto_location = getLocationByIP();
    // std::cout << auto_location.city;
    while (true);
}