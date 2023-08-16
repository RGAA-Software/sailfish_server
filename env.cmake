if(WIN32)
    set(VCPKG_ROOT C:/source/vcpkg)
    set(QT_ROOT C:/Qt/Qt6.4/6.4.0/msvc2019_64)

#    set(VCPKG_ROOT G:/vcpkg)
#    set(QT_ROOT F:/qt6.4/6.4.0/msvc2019_64)

    set(CMAKE_PREFIX_PATH ${QT_ROOT}/lib/cmake)
elseif (UNIX)

endif ()