#pragma once

#ifdef _WIN32
#include <conio.h>
inline char cli_getch() {
    return _getch();
}
#else
#include <termios.h>
#include <unistd.h>
inline char cli_getch() {
    struct termios oldt{}, newt{};
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    char ch;
    ssize_t n = read(STDIN_FILENO, &ch, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return n == 1 ? ch : '\n';
}
#endif
