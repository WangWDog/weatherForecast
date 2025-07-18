cmake_minimum_required(VERSION 3.21)
# 要求 CMake 的最低版本为 3.21，确保支持 Qt6 等特性
project(weatherForecast)
# 设置当前项目的名称为 WeatherApp

set(CMAKE_CXX_STANDARD 20)
# 设置 C++ 标准为 C++17

set(CURL_PATH "F:\\CPlusDependences\\LibCurl\\V8")

# 源码根目录
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

# 包含头文件路径
include_directories(
        ${CURL_PATH}/include
        ${SRC_ROOT}/core            # 天气核心模块路径
        ${SRC_ROOT}/gui             # GUI 模块路径
        ${SRC_ROOT}/utils           # 工具模块路径
        ${SRC_ROOT}/cli             # 命令行模块路径
        ${SRC_ROOT}/cli/i18n        # 国际化模块路径
        ${SRC_ROOT}/config
)
#引入CurlLib和bin
link_directories(${CURL_PATH}/lib)
# 源码文件（CLI 用）
set(CLI_SOURCES
        ${SRC_ROOT}/cli/common/delay.cpp
        ${SRC_ROOT}/cli/common/cli_animation_loader.cpp
        ${SRC_ROOT}/cli/common/cli_clear_console.cpp
        ${SRC_ROOT}/main_cli.cpp
        ${SRC_ROOT}/config/config_user.cpp
        ${SRC_ROOT}/config/config_key.cpp
        ${SRC_ROOT}/config/base_config.cpp
        ${SRC_ROOT}/cli/i18n/i18n_loader.cpp
        ${SRC_ROOT}/cli/i18n/i18n_loader.h
        ${SRC_ROOT}/utils/lunar_api.h
        ${SRC_ROOT}/utils/lunar_api.cpp
        ${SRC_ROOT}/utils/doubao_translator.cpp
        ${SRC_ROOT}/utils/date_utils.cpp
        ${SRC_ROOT}/utils/doubao_helper.cpp
        ${SRC_ROOT}/core/CacheManager.cpp
        ${SRC_ROOT}/cli/display/cli_menu_controller.cpp
        ${SRC_ROOT}/cli/display/cli_menu_controller.h
        ${SRC_ROOT}/config/config_context.cpp
        ${SRC_ROOT}/config/config_context.h
        ${SRC_ROOT}/cli/cli_application.cpp
        ${SRC_ROOT}/cli/cli_application.h
        ${SRC_ROOT}/cli/display/ai_advisor/cli_ai_advisor.cpp
        ${SRC_ROOT}/cli/display/ai_advisor/cli_ai_advisor.h
        ${SRC_ROOT}/cli/display/date_display/cli_date_display.cpp
        ${SRC_ROOT}/cli/display/date_display/cli_date_display.h
        ${SRC_ROOT}/cli/display/life_index/cli_life_index.cpp
        ${SRC_ROOT}/cli/display/life_index/cli_life_index.h
        ${SRC_ROOT}/cli/display/user_setting/cli_user_settings.cpp
        ${SRC_ROOT}/cli/display/user_setting/cli_user_settings.h
        ${SRC_ROOT}/core/doubao_manager.cpp
        ${SRC_ROOT}/core/doubao_manager.h
        ${SRC_ROOT}/cli/display/weather_display/cli_weather_display.cpp
        ${SRC_ROOT}/cli/display/weather_display/cli_weather_display.h

        ${SRC_ROOT}/cli/dispatch_model/cli_dispatch_controller.cpp
        ${SRC_ROOT}/cli/dispatch_model/cli_dispatch_controller.h
        ${SRC_ROOT}/cli/common/cli_context.cpp
        ${SRC_ROOT}/cli/common/cli_context.h

)
set(WEATHER_GLOBAL
        ${SRC_ROOT}/core/weather_manager.cpp          # 天气核心实现
        ${SRC_ROOT}/cli/display/update_city/cli_update_city.cpp
        ${SRC_ROOT}/cli/display/update_city/cli_update_city.h
        ${SRC_ROOT}/cli/context_builder/buildAISuggestionContent.cpp
        ${SRC_ROOT}/cli/context_builder/buildAISuggestionContent.h
        ${SRC_ROOT}/utils/ip_locator.cpp
        ${SRC_ROOT}/config/config_user.cpp
        ${SRC_ROOT}/config/config_user.h
        ${SRC_ROOT}/config/config_key.cpp
        ${SRC_ROOT}/config/config_key.h
        ${SRC_ROOT}/config/base_config.cpp
        ${SRC_ROOT}/config/base_config.h
        ${SRC_ROOT}/utils/http_client.cpp
        ${SRC_ROOT}/utils/http_client.h
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
file(COPY ${CMAKE_SOURCE_DIR}/configs/configUser.json
        DESTINATION ${CMAKE_BINARY_DIR}/configs)
#缓存文件
file(COPY ${CMAKE_SOURCE_DIR}/configs/cache.json
        DESTINATION ${CMAKE_BINARY_DIR}/configs)
#apiKey文件
file(COPY ${CMAKE_SOURCE_DIR}/configs/configKey.json
        DESTINATION ${CMAKE_BINARY_DIR}/configs)

#i18n语言文件
file(COPY ${SRC_ROOT}/cli/i18n/
        DESTINATION ${CMAKE_BINARY_DIR}/i18n)