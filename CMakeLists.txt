cmake_minimum_required(VERSION 3.21)

project(SuperBuffer VERSION 0.1.0)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

setup_geode_mod(${PROJECT_NAME})