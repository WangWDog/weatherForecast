#include "cli_menu_controller.h"

#include <iostream>

#include "../common/cli_animation_loader.h"
#include "../common/cli_clear_console.h"
#include "ai_advisor/cli_ai_advisor.h"
#include "../common/delay.h"
#include "weather_display/cli_weather_display.h"
#include "CacheManager.h"
#include "date_display/cli_date_display.h"
#include "life_index/cli_life_index.h"
#include "update_city/cli_update_city.h"
#include "user_setting/cli_user_settings.h"


void CliMenuController::run()
{
    clearConsole();
    showLoadingBar(cliContext.i18n.tr("main_cli", "menu_loading"), 8, 125, "\033[38;5;117m");
    while (true)
    {
        clearConsole();
        std::cout << "\n" << cliContext.i18n.tr("main_cli", "menu_title") << "\n";
        std::cout << "------------------------\n";
        auto options = cliContext.i18n.trList("main_cli", "menu_options");
        for (size_t i = 0; i < options.size(); ++i)
        {
            std::cout << i << ". " << options[i] << "\n";
        }
        std::cout << "------------------------\n";
        std::cout << cliContext.i18n.tr("main_cli", "prompt_input") << std::flush;

        std::string command;
        std::getline(std::cin, command);
        clearConsole();

        if (command == "0")
        {
            showAISuggestions(cliContext);
        }
        else if (command == "1")
        {
            showCurrentDate(cliContext, true);
        }
        else if (command == "2")
        {
            std::cout << cliContext.i18n.tr("weather_view", "forecast_title") << "\n";
            showWeatherForecast(cliContext.config, cliContext.i18n);
        }
        else if (command == "3")
        {
            std::cout << cliContext.i18n.tr("life_index", "title") << "\n";
            showLifeIndices(cliContext.config, cliContext.i18n);
        }
        else if (command == "4")
        {
            std::cout << cliContext.i18n.tr("city_update", "title") << "\n";
            showCityChoose(cliContext.config, cliContext.i18n);
            delay_ms(2000);
        }
        else if (command == "5")
        {
            std::cout << cliContext.i18n.tr("settings", "update_title") << "\n";
            updateUserSettings(cliContext.config.user(), cliContext.i18n);
        }
        else if (command == "6")
        {
            std::cout << cliContext.i18n.tr("main_cli", "goodbye") << std::endl;
            delay_ms(5000);
            break;
        }
        else
        {
            std::cout << cliContext.i18n.tr("main_cli", "invalid_option") << std::endl;
        }
    }
}
