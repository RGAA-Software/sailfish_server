cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(. ENCODER_SRCS)
include_directories(${FFMPEG_INCLUDE_DIRS})
link_directories(${FFMPEG_LIBRARY_DIRS})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/deps/sailfish_protos/messages.proto)
message("${PROTO_SRCS} ")
message("${PROTO_HDRS} ")

add_library(encoder ${ENCODER_SRCS} ${PROTO_SRCS})

message("ffmpeg include  : ${FFMPEG_INCLUDE_DIRS}")
message("ffmpeg lib dir  : ${FFMPEG_LIBRARY_DIRS}")
message("ffmpeg libraries: ${FFMPEG_LIBRARIES}")

target_link_libraries(encoder yuv)
#target_link_libraries(encoder ${FFMPEG_LIBRARIES})
target_link_libraries(encoder avcodec avutil)
target_link_libraries(encoder protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)