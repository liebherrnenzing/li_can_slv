cmake_minimum_required(VERSION 3.2)

option(LI_CAN_SLV_ARCH_CANPIE "activate canpie_adapter" ON)
option(LI_CAN_SLV_ARCH_WINDOWS "activate unittest windows" OFF)
option(LI_CAN_SLV_MON "activate LI_CAN_SLV_MON" OFF)

# stack
add_library(li_can_slv_api OBJECT api/li_can_slv_api.c)
add_library(li_can_slv_async OBJECT async/io_can_async.c)
add_library(li_can_slv_config OBJECT config/io_can_config.c)
add_library(li_can_slv_core OBJECT core/io_can.c sys/io_can_sys.c)
add_library(li_can_slv_core_main OBJECT core/io_can_main.c core/io_can_main_handler.c)

if(LI_CAN_SLV_MON)
	add_library(li_can_slv_core_mon OBJECT core/io_can_mon.c )
endif()

add_library(li_can_slv_error OBJECT error/io_can_error.c error/io_can_errno.c)
add_library(li_can_slv_sync OBJECT sync/io_can_sync.c sync/io_can_sync_handler.c)
add_library(li_can_slv_util OBJECT util/li_can_slv_assert.c)
add_library(li_can_slv_xload OBJECT xload/li_can_slv_xload.c xload/li_can_slv_xload_buffer.c)

#stack arch
if(LI_CAN_SLV_ARCH_WINDOWS)
	if(LI_CAN_SLV_MON)
		add_library(li_can_slv_arch OBJECT 
			arch/windows/unittest/io_can_hw.c
			arch/windows/unittest/io_can_main_hw.c
			arch/windows/unittest/io_can_main_hw_handler.c
			arch/windows/unittest/io_can_mon_hw.c
			arch/windows/unittest/io_can_async_hw.c)
	else()
		add_library(li_can_slv_arch OBJECT 
			arch/windows/unittest/io_can_hw.c
			arch/windows/unittest/io_can_main_hw.c
			arch/windows/unittest/io_can_main_hw_handler.c
			arch/windows/unittest/io_can_async_hw.c)
	endif()		
endif()

if(LI_CAN_SLV_MON)
	add_library(li_can_slv STATIC
		$<TARGET_OBJECTS:li_can_slv_api>
		$<TARGET_OBJECTS:li_can_slv_async>
		$<TARGET_OBJECTS:li_can_slv_config>
		$<TARGET_OBJECTS:li_can_slv_config>
		$<TARGET_OBJECTS:li_can_slv_core>
		$<TARGET_OBJECTS:li_can_slv_core_main>
		$<TARGET_OBJECTS:li_can_slv_core_mon>
		$<TARGET_OBJECTS:li_can_slv_error>
		$<TARGET_OBJECTS:li_can_slv_sync>
		$<TARGET_OBJECTS:li_can_slv_util>
		$<TARGET_OBJECTS:li_can_slv_xload>
		$<TARGET_OBJECTS:li_can_slv_arch>
		)
else()
	add_library(li_can_slv STATIC
		$<TARGET_OBJECTS:li_can_slv_api>
		$<TARGET_OBJECTS:li_can_slv_async>
		$<TARGET_OBJECTS:li_can_slv_config>
		$<TARGET_OBJECTS:li_can_slv_config>
		$<TARGET_OBJECTS:li_can_slv_core>
		$<TARGET_OBJECTS:li_can_slv_core_main>
		$<TARGET_OBJECTS:li_can_slv_error>
		$<TARGET_OBJECTS:li_can_slv_sync>
		$<TARGET_OBJECTS:li_can_slv_util>
		$<TARGET_OBJECTS:li_can_slv_xload>
		$<TARGET_OBJECTS:li_can_slv_arch>
		)  
endif()

target_include_directories(li_can_slv PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
	$<INSTALL_INTERFACE:include>
	PRIVATE source
	)
	