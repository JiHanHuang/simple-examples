cmake_minimum_required(VERSION 3.23)
project(plugin_module)

add_compile_options(-Wall -g  -fPIC -Wno-stringop-truncation)
SET(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ROOT_PATH        ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
    ${ROOT_PATH}/plugin
)

add_executable(pm
    ${ROOT_PATH}/main.c
)

add_subdirectory("plugin")

target_link_libraries(pm dl)
install(TARGETS pm DESTINATION ${ROOT_PATH}/target)