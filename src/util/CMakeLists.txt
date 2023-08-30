cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(. UTIL_SRCS)

add_library(util ${UTIL_SRCS})
target_link_libraries(util Qt6::Widgets Qt6::Core Qt6::Gui Qt6::Network)