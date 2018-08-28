PROJECT(li_can_slv)

SET(CMAKE_SYSTEM_NAME Windows)

# Set Path of own or modified CMake-Modules
# SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/project/cmake_modules)

# set minimum cmake version
cmake_minimum_required(VERSION 3.0)

# Uncomment if you want to do profiling with gprof
SET(CMAKE_CXX_FLAGS "-g -O0 -Wall -W -fprofile-arcs -ftest-coverage -save-temps")
SET(CMAKE_C_FLAGS "-g -O0 -Wall -W -fprofile-arcs -ftest-coverage -save-temps")

set(LI_CAN_SLV_ARCH_CANPIE OFF CACHE BOOL "activate canpie_adapter")
set(LI_CAN_SLV_ARCH_WINDOWS ON CACHE BOOL "activate unittest windows")

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/verify/source)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include/li_can_slv/arch/windows/unittest)

add_subdirectory(verify)
add_subdirectory(source)

