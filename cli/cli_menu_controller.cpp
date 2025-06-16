#include "cli_menu_controller.h"

#include <iostream>

#include "cli_animation_loader.h"
#include "displayUtils/cli_clear_console.h"
#include "cli_ai_advisor.h"
#include "delay.h"
#include "updateCity.h"
#include "doubao_translator.h"
#include "CacheManager.h"

// 外部功能声明（由其他模块实现）
void showAISuggestions(ConfigContext&, I18n&);
void showCurrentDate(ConfigContext&, I18n&, bool);
void showWeatherForecast(ConfigContext&, I18n&);
void showLifeIndices(ConfigContext&, I18n&);
void updateUserSettings(ConfigUser&, I18n&);

    CliMenuController::CliMenuController(ConfigContext& configContext, I18n& translator)
        : configContext(configContext), i18n(translator) {}

    void CliMenuController::run() {
        showLoadingBar(i18n.tr("main_cli", "menu_loading"), 8, 40, "\033[38;5;117m");
        while (true) {
            clearConsole();

            std::cout << "\n" << i18n.tr("main_cli", "menu_title") << "\n";
            std::cout << "------------------------\n";
            auto options = i18n.trList("main_cli", "menu_options");
            for (size_t i = 0; i < options.size(); ++i) {
                std::cout << i << ". " << options[i] << "\n";
            }
            std::cout << "------------------------\n";
            std::cout << i18n.tr("main_cli", "prompt_input") << std::flush;

            std::string command;
            std::getline(std::cin, command);
            clearConsole();

            if (command == "0") {
                showAISuggestions(configContext, i18n);
            } else if (command == "1") {
                showCurrentDate(configContext,i18n, true);
            } else if (command == "2") {
                std::cout << i18n.tr("weather_view", "forecast_title") << "\n";
                showWeatherForecast(configContext, i18n);
            } else if (command == "3") {
                std::cout << i18n.tr("life_index", "title") << "\n";
                showLifeIndices(configContext, i18n);
            } else if (command == "4") {
                std::cout << i18n.tr("city_update", "title") << "\n";
                updateCity(configContext, i18n);
                delay_ms(2000);
            } else if (command == "5") {
                std::cout << i18n.tr("settings", "update_title") << "\n";
                updateUserSettings(configContext.user(), i18n);
            } else if (command == "6") {
                std::cout << i18n.tr("main_cli", "goodbye") << std::endl;
                delay_ms(5000);
                break;
            } else {
                std::cout << i18n.tr("main_cli", "invalid_option") << std::endl;
            }
        }
    }