cmake_minimum_required(VERSION 3.20)

include(env.cmake)

SET(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

project(sailfish_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# config for each platform
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message("Config for Windows")

    include_directories(${CMAKE_SOURCE_DIR}/deps/ext_ffmpeg_prebuild/win64/include)
    add_definitions(-D_OS_WINDOWS_)
    add_definitions(-DUNICODE -D_UNICODE)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("Config for Linux")

    add_definitions(-DOS_LINUX)

endif ()
# config end

# find packages
#find_package(glfw3 CONFIG REQUIRED)
find_package(libyuv CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(wil CONFIG REQUIRED)
find_package(Opus CONFIG REQUIRED)
# If you want to use ffmpeg installed by your vcpkg, you need to
# ensure that ffmpeg's encoders were enabled, such as,
# x264(software), x265(software), nvenc(hardware). Then, enable linking
# statement in encoder/CMakeLists.txt
#find_package(FFMPEG REQUIRED)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui)
find_package(Boost COMPONENTS system filesystem thread locale program_options random)
# find packages end

# include
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_BINARY_DIR}/src/encoder)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/deps)
include_directories(${CMAKE_SOURCE_DIR}/deps/websocketpp)
include_directories(${CMAKE_SOURCE_DIR}/deps/rgaa_common)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    MESSAGE(STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}.")
    MESSAGE(STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}.")
    MESSAGE(STATUS "Boost_LIB_VERSION = ${Boost_LIB_VERSION}.")
endif()

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# include end

add_subdirectory(src)
add_subdirectory(deps)

set(RES resources/font.qrc resources/main.qrc resources/qss.qrc resources.qrc)

add_executable(sailfish_server main.cpp logo.rc ${RES})
target_link_libraries(sailfish_server main)
target_link_libraries(sailfish_server #[[glfw]] ${Boost_LIBRARIES} WIL::WIL)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_custom_command(TARGET sailfish_server PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/deps/ext_ffmpeg_prebuild/win64/lib/avcodec.lib"  "${CMAKE_BINARY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/deps/ext_ffmpeg_prebuild/win64/lib/avutil.lib"  "${CMAKE_BINARY_DIR}"
    )
    add_custom_command(TARGET sailfish_server POST_BUILD
        COMMAND ${QT_ROOT}/bin/windeployqt.exe $<TARGET_FILE:${PROJECT_NAME}> --no-translations
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/deps/ext_ffmpeg_prebuild/win64/bin/avcodec-60.dll"  "${CMAKE_BINARY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/deps/ext_ffmpeg_prebuild/win64/bin/avutil-58.dll"  "${CMAKE_BINARY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/deps/ext_ffmpeg_prebuild/win64/bin/swresample-4.dll"  "${CMAKE_BINARY_DIR}"

    )

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("Config for Linux")

    add_definitions(-DOS_LINUX)

endif ()
