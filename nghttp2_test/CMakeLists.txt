cmake_minimum_required(VERSION 2.6)
project(nghttp2_test)

aux_source_directory(client CLIENT_SRCS)
aux_source_directory(server SERVER_SRCS)

#option(HAVE_FCNTL_H "" ON)
set(SSL /root/opensource/openssl-1.1.1f)
include_directories(${SSL}/include)
#find_library(my_lib libcrypto.so /root/opensource/openssl-1.1.1f)
#find_library(my_lib libssl.so /root/opensource/openssl-1.1.1f)
link_directories(${SSL})
#set(LD ${my_lib} nghttp2 event event_openssl)
set(LD crypto ssl nghttp2 event event_openssl)

add_executable(client ${CLIENT_SRCS})
target_compile_definitions(client PRIVATE HAVE_FCNTL_H)
target_link_libraries(client ${LD} http_parser)

add_executable(server ${SERVER_SRCS})
target_compile_definitions(server PRIVATE HAVE_FCNTL_H)
target_link_libraries(server ${LD})