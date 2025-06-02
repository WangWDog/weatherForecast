    cmake_minimum_required(VERSION 3.21)
    # 要求 CMake 的最低版本为 3.21，确保支持 Qt6 等特性
    project(weatherForecast)
    # 设置当前项目的名称为 WeatherApp

    set(CMAKE_CXX_STANDARD 17)
    # 设置 C++ 标准为 C++17

    set(CMAKE_AUTOMOC ON)
    # 自动处理 Qt 的 MOC（元对象编译器），用于 QObject 派生类的信号与槽机制

    set(CMAKE_AUTORCC ON)
    # 自动处理 Qt 的资源文件（.qrc）

    set(CMAKE_AUTOUIC ON)
    # 自动处理 Qt Designer 生成的 .ui 文件（用于图形界面）

    # 查找 Qt6 包（Widgets、Network 模块）
    find_package(Qt6 REQUIRED COMPONENTS Widgets Network)
    # 查找 Qt6 并确保 Widgets 和 Network 模块可用，否则中止编译

    set(CURL_PATH "F:\\CPlusDependences\\LibCurl\\V8")

    # 包含头文件路径
    include_directories(
            ${CMAKE_CURRENT_SOURCE_DIR}/weather_core  # 天气核心模块路径
            ${CMAKE_CURRENT_SOURCE_DIR}/gui           # GUI 模块路径
            ${CMAKE_CURRENT_SOURCE_DIR}/utils         # 工具模块路径
            ${CMAKE_CURRENT_SOURCE_DIR}/cli           # 命令行模块路径
    )
    #引入CurlLib和bin

    include_directories(${CURL_PATH}/include)
    link_directories(${CURL_PATH}/lib)

    # 源码文件（CLI 用）
    set(CLI_SOURCES
            main_cli.cpp                              # 命令行主入口
            weather_core/weather_manager.cpp          # 天气核心实现
            utils/config_loader.cpp                   # 配置文件加载实现（如读取 JSON）
    )

    # 源码文件（GUI 用）
    set(GUI_SOURCES
            main_gui.cpp                              # GUI 主入口
            gui/mainwindow.cpp                        # 主窗口实现
            weather_core/weather_manager.cpp          # 天气核心实现
            utils/config_loader.cpp                   # 配置文件加载实现
    )

    # GUI 头文件（用于自动处理 MOC）
    set(GUI_HEADERS
            gui/mainwindow.h                          # 主窗口头文件
            weather_core/weather_manager.h            # 天气管理器头文件
            utils/config_loader.h                     # 配置加载头文件
    )

    # CLI 可执行文件
    add_executable(weather_cli ${CLI_SOURCES})
    # 创建 CLI 可执行程序 weather_cli，包含上面定义的 CLI_SOURCES
    #将curl 连接到可执行文件
    target_link_libraries(weather_cli curl)

    # GUI 可执行文件
    add_executable(weather_gui
            ${GUI_SOURCES}                            # 源码文件
            ${GUI_HEADERS}                            # 头文件（用于 moc）
            gui/mainwindow.ui                         # Qt Designer 的 UI 文件
    )
    target_link_libraries(weather_gui curl)
    # 链接 Qt6 的 Widgets 和 Network 模块到 GUI 可执行文件
    target_link_libraries(weather_gui Qt6::Widgets Qt6::Network)


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