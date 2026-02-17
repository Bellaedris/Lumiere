cmake_minimum_required(VERSION 3.27)
project(Lumiere VERSION 0.1)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(glm_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# fix for a weird bug that caused windows.h to expand "max" and "min" keywords into macros.
add_definitions(-DNOMINMAX)

link_directories(${CMAKE_CURRENT_LIST_DIR}/lib)

file(GLOB_RECURSE LUMIERE_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/*.h
        ${CMAKE_CURRENT_LIST_DIR}/src/*.hpp
        ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
)

add_library(Lumiere STATIC ${LUMIERE_SOURCES})

target_include_directories(Lumiere PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

configure_file(LumiereConfig.h.in ${CMAKE_CURRENT_LIST_DIR}/src/LumiereConfig.h)
target_include_directories(Lumiere PUBLIC ${CMAKE_CURRENT_LIST_DIR}/includes)

find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
FetchContent_Declare(
        glm
        GIT_REPOSITORY	https://github.com/g-truc/glm.git
        GIT_TAG 	master
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG master
)
FetchContent_MakeAvailable(yaml-cpp)
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_library(GLAD thirdparty/glad.c)
add_library(STB_IMAGE thirdparty/stb_image.cpp)

file(GLOB_RECURSE IMGUI_SOURCES includes/imgui/*.cpp)
add_library(IMGUI STATIC ${IMGUI_SOURCES})


include_directories("${CMAKE_CURRENT_LIST_DIR}/includes")

target_link_libraries(Lumiere
    PUBLIC
        glm::glm
        IMGUI
        yaml-cpp::yaml-cpp
    PRIVATE
        ${GLFW3_LIBRARY}
        ${OPENGL_LIBRARY}
        GLAD
        ${CMAKE_DL_LIBS}
        STB_IMAGE
        ${ASSIMP_LIBRARY}
)

add_custom_command(TARGET Lumiere POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_LIST_DIR}/dlls"
        "${CMAKE_SOURCE_DIR}/bin"
)