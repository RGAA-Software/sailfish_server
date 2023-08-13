cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(. NETWORK_SRCS)

add_library(network ${NETWORK_SRCS})