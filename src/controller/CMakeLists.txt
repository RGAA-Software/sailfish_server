set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(. CTRL_SRCS)

add_library(controller ${CTRL_SRCS})
target_link_libraries(controller ${Boost_LIBRARIES} common)