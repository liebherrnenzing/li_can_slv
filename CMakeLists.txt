cmake_minimum_required(VERSION 3.16)

project(li_can_slv LANGUAGES C VERSION 2.0.0)

# Set Path of own or modified CMake-Modules
# SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/project/cmake_modules)

# Uncomment if you want to do profiling with gprof
#SET(CMAKE_CXX_FLAGS "-g -O0 -Wall -W -fprofile-arcs -ftest-coverage -save-temps")
#SET(CMAKE_C_FLAGS "-g -O0 -Wall -W -fprofile-arcs -ftest-coverage -save-temps")

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

option(LI_CAN_SLV_ARCH_CANPIE "activate canpie_adapter" OFF)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
	option(LI_CAN_SLV_ARCH_WINDOWS "activate unittest windows" ON)
else()
	option(LI_CAN_SLV_ARCH_WINDOWS "activate unittest windows" OFF)
endif()

option(LI_CAN_SLV_MON "activate LI_CAN_SLV_MON" OFF)

# core lib => li_can_slv_core
add_library(${PROJECT_NAME}_core INTERFACE)

target_sources(${PROJECT_NAME}_core INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/source/api/li_can_slv_api.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/async/io_can_async.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/async/io_can_async_handler.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/config/io_can_config.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/core/io_can_main_handler.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/core/io_can_main.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/core/io_can.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/error/io_can_errno.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/error/io_can_error.c 
	${CMAKE_CURRENT_SOURCE_DIR}/source/sync/io_can_sync_handler.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/sync/io_can_sync.c 
	${CMAKE_CURRENT_SOURCE_DIR}/source/sys/io_can_sys.c	
	${CMAKE_CURRENT_SOURCE_DIR}/source/util/li_can_slv_assert.c
)

target_include_directories(${PROJECT_NAME}_core INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>)

# reconnect lib => li_can_slv_reconnect
add_library(${PROJECT_NAME}_reconnect INTERFACE)

target_sources(${PROJECT_NAME}_reconnect INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/source/core/io_can_reconnect.c
)

target_include_directories(${PROJECT_NAME}_reconnect INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>)

# xload lib => li_can_slv_xload
add_library(${PROJECT_NAME}_xload INTERFACE)

target_sources(${PROJECT_NAME}_xload INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/source/xload/li_can_slv_xload_buffer.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/xload/li_can_slv_xload.c 
)

target_include_directories(${PROJECT_NAME}_xload INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>)

# xload_info lib => li_can_slv_xload_info
add_library(${PROJECT_NAME}_xload_info INTERFACE)

target_sources(${PROJECT_NAME}_xload_info INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/source/xload/li_can_slv_xload_info.c
)

target_include_directories(${PROJECT_NAME}_xload_info INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>)

# arch lib => li_can_slv_arch
add_library(${PROJECT_NAME}_arch INTERFACE)

if(LI_CAN_SLV_ARCH_WINDOWS)
	message("arch windows")
	set(arch "arch/windows/unittest")
elseif(LI_CAN_SLV_ARCH_CANPIE)
	message("arch/canpie_adapter")
	set(arch "arch/canpie_adapter")
endif()

if(LI_CAN_SLV_MON)
target_sources(${PROJECT_NAME}_arch INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/source/${arch}/io_can_hw.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/${arch}/io_can_main_hw.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/${arch}/io_can_main_hw_handler.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/${arch}/io_can_mon_hw.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/${arch}/io_can_async_hw.c)		
else()
target_sources(${PROJECT_NAME}_arch INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/source/${arch}/io_can_hw.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/${arch}/io_can_main_hw.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/${arch}/io_can_main_hw_handler.c
	${CMAKE_CURRENT_SOURCE_DIR}/source/${arch}/io_can_async_hw.c)
endif()

target_include_directories(${PROJECT_NAME}_arch INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/li_can_slv/${arch}>
	$<INSTALL_INTERFACE:/include/li_can_slv/${arch}>)
		
# li_can_slv_core, li_can_slv_reconnect, li_can_slv_xload, li_can_slv_xload_info, li_can_slv_arch

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
	add_subdirectory(verify)
endif()
