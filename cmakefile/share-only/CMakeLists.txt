cmake_minimum_required(VERSION 3.23)
project(share-only VERSION 1.0)

#用于ide进行路径解析
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

string(TIMESTAMP VER_STAMP "%Y-%m-%d %H:%M:%S")
add_definitions(-DVER_STAMP="${VER_STAMP}")

set(CMAKE_C_FLAGS "-fPIC -std=gnu11 -Wall -rdynamic -Werror -Wno-strict-aliasing ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-fPIC -std=gnu++11 -Wall -rdynamic -Werror -Wno-strict-aliasing ${CMAKE_CXX_FLAGS}")
    
add_compile_options(-g -w -Wall)
SET(CMAKE_BUILD_TYPE "Debug")

option(UNIT_TEST "Unit test" ON)

set(ROOT_PATH        ${CMAKE_CURRENT_SOURCE_DIR})

#如果存在其他引用
set(PUB_PATH        ${ROOT_PATH}/public)
set(THIRD_LIBS_PATH ${ROOT_PATH}/third_lib)
# message("PUB_PATH is ${PUB_PATH}")
# message("THIRD_LIBS_PATH is ${THIRD_LIBS_PATH}")

include_directories(
                    ${ROOT_PATH}
                    ${PUB_PATH}/include
                    ${THIRD_LIBS_PATH}/libplatthirdparty/include/list)


SET(SOURCE_FILES  ${ROOT_PATH}/share.c)

if (NOT UNIT_TEST)
    message("build share lib")

    add_library(share ${SOURCE_FILES})

    install(TARGETS share DESTINATION ${ROOT_PATH}/lib)
    install(FILES ./share.h DESTINATION ${ROOT_PATH}/include/share)

else()
    #测试进程编译
    message("build share Unit test")

    include_directories(${ROOT_PATH}/include)
    link_directories(${ROOT_PATH}/lib)

    add_executable(share_test ./share_test.c)
    target_link_libraries(share_test share)
    
    install(TARGETS share_test DESTINATION ${ROOT_PATH}/bin)
endif()