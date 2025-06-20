cmake_minimum_required(VERSION 3.21)
# 要求 CMake 的最低版本为 3.21，确保支持 Qt6 等特性
project(weatherForecast)
# 设置当前项目的名称为 WeatherApp

set(CMAKE_CXX_STANDARD 20)
# 设置 C++ 标准为 C++17

set(CURL_PATH "F:\\CPlusDependences\\LibCurl\\V8")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

# 包含头文件路径
include_directories(
        ${CURL_PATH}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/core/weather_core  # 天气核心模块路径
        ${CMAKE_CURRENT_SOURCE_DIR}/gui           # GUI 模块路径
        ${CMAKE_CURRENT_SOURCE_DIR}/utils         # 工具模块路径
        ${CMAKE_CURRENT_SOURCE_DIR}/cli           # 命令行模块路径
        ${CMAKE_CURRENT_SOURCE_DIR}/core           # 命令行模块路径
        ${CMAKE_CURRENT_SOURCE_DIR}/cli/i18n       # 命令行模块路径
        ${CMAKE_CURRENT_SOURCE_DIR}/config
)
#引入CurlLib和bin
link_directories(${CURL_PATH}/lib)
# 源码文件（CLI 用）
set(CLI_SOURCES
        cli/common/delay.cpp
        cli/common/cli_animation_loader.cpp
        cli/common/cli_clear_console.cpp
        main_cli.cpp
        config/config_user.cpp
        config/config_key.cpp
        config/base_config.cpp
        cli/i18n/i18n_loader.cpp
        cli/i18n/i18n_loader.h
        utils/lunar_api.h
        utils/lunar_api.cpp
        utils/doubao_translator.cpp
        utils/date_utils.cpp
        utils/doubao_helper.cpp
        core/CacheManager.cpp
        cli/display/cli_menu_controller.cpp
        cli/display/cli_menu_controller.h
        config/config_context.cpp
        config/config_context.h
        cli/cli_application.cpp
        cli/cli_application.h
        cli/display/ai_advisor/cli_ai_advisor.cpp
        cli/display/ai_advisor/cli_ai_advisor.h
        cli/display/date_display/cli_date_display.cpp
        cli/display/date_display/cli_date_display.h
        cli/display/life_index/cli_life_index.cpp
        cli/display/life_index/cli_life_index.h
        cli/display/user_setting/cli_user_settings.cpp
        cli/display/user_setting/cli_user_settings.h
        core/doubao_manager.cpp
        core/doubao_manager.h
        cli/display/weather_display/cli_weather_display.cpp
        cli/display/weather_display/cli_weather_display.h

        cli/dispatch_model/cli_dispatch_controller.cpp
        cli/dispatch_model/cli_dispatch_controller.h
        cli/common/cli_context.cpp
        cli/common/cli_context.h

)
set(WEATHER_GLOBAL
        core/weather_manager.cpp          # 天气核心实现
        cli/display/update_city/cli_update_city.cpp
        cli/display/update_city/cli_update_city.h
        cli/context_builder/buildAISuggestionContent.cpp
        cli/context_builder/buildAISuggestionContent.h
        utils/ip_locator.cpp
        config/config_user.cpp
        config/config_user.h
        config/config_key.cpp
        config/config_key.h
        config/base_config.cpp
        config/base_config.h
)



# CLI 可执行文件
add_executable(weather_cli ${WEATHER_GLOBAL} ${CLI_SOURCES})

# 创建 CLI 可执行程序 weather_cli，包含上面定义的 CLI_SOURCES
#将curl 连接到可执行文件

target_link_libraries(weather_cli PRIVATE curl)

#手动复制.dll 文件到指定目标
if(WIN32)
    set(CURL_DLL_PATH ${CURL_PATH}/bin/libcurl-x64.dll)

    add_custom_command(TARGET weather_cli POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CURL_DLL_PATH}"
            $<TARGET_FILE_DIR:weather_cli>
            COMMENT "Copying libcurl.dll to output directory..."
    )
endif()

#用户配置文件
file(COPY ${CMAKE_SOURCE_DIR}/configUser.json
        DESTINATION ${CMAKE_BINARY_DIR})
#缓存文件
file(COPY ${CMAKE_SOURCE_DIR}/cache.json
        DESTINATION ${CMAKE_BINARY_DIR})
#apiKey文件
file(COPY ${CMAKE_SOURCE_DIR}/configKey.json
        DESTINATION ${CMAKE_BINARY_DIR})

#语言文件
file(COPY ${CMAKE_SOURCE_DIR}/cli/i18n/lang_zh.json
        DESTINATION ${CMAKE_BINARY_DIR}/i18n)
#语言文件
file(COPY ${CMAKE_SOURCE_DIR}/cli/i18n/lang_en.json
        DESTINATION ${CMAKE_BINARY_DIR}/i18n)