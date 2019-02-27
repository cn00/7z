# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.
# by huzongyao

cmake_minimum_required(VERSION 3.4.1)

macro(source_group_by_dir proj_dir source_files)
    if(MSVC OR APPLE)
        get_filename_component(sgbd_cur_dir ${proj_dir} ABSOLUTE)
        foreach(sgbd_file ${${source_files}})
            get_filename_component(sgbd_abs_file ${sgbd_file} ABSOLUTE)
            file(RELATIVE_PATH sgbd_fpath ${sgbd_cur_dir} ${sgbd_abs_file})
            string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "\\")
            endif(sgbd_nogroup)
            source_group(${sgbd_group_name} FILES ${sgbd_file})
        endforeach(sgbd_file)
    endif(MSVC OR APPLE)
endmacro(source_group_by_dir)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -DNDEBUG -D_REENTRANT -DENV_UNIX")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBREAK_HANDLER -DUNICODE -D_UNICODE -DUNIX_USE_WIN_FILE -fPIC")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.
file(GLOB_RECURSE P7ZIP_SRCS 
    ${CMAKE_CURRENT_SOURCE_DIR}/C/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/C/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CPP/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CPP/*.cpp
)
# message(stats "${P7ZIP_SRCS}")
file(GLOB_RECURSE P7ZIP_LUA_BIND_SRCS 
    ${CMAKE_CURRENT_SOURCE_DIR}/lua-bind/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/lua-bind/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lua-bind/*.cpp
)
if(USE_LUA_BIND)
    set(P7ZIP_SRCS ${P7ZIP_SRCS} ${P7ZIP_LUA_BIND_SRCS})
endif(USE_LUA_BIND)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/C
    ${CMAKE_CURRENT_SOURCE_DIR}/CPP
    ${CMAKE_CURRENT_SOURCE_DIR}/CPP/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/CPP/myWindows
    ${CMAKE_CURRENT_SOURCE_DIR}/CPP/include_windows
    /usr/local/include/lua
)

add_library(p7zip_static ${P7ZIP_SRCS})
source_group_by_dir(${CMAKE_CURRENT_SOURCE_DIR} P7ZIP_SRCS)

if (BUILD_SHARED_LIB)
    add_library(p7zip SHARED ${P7ZIP_SRCS})
    if(USE_LUA_BIND)
        find_library(lua lua)
        target_link_libraries(p7zip ${lua})
    endif(USE_LUA_BIND)
endif(BUILD_SHARED_LIB)

if (APPLE AND BUILD_SHARED_LIB)
	find_library(ObjC objc)
	find_library(CoreFoundation CoreFoundation)
	find_library(IOKit IOKit)
    target_link_libraries(p7zip 
        ${CoreFoundation} 
        ${IOKit} 
        ${ObjC}
    )
    
    if(P7ZIP_EXECUTABLE)
        target_link_libraries(7z 
            ${CoreFoundation} 
            ${IOKit} 
            ${ObjC}
        )
    endif(P7ZIP_EXECUTABLE)
endif()
