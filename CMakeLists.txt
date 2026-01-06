# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(weather-ticker)

# Create LittleFS partition image with the built front-end
littlefs_create_partition_image(littlefs "front" FLASH_IN_PROJECT)

# Note: Front-end assets are expected to be pre-built into front/config.
# CI builds them before invoking idf.py. For local builds, run:
#   (cd front-src/config && npm install && npm run deploy)
# If assets are missing, the LittleFS image may be empty.
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/front/config/index.html)
    message(WARNING "Front-end assets are missing in front/config. Run npm deploy in front-src/config before building.")
endif()
