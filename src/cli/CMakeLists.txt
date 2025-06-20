set(CLI_SOURCES
    cli_application.cpp
    common/cli_animation_loader.cpp
    common/cli_clear_console.cpp
    common/cli_context.cpp
    common/date_utils.cpp
    common/doubao_translator.cpp
    common/lunar_api.cpp
    common/delay.cpp
    context_builder/buildAISuggestionContent.cpp
    dispatch_model/cli_dispatch_controller.cpp
    display/ai_advisor/cli_ai_advisor.cpp
    display/cli_menu_controller.cpp
    display/date_display/cli_date_display.cpp
    display/life_index/cli_life_index.cpp
    display/update_city/cli_update_city.cpp
    display/user_setting/cli_user_settings.cpp
    display/weather_display/cli_weather_display.cpp
    i18n/i18n_loader.cpp
)

add_library(cli STATIC ${CLI_SOURCES})

# Include directories from cli and dependencies


# We'll set include directories for CLI


target_include_directories(cli PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/i18n
    ${CMAKE_CURRENT_SOURCE_DIR}/../utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../config
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
)

target_link_libraries(cli PUBLIC core utils config)
