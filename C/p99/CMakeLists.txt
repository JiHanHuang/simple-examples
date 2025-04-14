cmake_minimum_required(VERSION 3.23)
project(p99 VERSION 1.0)

#用于ide进行路径解析
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

string(TIMESTAMP VER_STAMP "%Y-%m-%d %H:%M:%S")
add_definitions(-DVER_STAMP="${VER_STAMP}")

set(CMAKE_C_FLAGS "-fPIC -std=gnu11 -Wall -rdynamic -Werror -Wno-strict-aliasing ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-fPIC -std=gnu++11 -Wall -rdynamic -Werror -Wno-strict-aliasing ${CMAKE_CXX_FLAGS}")
    
add_compile_options(-g -w -Wall)
SET(CMAKE_BUILD_TYPE "Debug")

set(ROOT_PATH        ${CMAKE_CURRENT_SOURCE_DIR})

#如果存在其他引用
set(PUB_PATH        ${ROOT_PATH}/public)
set(THIRD_LIBS_PATH ${ROOT_PATH}/third_lib)
# message("PUB_PATH is ${PUB_PATH}")
# message("THIRD_LIBS_PATH is ${THIRD_LIBS_PATH}")

# 添加头文件路径
include_directories(
                    ${ROOT_PATH}
                    ${PUB_PATH}/include
                    ${THIRD_LIBS_PATH}/libplatthirdparty/include/list)

# 添加库路径
link_directories(${THIRD_LIBS_PATH}/libplatthirdparty/lib)

# SET(SOURCE_FILES  ${ROOT_PATH}/print.c)
aux_source_directory(${ROOT_PATH} SOURCE_FILES)


add_executable(p99 ${SOURCE_FILES})

# 链接第三方库
target_link_libraries(p99 event hiredis)

install(TARGETS p99 DESTINATION ${ROOT_PATH}/bin)