cmake_minimum_required(VERSION 3.17)
project(vertx_tcp_seastar)

set(CMAKE_CXX_STANDARD 20)
set(EXTRA_LIBRARY /usr/local/lib/libseastar.a
        /usr/lib/libboost_program_options.so
        /usr/lib/libboost_thread.so
        /usr/lib/libcares.so
        /usr/lib/libcryptopp.so
        /usr/lib/libfmt.so.6.2.0
        -ldl
        -lrt
        /usr/lib/libboost_filesystem.so
        /usr/lib/libsctp.so
        /usr/lib/libnuma.so
        -latomic
        -llz4
        -lgnutls
        -lgmp
        -lunistring
        -lidn2
        -latomic
        -lhogweed
        -lgmp
        -lnettle
        -ltasn1
        -lp11-kit
        -lprotobuf
        -lz
        -lhwloc
        -lm
        -ludev
        -ldl
        -lpthread
        -lyaml-cpp
        )
set(LIBRARIES_SEARCH_PATHS ${EXTRA_LIBRARY})
include_directories(${PROJECT_SOURCE_DIR}/include)
#include_directories(/home/nerull/dev/repo/cpp/hazelcast-cpp-client-3.12.1-Linux_64/cpp/Linux_64/hazelcast/include )
include_directories(/home/nerull/dev/repo/cpp/hazelcast-cpp-client/hazelcast-cpp-client/cpp/Linux_64/hazelcast/include )
#/home/nerull/dev/repo/cpp/hazelcast-cpp-client/hazelcast-cpp-client/external/include

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

add_library(hazelcast STATIC IMPORTED)
#set_property(TARGET hazelcast PROPERTY IMPORTED_LOCATION /home/nerull/dev/repo/cpp/hazelcast-cpp-client/hazelcast-cpp-client/release/libHazelcastClient3.12_64.a)
set_property(TARGET hazelcast PROPERTY IMPORTED_LOCATION /home/nerull/dev/repo/cpp/hazelcast-cpp-client/hazelcast-cpp-client/cpp/Linux_64/hazelcast/lib/libHazelcastClient3.12_64.a)


add_executable(vertx_tcp_seastar_bin main.cpp
        src/tcp/connection.cpp
        src/tcp/tcp_server.cpp
        src/vertx/hazelcast_cluster.cpp
        include/tcp/client_connection.h)
target_link_libraries (vertx_tcp_seastar_bin  ${LIBRARIES_SEARCH_PATHS}  -lrt -lssl -lcrypto -lc hazelcast)