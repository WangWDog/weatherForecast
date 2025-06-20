
#include "cli_application.h"
#include "doubao_manager.h"

int main(int argc, char* argv[]) {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
    CliApplication app;
    app.run(argc, argv);
    return 0;
}
