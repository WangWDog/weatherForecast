set(CONFIG_SOURCES
    base_config.cpp
    config_context.cpp
    config_key.cpp
    config_user.cpp
)

add_library(config STATIC ${CONFIG_SOURCES})

target_include_directories(config PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../utils)

target_link_libraries(config PUBLIC utils)
