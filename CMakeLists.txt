project(scsynth-interop C CXX)

if (NOT CMAKE_C_COMPILER)
    message(FATAL_ERROR "Could not find prerequisite C compiler")
endif ()

if (NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "Could not find prerequisite C++ compiler")
endif ()

########################################################################
## CMake Configuration

cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules
                      ${CMAKE_MODULE_PATH})

########################################################################
## Project/Build Configuration

set(SuperCollider_FIND_REQUIRED TRUE)
find_package(SuperCollider)

include_directories(/usr/include)
include_directories(${SuperCollider_INCLUDE_DIR}/SuperCollider/plugin_interface)
include_directories(${SuperCollider_INCLUDE_DIR}/SuperCollider/common)

add_library(scsynth-interop SHARED scsynth-interop.cpp)
target_link_libraries (scsynth-interop ${SuperCollider_LIBS})
