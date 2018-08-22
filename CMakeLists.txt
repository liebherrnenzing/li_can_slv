PROJECT(li_can_slv)

SET(CMAKE_SYSTEM_NAME Windows)

# Set Path of own or modified CMake-Modules
# SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/project/cmake_modules)

# set minimum cmake version
cmake_minimum_required(VERSION 3.0)

# Uncomment if you want to do profiling with gprof
SET(CMAKE_CXX_FLAGS "-g -O0 -Wall -W -fprofile-arcs -ftest-coverage -save-temps")
SET(CMAKE_C_FLAGS "-g -O0 -Wall -W -fprofile-arcs -ftest-coverage -save-temps")

SET(LI_CAN_SLV_ARCH_WINDOWS ON)

#set custom defines
add_definitions(-DUNITY_INCLUDE_CONFIG_H)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include/li_can_slv/arch/windows/unittest)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/verify/source)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/verify/unity/src)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/verify/xtfw)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/verify/xtfw/framework/unity)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/verify/io_app)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/verify/io_app_conv)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/verify/io_app_hw)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/verify/io_app_frc2)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/verify/io_app_incx)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/verify/io_app_inxy)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/verify/io_app_ma_w)

add_subdirectory(verify/source)
add_subdirectory(source)