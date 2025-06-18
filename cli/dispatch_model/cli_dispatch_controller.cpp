#include "cli_dispatch_controller.h"
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>

#include "common/cli_context.h"
#include "common/delay.h"
#include "display/weather_display/cli_weather_display.h"
#include "display/life_index/cli_life_index.h"
#include "display/date_display/cli_date_display.h"
#include "display/update_city/cli_update_city.h"
#include "display/user_setting/cli_user_settings.h"



void CliDispatchController::showCommandHelp() {
    std::cout << "Available commands:\n";
    std::cout << "  show_date        - Show current date and time\n";
    std::cout << "  show_date --all  - Show current time, date, and the Chinese Almanac\n";
    std::cout << "  show_forecast    - Show 7-day weather forecast\n";
    std::cout << "  show_life        - Show life indices for the city\n";
    std::cout << "  update_city      - Update the city setting\n";
    std::cout << "  update_settings  - Update user settings\n";
    std::cout << "  exit             - Exit the application\n";
}

void CliDispatchController::handle(int argc, char* argv[]) {
    if (argc < 2) {
        showCommandHelp();
        return;
    }

    std::string command = argv[1];

    if (command == "show_date") {
        bool showAll = false;
        if (argc > 2) {
            std::string type = argv[2];
            if (type.find("--all") != std::string::npos) {
                showAll = true;
            }
        }
        showCurrentDate(cliContext, showAll);

    } else if (command == "show_forecast") {
        showWeatherForecast(cliContext.config, cliContext.i18n);

    } else if (command == "show_life") {
        showLifeIndices(cliContext.config, cliContext.i18n);

    } else if (command == "update_city") {
        showCityChoose(cliContext.config, cliContext.i18n);

    } else if (command == "update_settings") {
        updateUserSettings(cliContext.config.user(), cliContext.i18n);

    } else if (command == "exit") {
        std::cout << cliContext.i18n.tr("main_cli", "goodbye") << std::endl;
        delay_ms(2000);
        exit(0);

    } else {
        std::cout << "❌ Invalid command! Type 'help' for a list of commands." << std::endl;
    }
}
