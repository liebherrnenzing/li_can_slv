#----------------------------------------------------------------------------------------------------------------------#
# Top level CMake file for building the CANpie FD project                                                              #
#                                                                                                                      #
#----------------------------------------------------------------------------------------------------------------------#

#-------------------------------------------------------------------------------------------------------
# specify minimum version of CMake that can be used with CMakeLists.txt file
#
cmake_minimum_required(VERSION 3.16.4 FATAL_ERROR)
cmake_policy(SET CMP0048 NEW)

#-------------------------------------------------------------------------------------------------------
# This project can be translated with the following commands:
# > cd build
# > cmake ..
# > cmake --build .
# 


#-------------------------------------------------------------------------------------------------------
# specify project
#
project("CANpie FD")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/application)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

