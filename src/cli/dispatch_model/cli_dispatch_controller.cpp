#include "cli_dispatch_controller.h"
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>

#include "common/cli_clear_console.h"
#include "common/cli_context.h"
#include "common/delay.h"
#include "display/cli_menu_controller.h"
#include "display/ai_advisor/cli_ai_advisor.h"
#include "display/weather_display/cli_weather_display.h"
#include "display/life_index/cli_life_index.h"
#include "display/date_display/cli_date_display.h"
#include "display/update_city/cli_update_city.h"
#include "display/user_setting/cli_user_settings.h"


void CliDispatchController::showCommandHelp()
{
    std::cout << "Available commands:\n";
    std::cout << "  --AI               - show AI suggestions based on weather info";
    std::cout << "  --show_date        - Show current date and time\n";
    std::cout << " --show_date --all  - Show current time, date, and the Chinese Almanac\n";
    std::cout << " --show_forecast    - Show 7-day weather forecast\n";
    std::cout << "  --show_forecast --unit=<metric|imperial>\n";
    std::cout << "                      - Override units for this command\n";
    std::cout << "  --unit=<metric|imperial>\n";
    std::cout << "                      - Override units for all commands\n";
    std::cout << "  --show_life        - Show life indices for the city\n";
    std::cout << "  --update_city      - Update the city setting\n";
    std::cout << "  --update_settings  - Update user settings\n";
}

void CliDispatchController::handle(int argc, char* argv[])
{
    std::string unitOverride;
    int writeIndex = 1;
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg.rfind("--unit=", 0) == 0)
        {
            unitOverride = arg.substr(7);
        }
        else
        {
            argv[writeIndex++] = argv[i];
        }
    }
    argc = writeIndex;
    if (!unitOverride.empty())
    {
        cliContext.config.user().setUnits(unitOverride);
    }

    if (argc < 2)
    {
        showCommandHelp();
        return;
    }

    std::string command = argv[1];
    clearConsole();
    if (command == "--show_date")
    {
        bool showAll = false;
        if (argc > 2)
        {
            std::string type = argv[2];
            if (type.find("--all") != std::string::npos)
            {
                showAll = true;
            }
        }
        showCurrentDate(cliContext, showAll);
    }
    else if (command == "--show_forecast")
    {
        std::string unit = cliContext.config.user().getUnits();
        for (int i = 2; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg.rfind("--unit=", 0) == 0)
            {
                unit = arg.substr(7);
            }
        }
        showWeatherForecast(cliContext, unit);
    }
    else if (command == "--help")
    {
        showCommandHelp();
    }
    else if (command == "--show_life")
    {
        showLifeIndices(cliContext);
    }
    else if (command == "--AI")
    {
        showAISuggestions(cliContext);
    }
    else if (command == "--update_city")
    {
        showCityChoose(cliContext);
    }
    else if (command == "--update_settings")
    {
        updateUserSettings(cliContext.config.user(), cliContext.i18n);
    }
    else if (command == "--interactive")
    {
        cliContext.mode = CliMode::Interactive;
        CliMenuController controller(cliContext);
        controller.run();
    }
    else
    {
        std::cout << "❌ Invalid command! Type 'help' for a list of commands." << std::endl;
    }
}
