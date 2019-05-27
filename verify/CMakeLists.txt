#set custom defines
add_definitions(-DUNITY_INCLUDE_CONFIG_H)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/source)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/unity/src)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/xtfw)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/xtfw/framework/unity)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/io_app)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/io_app_conv)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/io_app_hw)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/io_app_frc2)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/io_app_incx)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/io_app_inxy)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/io_app_ma_w)

list (APPEND li_can_slv_hw_SOURCE
	io_app_conv/io_app_convc.c
	io_app/io_app_module_change.c
)

add_library(li_can_slv_hw ${li_can_slv_hw_SOURCE})

list (APPEND UNITY_SOURCES
	unity/src/unity.c
)
add_library(unity ${UNITY_SOURCES})

get_filename_component(li_can_slv_core_test_absolute_path "source/li_can_slv_core_test.c" REALPATH CACHE)
get_filename_component(li_can_slv_error_test_absolute_path "source/li_can_slv_error_test.c" REALPATH CACHE)
get_filename_component(li_can_slv_sync_test_absolute_path "source/li_can_slv_sync_test.c" REALPATH CACHE)

execute_process(
 COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/unity/auto/generate_test_runner.rb ${li_can_slv_core_test_absolute_path}
)

execute_process(
 COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/unity/auto/generate_test_runner.rb ${li_can_slv_error_test_absolute_path}
)

execute_process(
 COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/unity/auto/generate_test_runner.rb ${li_can_slv_sync_test_absolute_path}
)


ADD_EXECUTABLE(li_can_slv_core_test source/li_can_slv_core_test.c source/li_can_slv_core_test_Runner.c)
TARGET_LINK_LIBRARIES(li_can_slv_core_test li_can_slv li_can_slv_hw unity)

ADD_EXECUTABLE(li_can_slv_error_test source/li_can_slv_error_test.c source/li_can_slv_error_test_Runner.c)
TARGET_LINK_LIBRARIES(li_can_slv_error_test li_can_slv li_can_slv_hw unity)

ADD_EXECUTABLE(li_can_slv_sync_test source/li_can_slv_sync_test.c source/li_can_slv_sync_test_Runner.c)
TARGET_LINK_LIBRARIES(li_can_slv_sync_test li_can_slv li_can_slv_hw unity)


ADD_CUSTOM_TARGET(unittest_run
	COMMAND 
	li_can_slv_core_test.exe > result.testresult & 
	li_can_slv_error_test.exe >> result.testresult & 
	li_can_slv_sync_test.exe >> result.testresult ||
	(exit 0) &
	type result.testresult
)

ADD_CUSTOM_TARGET(unittest_prepare
	COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/unity/auto/generate_test_runner.rb ${li_can_slv_core_test_absolute_path}
	COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/unity/auto/generate_test_runner.rb ${li_can_slv_error_test_absolute_path}
	COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/unity/auto/generate_test_runner.rb ${li_can_slv_sync_test_absolute_path}	
)

list(APPEND extra_clean_files "${CMAKE_CURRENT_SOURCE_DIR}/source/li_can_slv_core_test_Runner.c")
list(APPEND extra_clean_files "${CMAKE_CURRENT_SOURCE_DIR}/source/li_can_slv_error_test_Runner.c")
list(APPEND extra_clean_files "${CMAKE_CURRENT_SOURCE_DIR}/source/li_can_slv_sync_test_Runner.c")

SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${extra_clean_files}")
