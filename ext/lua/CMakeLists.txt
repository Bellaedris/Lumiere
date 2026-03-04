
cmake_minimum_required(VERSION 3.27)

file(GLOB LUA_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.c)
add_library(lua STATIC ${LUA_SOURCES})

file(GLOB PUBLIC_HEADER lauxlib.h lua.h luaconf.h lualib.h)

target_include_directories(lua PUBLIC ${CMAKE_CURRENT_LIST_DIR})