cmake_minimum_required(VERSION 3.1)

project(SFMLTest)

## If you want to link SFML statically
set(SFML_STATIC_LIBRARIES TRUE)

## In most cases better set in the CMake cache
set(SFML_DIR "C:/SFML2/lib/cmake/SFML")

aux_source_directory(lua LUA_SOURCES)
file(GLOB_RECURSE LUA_SOURCES "lua/*.c" "lua/*.h")

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
add_executable(SFMLTest main.cpp CartesianAxis.hpp ${LUA_SOURCES})
target_link_libraries(SFMLTest sfml-graphics sfml-audio)