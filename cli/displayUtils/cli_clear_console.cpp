#include <cstdlib>

void clearConsole() {
#ifdef _WIN32
    std::system("cls");  // Windows
#else
    std::system("clear"); // Linux/macOS
#endif
}