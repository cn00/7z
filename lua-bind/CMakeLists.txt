# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.
# by huzongyao

cmake_minimum_required(VERSION 3.4.1)


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.
file(GLOB_RECURSE p7zip_lua_bind_SRCS 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
# message(stats "${p7zip_lua_bind_SRCS}")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../p7zip/
    ${CMAKE_CURRENT_SOURCE_DIR}/../p7zip/C
    ${CMAKE_CURRENT_SOURCE_DIR}/../p7zip/CPP
    ${CMAKE_CURRENT_SOURCE_DIR}/../p7zip/CPP/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/../p7zip/CPP/myWindows
    ${CMAKE_CURRENT_SOURCE_DIR}/../p7zip/CPP/include_windows
)

add_library(p7zip_lua_bind ${p7zip_lua_bind_SRCS})
source_group_by_dir(${CMAKE_CURRENT_SOURCE_DIR} p7zip_lua_bind_SRCS)
