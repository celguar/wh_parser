cmake_minimum_required(VERSION 3.24)
project(wh_parser)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Runtime)

#set(CMAKE_TOOLCHAIN_FILE "D:/Soft/web-scrap/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE INTERNAL "" FORCE)
# Add the cmake modules directory to our module path.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
find_package(cpr CONFIG REQUIRED)
find_package(libmysql CONFIG REQUIRED)
find_package(Gumbo REQUIRED)

add_executable(wh_parser
        Main.cpp Main.h)

#message(STATUS "vars: ${MYSQL_LIBRARIES} ${MYSQL_LIBRARIES}")
target_link_libraries(wh_parser PRIVATE cpr::cpr)
target_link_libraries(wh_parser PRIVATE ${MYSQL_LIBRARIES})
target_link_libraries(wh_parser PRIVATE ${Gumbo_static_LIBRARY})
#target_link_libraries(wh_parser PRIVATE libmysql::libmysql)
