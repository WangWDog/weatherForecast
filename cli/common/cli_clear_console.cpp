#include <cstdlib>

//清空控制台屏幕
void clearConsole() {
#ifdef _WIN32
    std::system("cls");  // Windows
#else
    std::system("clear"); // Linux/macOS
#endif
}