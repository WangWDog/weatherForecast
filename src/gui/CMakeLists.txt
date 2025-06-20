set(GUI_SOURCES
    mainwindow.cpp
)

add_library(gui STATIC ${GUI_SOURCES})

qt_wrap_ui(GUI_HEADERS mainwindow.ui)

# but we don't have Qt environment; but we just keep as placeholder

# For now just include directories

target_include_directories(gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

