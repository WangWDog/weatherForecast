cmake_minimum_required(VERSION 3.21)
project(weatherForecast)

set(CMAKE_CXX_STANDARD 20)

# Customizable path for curl on Windows
set(CURL_PATH "F:\\CPlusDependences\\LibCurl\\V8" CACHE PATH "Path to libcurl")
include_directories(${CURL_PATH}/include)
link_directories(${CURL_PATH}/lib)

# Add module directories
add_subdirectory(src/utils)
add_subdirectory(src/config)
add_subdirectory(src/core)
add_subdirectory(src/cli)

# Application executable
add_executable(weather_cli src/app/main_cli.cpp)

# Link libraries
target_link_libraries(weather_cli PRIVATE cli core config utils curl)

# Windows DLL copy
if(WIN32)
    set(CURL_DLL_PATH ${CURL_PATH}/bin/libcurl-x64.dll)
    add_custom_command(TARGET weather_cli POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CURL_DLL_PATH}"
            $<TARGET_FILE_DIR:weather_cli>
        COMMENT "Copying libcurl.dll to output directory...")
endif()

# Runtime configuration files
file(COPY ${CMAKE_SOURCE_DIR}/configs/configUser.json DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/configs/cache.json DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/configs/configKey.json DESTINATION ${CMAKE_BINARY_DIR})

# i18n resources
file(COPY ${CMAKE_SOURCE_DIR}/src/cli/i18n/lang_zh.json DESTINATION ${CMAKE_BINARY_DIR}/i18n)
file(COPY ${CMAKE_SOURCE_DIR}/src/cli/i18n/lang_en.json DESTINATION ${CMAKE_BINARY_DIR}/i18n)
