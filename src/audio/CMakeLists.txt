cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR})

aux_source_directory(. AUDIO_SRCS)

add_library(audio ${AUDIO_SRCS})
target_link_libraries(audio Winmm)