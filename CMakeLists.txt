# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(weather-ticker)

# Build front-end using npm before creating LittleFS image
add_custom_target(build_frontend
    COMMAND ${CMAKE_SOURCE_DIR}/build-frontend.sh 
            ${CMAKE_SOURCE_DIR}/front-src/config 
            ${CMAKE_SOURCE_DIR}/front/config
    COMMENT "Building front-end with npm..."
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    VERBATIM
)

# Create LittleFS partition image with the built front-end
littlefs_create_partition_image(littlefs "front" FLASH_IN_PROJECT)

# Make sure front-end is built before creating the partition image
add_dependencies(littlefs_littlefs_bin build_frontend)