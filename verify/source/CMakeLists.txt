list (APPEND li_can_slv_SRCS

../io_app_conv/io_app_convc.c
../io_app/io_app_module_change.c

${CMAKE_SOURCE_DIR}/source/arch/windows/unittest/io_can_hw.c
${CMAKE_SOURCE_DIR}/source/arch/windows/unittest/io_can_main_hw.c
${CMAKE_SOURCE_DIR}/source/arch/windows/unittest/io_can_mon_hw.c
${CMAKE_SOURCE_DIR}/source/arch/windows/unittest/io_can_main_hw_handler.c
${CMAKE_SOURCE_DIR}/source/arch/windows/unittest/io_can_async_hw.c

${CMAKE_SOURCE_DIR}/source/api/li_can_slv_api.c

${CMAKE_SOURCE_DIR}/source/async/io_can_async.c

${CMAKE_SOURCE_DIR}/source/config/io_can_config.c

${CMAKE_SOURCE_DIR}/source/core/io_can_main.c
${CMAKE_SOURCE_DIR}/source/core/io_can_main_handler.c
${CMAKE_SOURCE_DIR}/source/core/io_can_mon.c
${CMAKE_SOURCE_DIR}/source/core/io_can.c

${CMAKE_SOURCE_DIR}/source/error/io_can_error.c
${CMAKE_SOURCE_DIR}/source/error/io_can_errno.c

${CMAKE_SOURCE_DIR}/source/sync/io_can_sync.c
${CMAKE_SOURCE_DIR}/source/sync/io_can_sync_handler.c

${CMAKE_SOURCE_DIR}/source/sys/io_can_sys.c

${CMAKE_SOURCE_DIR}/source/util/li_can_slv_assert.c

${CMAKE_SOURCE_DIR}/source/xload/li_can_slv_xload.c
${CMAKE_SOURCE_DIR}/source/xload/li_can_slv_xload_buffer.c

)
add_library(li_can_slv ${li_can_slv_SRCS})

list (APPEND UNITY_SOURCES
../unity/src/unity.c
)
add_library(unity ${UNITY_SOURCES})

get_filename_component(li_can_slv_core_test_absolute_path "li_can_slv_core_test.c" REALPATH CACHE)
get_filename_component(li_can_slv_error_test_absolute_path "li_can_slv_error_test.c" REALPATH CACHE)

execute_process(
 COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/../unity/auto/generate_test_runner.rb ${li_can_slv_core_test_absolute_path}
)

execute_process(
 COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/../unity/auto/generate_test_runner.rb ${li_can_slv_error_test_absolute_path}
)

ADD_EXECUTABLE(li_can_slv_core_test li_can_slv_core_test.c li_can_slv_core_test_Runner.c)
TARGET_LINK_LIBRARIES(li_can_slv_core_test li_can_slv unity)

ADD_EXECUTABLE(li_can_slv_error_test li_can_slv_error_test.c li_can_slv_error_test_Runner.c)
TARGET_LINK_LIBRARIES(li_can_slv_error_test li_can_slv unity)

ADD_CUSTOM_TARGET(unittest_run
	COMMAND 
	li_can_slv_core_test.exe > result.testresult & 
	li_can_slv_error_test.exe >> result.testresult || 
	(exit 0) &
	type result.testresult
)

ADD_CUSTOM_TARGET(unittest_prepare
	COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/../unity/auto/generate_test_runner.rb ${li_can_slv_core_test_absolute_path}
	COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/../unity/auto/generate_test_runner.rb ${li_can_slv_error_test_absolute_path}
)

list(APPEND extra_clean_files "${CMAKE_CURRENT_SOURCE_DIR}/li_can_slv_core_test_Runner.c")
list(APPEND extra_clean_files "${CMAKE_CURRENT_SOURCE_DIR}/li_can_slv_error_test_Runner.c")

SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${extra_clean_files}")

