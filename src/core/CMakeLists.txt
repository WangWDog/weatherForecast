set(CORE_SOURCES
    CacheManager.cpp
    doubao_manager.cpp
    weather_manager.cpp
)

add_library(core STATIC ${CORE_SOURCES})

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../utils ../config)

target_link_libraries(core PUBLIC utils config)
