cmake_minimum_required(VERSION 3.29)
project(ExtendedDemonStars VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "GEODE_SDK is not set. Install the Geode SDK/CLI and configure your profile.")
endif()

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    src/StarCalculator.cpp
    src/DataManager.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

setup_geode_mod(${PROJECT_NAME})
