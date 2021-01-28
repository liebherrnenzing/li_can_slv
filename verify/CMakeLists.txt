list (APPEND li_can_slv_test_HEADER
	${CMAKE_CURRENT_SOURCE_DIR}/source
	${CMAKE_CURRENT_SOURCE_DIR}/unity/src
	${CMAKE_CURRENT_SOURCE_DIR}/xtfw
	${CMAKE_CURRENT_SOURCE_DIR}/xtfw/framework/unity
	${CMAKE_CURRENT_SOURCE_DIR}/io_app
	${CMAKE_CURRENT_SOURCE_DIR}/io_app_conv
	${CMAKE_CURRENT_SOURCE_DIR}/io_app_hw
	${CMAKE_CURRENT_SOURCE_DIR}/io_app_frc2
	${CMAKE_CURRENT_SOURCE_DIR}/io_app_incx
	${CMAKE_CURRENT_SOURCE_DIR}/io_app_inxy
	${CMAKE_CURRENT_SOURCE_DIR}/io_app_ma_w
)

list (APPEND li_can_slv_test_SOURCE
	io_app_conv/io_app_convc.c
	io_app/io_app_module_change.c
	unity/src/unity.c
)

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

#find_program(CMAKE_C_CPPCHECK NAMES cppcheck)
#if (CMAKE_C_CPPCHECK)
#	message("-- CppCheck found : ${CMAKE_C_CPPCHECK}")
#	message("-- current source dir : ${CMAKE_CURRENT_SOURCE_DIR}")
#	message("-- source dir : ${CMAKE_SOURCE_DIR}")
#    list(
#        APPEND CMAKE_C_CPPCHECK 
#        	"--enable=warning,style,performance,portability,information,missingInclude"
#        	"--force"    
#            "-i${CMAKE_CURRENT_SOURCE_DIR}/unity/"
#            "-i${CMAKE_SOURCE_DIR}/verify/unity/"
#            #"--suppress=*:${CMAKE_SOURCE_DIR}/verify/unity/*"
#            #"--suppressions-list=${CMAKE_SOURCE_DIR}/CppCheckSuppressions.txt"
#            #"--template=gcc"
#    )
#endif()

add_executable(li_can_slv_core_test source/li_can_slv_core_test.c source/li_can_slv_core_test_Runner.c ${li_can_slv_test_SOURCE})
target_include_directories(li_can_slv_core_test PUBLIC ${li_can_slv_test_HEADER})
target_compile_definitions(li_can_slv_core_test PRIVATE UNITY_INCLUDE_CONFIG_H)
target_compile_options(li_can_slv_core_test PUBLIC -O0 -ggdb)
target_link_libraries(li_can_slv_core_test li_can_slv_core li_can_slv_xload li_can_slv_arch_win_unittest)
add_test(NAME li_can_slv_core_test COMMAND li_can_slv_core_test)

add_executable(li_can_slv_error_test source/li_can_slv_error_test.c source/li_can_slv_error_test_Runner.c ${li_can_slv_test_SOURCE})
target_include_directories(li_can_slv_error_test PUBLIC ${li_can_slv_test_HEADER})
target_compile_definitions(li_can_slv_error_test PRIVATE UNITY_INCLUDE_CONFIG_H)
target_compile_options(li_can_slv_error_test PUBLIC -O0 -ggdb)
target_link_libraries(li_can_slv_error_test li_can_slv_core li_can_slv_xload li_can_slv_arch_win_unittest)
add_test(NAME li_can_slv_error_test COMMAND li_can_slv_error_test)

add_executable(li_can_slv_sync_test source/li_can_slv_sync_test.c source/li_can_slv_sync_test_Runner.c ${li_can_slv_test_SOURCE})
target_include_directories(li_can_slv_sync_test PUBLIC ${li_can_slv_test_HEADER})
target_compile_definitions(li_can_slv_sync_test PRIVATE UNITY_INCLUDE_CONFIG_H)
target_compile_options(li_can_slv_sync_test PUBLIC -O0 -ggdb)
target_link_libraries(li_can_slv_sync_test li_can_slv_core li_can_slv_xload li_can_slv_arch_win_unittest)
add_test(NAME li_can_slv_sync_test COMMAND li_can_slv_sync_test)

add_custom_target(unittest_run
	COMMAND 
	li_can_slv_core_test.exe > result.testresult & 
	li_can_slv_error_test.exe >> result.testresult & 
	li_can_slv_sync_test.exe >> result.testresult &
	type result.testresult
)

add_custom_target(unittest_prepare
	COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/unity/auto/generate_test_runner.rb ${li_can_slv_core_test_absolute_path}
	COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/unity/auto/generate_test_runner.rb ${li_can_slv_error_test_absolute_path}
	COMMAND ruby ${CMAKE_CURRENT_SOURCE_DIR}/unity/auto/generate_test_runner.rb ${li_can_slv_sync_test_absolute_path}	
)

list(APPEND extra_clean_files "${CMAKE_CURRENT_SOURCE_DIR}/source/li_can_slv_core_test_Runner.c")
list(APPEND extra_clean_files "${CMAKE_CURRENT_SOURCE_DIR}/source/li_can_slv_error_test_Runner.c")
list(APPEND extra_clean_files "${CMAKE_CURRENT_SOURCE_DIR}/source/li_can_slv_sync_test_Runner.c")

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${extra_clean_files}")
