set(UTILS_SOURCES
    doubao_helper.cpp
    http_client.cpp
    ip_locator.cpp
)

add_library(utils STATIC ${UTILS_SOURCES})

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../config)
target_link_libraries(utils PUBLIC config)
