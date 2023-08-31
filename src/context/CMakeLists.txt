cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(. CONTEXT_SRCS)

add_library(context ${CONTEXT_SRCS})
target_link_libraries(context Qt6::Widgets Qt6::Core Qt6::Gui)
target_link_libraries(context leveldb::leveldb)