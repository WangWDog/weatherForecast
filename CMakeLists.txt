cmake_minimum_required(VERSION 3.21)
# 要求 CMake 的最低版本为 3.21，确保支持 Qt6 等特性
project(weatherForecast)
# 设置当前项目的名称为 WeatherApp

set(CMAKE_CXX_STANDARD 20)
# 设置 C++ 标准为 C++17


# 查找 Qt6 并确保 Widgets 和 Network 模块可用，否则中止编译

set(CURL_PATH "F:\\CPlusDependences\\LibCurl\\V8")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
# 包含头文件路径
include_directories(
        ${CURL_PATH}/include
        core/weather_core  # 天气核心模块路径
        ${CMAKE_CURRENT_SOURCE_DIR}/gui           # GUI 模块路径
        ${CMAKE_CURRENT_SOURCE_DIR}/utils         # 工具模块路径
        ${CMAKE_CURRENT_SOURCE_DIR}/cli           # 命令行模块路径
        ${CMAKE_CURRENT_SOURCE_DIR}/core           # 命令行模块路径
        ${CMAKE_CURRENT_SOURCE_DIR}/cli/i18n       # 命令行模块路径
        ${CMAKE_CURRENT_SOURCE_DIR}/config
)
#引入CurlLib和bin

include_directories(core/weather_core)
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
        cli/menus_model/cli_menu_controller.cpp
        cli/menus_model/cli_menu_controller.h
        config/config_context.cpp
        config/config_context.h
        cli/cli_application.cpp
        cli/cli_application.cpp
        cli/cli_application.h
        cli/menus_model/ai_advisor/cli_ai_advisor.cpp
        cli/menus_model/ai_advisor/cli_ai_advisor.h
        cli/menus_model/date_display/cli_date_display.cpp
        cli/menus_model/date_display/cli_date_display.h
        cli/menus_model/life_index/cli_life_index.cpp
        cli/menus_model/life_index/cli_life_index.h
        cli/menus_model/user_setting/cli_user_settings.cpp
        cli/menus_model/user_setting/cli_user_settings.h
        core/doubao_manager.cpp
        core/doubao_manager.h
        cli/menus_model/weather_display/cli_weather_display.cpp
        cli/menus_model/weather_display/cli_weather_display.h
        cli/menus_model/update_city/cli_update_city.cpp
        cli/menus_model/update_city/cli_update_city.h
        cli/dispatch_model/cli_menu_dispatch_controller.cpp
        cli/dispatch_model/cli_menu_dispatch_controller.h
)
set(WEATHER_GLOBAL
        core/weather_manager.cpp          # 天气核心实现
        config/config_loader.cpp
        utils/ip_locator.cpp
        config/config_user.cpp
        config/config_user.h
        config/config_key.cpp
        config/config_key.h
        config/base_config.cpp
        config/base_config.h
)

# 源码文件（GUI 用）
set(GUI_SOURCES
        main_gui.cpp                              # GUI 主入口
        gui/mainwindow.cpp                        # 主窗口实现
)

# GUI 头文件（用于自动处理 MOC）
set(GUI_HEADERS
        gui/mainwindow.h                          # 主窗口头文件
        core/weather_manager.h            # 天气管理器头文件
        config/config_loader.h                     # 配置加载头文件
)

# CLI 可执行文件
add_executable(weather_cli ${WEATHER_GLOBAL} ${CLI_SOURCES})
# 创建 CLI 可执行程序 weather_cli，包含上面定义的 CLI_SOURCES
#将curl 连接到可执行文件
target_link_libraries(weather_cli PRIVATE curl)



# GUI 可执行文件
add_executable(weather_gui
        ${GUI_SOURCES}                            # 源码文件
        ${GUI_HEADERS}                            # 头文件（用于 moc）
        ${WEATHER_GLOBAL}                         # 核心库的头文件
        gui/mainwindow.ui                         # Qt Designer 的 UI 文件
)
target_link_libraries(weather_gui curl)
# 链接 Qt6 的 Widgets 和 Network 模块到 GUI 可执行文件
target_link_libraries(weather_gui Qt6::Widgets Qt6::Network)
# 显式告诉 find_package 去哪里找 CURL
set(CURL_INCLUDE_DIR "${CURL_PATH}/include")
set(CURL_LIBRARY "${CURL_PATH}/lib/libcurl.a")  # 如果你用 dll，写 libcurl.dll.a

set(CURL_FOUND TRUE)


find_package(CURL REQUIRED)




# 自动复制运行时 DLL（如 libcurl.dll）到可执行文件目录
# 这种方式不行 因为我们用的是裸连接 用findpackage才行
#    if(WIN32)
##        add_custom_command(TARGET weather_cli POST_BUILD
##                COMMAND ${CMAKE_COMMAND} -E copy_if_different
##                $<TARGET_RUNTIME_DLLS:weather_cli>
##                $<TARGET_FILE_DIR:weather_cli>
##                COMMAND_EXPAND_LISTS
##                COMMENT "Auto copying runtime DLLs (like libcurl.dll) to output directory..."
##        )
##    endif()

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
#apiKey文件
file(COPY ${CMAKE_SOURCE_DIR}/cache.json
        DESTINATION ${CMAKE_BINARY_DIR})
#缓存文件
file(COPY ${CMAKE_SOURCE_DIR}/configKey.json
        DESTINATION ${CMAKE_BINARY_DIR})

#语言文件
file(COPY ${CMAKE_SOURCE_DIR}/cli/i18n/lang_zh.json
        DESTINATION ${CMAKE_BINARY_DIR}/i18n)
#语言文件
file(COPY ${CMAKE_SOURCE_DIR}/cli/i18n/lang_en.json
        DESTINATION ${CMAKE_BINARY_DIR}/i18n)