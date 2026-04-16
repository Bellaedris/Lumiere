cmake_minimum_required(VERSION 3.27)
project(Lumiere VERSION 0.1)

include(FetchContent)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Distribution")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DISTRIBUTION OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# fix for a weird bug that caused windows.h to expand "max" and "min" keywords into macros.
add_definitions(-DNOMINMAX)
add_definitions(-DUUID_SYSTEM_GENERATOR )

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

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ext/lua)

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

# Jolt Physics config from https://github.com/jrouwe/JoltPhysicsHelloWorld/blob/main/Build/CMakeLists.txt
set(CROSS_PLATFORM_DETERMINISTIC ON)
set(CPP_EXCEPTIONS_ENABLED ON)
set(OBJECT_LAYER_BITS 16)
set(INTERPROCEDURAL_OPTIMIZATION OFF)
set(CPP_RTTI_ENABLED ON)
FetchContent_Declare(
        JoltPhysics
        GIT_REPOSITORY "https://github.com/jrouwe/JoltPhysics"
        GIT_TAG "v5.5.0"
        SOURCE_SUBDIR "Build"
)
FetchContent_MakeAvailable(JoltPhysics)
if (MSVC)
    # 64 bit architecture
    set(CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE "x64")

    # Set runtime library
    if (USE_STATIC_MSVC_RUNTIME_LIBRARY)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()

    # Set general compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /Gm- /MP /nologo /diagnostics:classic /FC /fp:except- /Zc:inline")

    # Enable warnings
    if (ENABLE_ALL_WARNINGS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /WX")
    endif()

    # Optionally generate debug symbols
    if (GENERATE_DEBUG_SYMBOLS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
    endif()

    if (NOT CPP_RTTI_ENABLED)
        # Set compiler flag for disabling RTTI
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
    else()
        # Set compiler flag for enabling RTTI
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR")
    endif()

    if (NOT CPP_EXCEPTIONS_ENABLED)
        # Remove any existing compiler flag that enables exceptions
        string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

        # Disable warning about STL and compiler-generated types using noexcept when exceptions are disabled
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4577")
    else()
        # Enable exceptions
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    endif()

    # Set compiler flags for various configurations
    if (OVERRIDE_CXX_FLAGS)
        set(CMAKE_CXX_FLAGS_DEBUG "/GS /Od /Ob0 /RTC1")
        set(CMAKE_CXX_FLAGS_RELEASE "/GS- /Gy /O2 /Oi /Ot")
    endif()

    # Set linker flags
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        if (CROSS_PLATFORM_DETERMINISTIC)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:precise")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast") # Clang doesn't use fast math because it cannot be turned off inside a single compilation unit
        endif()
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /showFilenames")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments") # Clang emits warnings about unused arguments such as /MP and /GL
    endif()
else()
    # Enable warnings
    if (ENABLE_ALL_WARNINGS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    endif()

    # Optionally generate debug symbols
    if (GENERATE_DEBUG_SYMBOLS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    endif()

    if (NOT CPP_RTTI_ENABLED)
        # Set compiler flag for disabling RTTI
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
    else()
        # Set compiler flag for enabling RTTI
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti")
    endif()

    if (NOT CPP_EXCEPTIONS_ENABLED)
        # Set compiler flag for disabling exception-handling
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
    else()
        # Set compiler flag for enabling exception-handling
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
    endif()

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # Also disable -Wstringop-overflow or it will generate false positives that can't be disabled from code when link-time optimizations are enabled
        # Also turn off automatic fused multiply add contractions, there doesn't seem to be a way to do this selectively through the macro JPH_PRECISE_MATH_OFF
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-stringop-overflow -ffp-contract=off")
    else()
        # Do not use -ffast-math since it cannot be turned off in a single compilation unit under clang, see Core.h
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffp-model=precise")

        # On clang 14 and later we can turn off float contraction through a pragma, older versions and deterministic versions need it off always, see Core.h
        if (CMAKE_CXX_COMPILER_VERSION LESS 14 OR CROSS_PLATFORM_DETERMINISTIC)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffp-contract=off")
        endif()
    endif()

    # See https://github.com/jrouwe/JoltPhysics/issues/922. When compiling with DOUBLE_PRECISION=YES and CMAKE_OSX_DEPLOYMENT_TARGET=10.12 clang triggers a warning that we silence here.
    if ("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin" AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -faligned-allocation")
    endif()

    # Set compiler flags for various configurations
    if (OVERRIDE_CXX_FLAGS)
        set(CMAKE_CXX_FLAGS_DEBUG "")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    endif()
    set(CMAKE_CXX_FLAGS_DISTRIBUTION "${CMAKE_CXX_FLAGS_RELEASE}")
endif()
set(CMAKE_EXE_LINKER_FLAGS_DISTRIBUTION "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
SET_INTERPROCEDURAL_OPTIMIZATION()

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

add_library(sol2 INTERFACE)
target_include_directories(sol2
        INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/includes/sol
)
target_compile_definitions(Lumiere PUBLIC SOL_ALL_SAFETIES_ON=1)

target_link_libraries(sol2
        INTERFACE
        lua
)
include_directories("${CMAKE_CURRENT_LIST_DIR}/includes")
target_include_directories(Lumiere PUBLIC ${JoltPhysics_SOURCE_DIR}/..)

# On linux, we need to link against UUID to use system uuid generation
IF (WIN32)
    target_link_libraries(Lumiere
        PUBLIC
            glm::glm
            IMGUI
            yaml-cpp::yaml-cpp
            sol2
        PRIVATE
            ${GLFW3_LIBRARY}
            ${OPENGL_LIBRARY}
            GLAD
            ${CMAKE_DL_LIBS}
            STB_IMAGE
            ${ASSIMP_LIBRARY}
            Jolt
    )
    add_custom_command(TARGET Lumiere POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_LIST_DIR}/dlls"
            "${CMAKE_SOURCE_DIR}/bin"
    )
ELSEIF (LINUX)
    target_link_libraries(Lumiere
        PUBLIC
            glm::glm
            IMGUI
            yaml-cpp::yaml-cpp
            sol2
        PRIVATE
            uuid
            ${GLFW3_LIBRARY}
            ${OPENGL_LIBRARY}
            GLAD
            ${CMAKE_DL_LIBS}
            STB_IMAGE
            ${ASSIMP_LIBRARY}
            Jolt
    )
ENDIF ()