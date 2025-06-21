#include "cli_interface.h"
#include "cli_application.h"

int run_cli(int argc, char* argv[]) {
    CliApplication app;
    app.run(argc, argv);
    return 0;
}
