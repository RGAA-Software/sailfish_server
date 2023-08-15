cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(. UI_SRCS)

add_library(ui ${UI_SRCS})
target_link_libraries(ui Qt6::Widgets Qt6::Core Qt6::Gui)