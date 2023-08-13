cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(. SETTINGS_SRCS)

add_library(settings ${SETTINGS_SRCS})