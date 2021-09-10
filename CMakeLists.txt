cmake_minimum_required(VERSION 2.8.2)

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})

set(GTEST_BUILD ${CMAKE_CURRENT_BINARY_DIR}/googletest-build)
set(GTEST_SRC ${CMAKE_CURRENT_SOURCE_DIR}/googletest)
set(GTEST_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/googletest-install)
file(MAKE_DIRECTORY ${GTEST_BUILD})
file(MAKE_DIRECTORY ${GTEST_INSTALL})

project(maple_tree)

# compiler googletest at configure time
execute_process(COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${GTEST_INSTALL} "${CMAKE_GENERATOR}" ${GTEST_SRC}
	       RESULT_VARIABLE result
	       WORKING_DIRECTORY ${GTEST_BUILD})
if(result)
   message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

execute_process(COMMAND make 
	       RESULT_VARIABLE result
	       WORKING_DIRECTORY ${GTEST_BUILD})
if(result)
   message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

execute_process(COMMAND make install
	       RESULT_VARIABLE result
	       WORKING_DIRECTORY ${GTEST_BUILD})
if(result)
   message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

include_directories("${GTEST_INSTALL}/include")

# Now simply link against gtest or gtest_main as needed. Eg
set(TEST_SOURCE_FILES "")
add_subdirectory(test)
add_executable(maple_tree ${TEST_SOURCE_FILES})

if (EXISTS "${GTEST_INSTALL}/lib64/libgtest.a")
   target_link_libraries(maple_tree ${GTEST_INSTALL}/lib64/libgtest.a)
else()
   target_link_libraries(maple_tree ${GTEST_INSTALL}/lib/libgtest.a)
endif()