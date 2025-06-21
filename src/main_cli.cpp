
#include "cli_interface.h"
#include "doubao_manager.h"

int main(int argc, char* argv[]) {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
    return run_cli(argc, argv);
}
