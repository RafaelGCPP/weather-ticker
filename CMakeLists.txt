# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(weather-ticker)

# Build front-end using npm
add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/front/config/index.html
    COMMAND npm install
    COMMAND npm run deploy
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/front-src/config
    COMMENT "Building front-end with npm..."
    VERBATIM
)

add_custom_target(build_frontend DEPENDS ${CMAKE_SOURCE_DIR}/front/config/index.html)

# Create LittleFS partition image with the built front-end
littlefs_create_partition_image(littlefs "front" FLASH_IN_PROJECT)

# Make sure front-end is built before creating the partition image
add_dependencies(littlefs_littlefs_bin build_frontend)
