cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(capture)
add_subdirectory(settings)
add_subdirectory(context)
add_subdirectory(encoder)
add_subdirectory(network)
add_subdirectory(controller)
add_subdirectory(audio)
add_subdirectory(ui)
add_subdirectory(util)

aux_source_directory(. SRCS)
add_library(main ${SRCS})
target_link_libraries(main capture settings context common encoder network controller audio opus_codec ui util)