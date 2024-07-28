cmake_minimum_required(VERSION 3.22)
project(wh_parser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8) # The commit hash for 1.10.5. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

# default to x86 platform.  We'll check for X64 in a bit
SET(PLATFORM X86)
IF(CMAKE_SIZEOF_VOID_P MATCHES 8)
    MESSAGE(STATUS "Detected 64-bit platform.")
    IF(WIN32)
        ADD_DEFINITIONS("-D_WIN64")
    ENDIF()
    SET(PLATFORM X64)
ELSE()
    MESSAGE(STATUS "Detected 32-bit platform.")
ENDIF()

# Find out what system we use to include the needed libs
if(WIN32)
    if(PLATFORM MATCHES X86) # 32-bit
        set(DEP_ARCH win32)
    else() # 64-bit
        set(DEP_ARCH x64)
    endif()
endif()

# Win32 delifered packages
if(WIN32)
    set(MYSQL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/dep/windows/include/mysql)
    set(MYSQL_LIBRARY ${PROJECT_SOURCE_DIR}/dep/windows/lib/${DEP_ARCH}_release/libmySQL.lib)
    set(MYSQL_DEBUG_LIBRARY ${PROJECT_SOURCE_DIR}/dep/windows/lib/${DEP_ARCH}_debug/libmySQL.lib)
    #set(OPENSSL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dep/windows/include/openssl)
    #set(OPENSSL_LIBRARIES ${CMAKE_SOURCE_DIR}/dep/windows/lib/${DEP_ARCH}_release/libeay32.lib)
    #set(OPENSSL_DEBUG_LIBRARIES ${CMAKE_SOURCE_DIR}/dep/windows/lib/${DEP_ARCH}_debug/libeay32.lib)
    #set(ZLIB_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/dep/windows/include/zlib)
endif()
# *nix-specific packages
#[[if(UNIX)
    find_package(MySQL REQUIRED)
    find_package(OpenSSL REQUIRED)
    find_package(ZLIB REQUIRED)
endif()]]

# Add Gumbo
file(GLOB GUMBO_HEADERS_FILES
        ${PROJECT_SOURCE_DIR}/dep/windows/gumbo/src/*.h
)

file(GLOB GUMBO_EXPORT_HEADERS_FILES
        ${PROJECT_SOURCE_DIR}/dep/windows/gumbo/src/gumbo.h
        ${PROJECT_SOURCE_DIR}/dep/windows/gumbo/src/tag_enum.h
)

if(MSVC)
    list(APPEND GUMBO_EXPORT_HEADERS_FILES ${PROJECT_SOURCE_DIR}/dep/windows/gumbo/visualc/include/strings.h)
endif()

file(INSTALL ${GUMBO_EXPORT_HEADERS_FILES} DESTINATION ${CMAKE_BINARY_DIR}/include/gumbo)

file(GLOB GUMBO_SOURCES_FILES
        ${PROJECT_SOURCE_DIR}/dep/windows/gumbo/src/*.c
)

add_library(gumbo STATIC
        ${GUMBO_HEADERS_FILES}
        ${GUMBO_SOURCES_FILES}
)

target_include_directories(gumbo
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
        # For support include different way. (gumbo/gumbo.h or gumbo.h)
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include/gumbo>
        $<INSTALL_INTERFACE:include/gumbo>
)

add_executable(wh_parser
        Main.cpp Main.h)

include_directories(${MYSQL_INCLUDE_DIR})

target_link_libraries(wh_parser PRIVATE cpr::cpr)
target_link_libraries(wh_parser PRIVATE optimized ${MYSQL_LIBRARY})
target_link_libraries(wh_parser PRIVATE debug ${MYSQL_DEBUG_LIBRARY})
target_link_libraries(wh_parser PRIVATE gumbo)

if(WIN32)
    add_custom_command(TARGET wh_parser POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${PROJECT_SOURCE_DIR}/dep/windows/lib/${DEP_ARCH}_$<CONFIG>/libeay32.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/"
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${PROJECT_SOURCE_DIR}/dep/windows/lib/${DEP_ARCH}_$<CONFIG>/libmySQL.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/"
    )
endif()
