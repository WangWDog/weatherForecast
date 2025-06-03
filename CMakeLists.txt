cmake_minimum_required(VERSION 3.21)
project(weatherForecast)

set(CMAKE_CXX_STANDARD 17)

# 自动处理 Qt 的 .moc、.qrc、.ui
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找 Qt6 包
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

# 修正后的 curl 安装路径
set(CURL_PATH "E:/DevLibs/curl/curl-8.14.0_1-win64-mingw")

# 头文件路径
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/weather_core
    ${CMAKE_CURRENT_SOURCE_DIR}/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/cli
    ${CURL_PATH}/include
)

# 动态库路径
link_directories(${CURL_PATH}/lib)

# CLI 模块源文件
set(CLI_SOURCES
    main_cli.cpp
    weather_core/weather_manager.cpp
    utils/config_loader.cpp
)

# GUI 模块源文件
set(GUI_SOURCES
    main_gui.cpp
    gui/mainwindow.cpp
    weather_core/weather_manager.cpp
    utils/config_loader.cpp
    gui/mainwindow.ui
)

# GUI 头文件
set(GUI_HEADERS
    gui/mainwindow.h
    weather_core/weather_manager.h
    utils/config_loader.h
)

# CLI 可执行文件
add_executable(weather_cli ${CLI_SOURCES})
target_link_libraries(weather_cli curl)

# GUI 可执行文件
add_executable(weather_gui ${GUI_SOURCES} ${GUI_HEADERS})
target_link_libraries(weather_gui curl Qt6::Widgets Qt6::Network)

# Windows 下复制 DLL 到构建输出目录
if(WIN32)
    set(CURL_DLL_PATH "${CURL_PATH}/bin/libcurl-x64.dll")

    add_custom_command(TARGET weather_cli POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CURL_DLL_PATH}"
        $<TARGET_FILE_DIR:weather_cli>
        COMMENT "Copying libcurl.dll to CLI output directory..."
    )

    add_custom_command(TARGET weather_gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CURL_DLL_PATH}"
        $<TARGET_FILE_DIR:weather_gui>
        COMMENT "Copying libcurl.dll to GUI output directory..."
    )
endif()
