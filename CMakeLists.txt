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
set(SOURCE_FILES_LIST "")
set(TEST_FILES_LIST "")
set(SOURCE_INCLUDE_PATH "")
add_subdirectory(src)
add_subdirectory(test)
include_directories(${SOURCE_INCLUDE_PATH})
message(STATUS "SOURCE_INCLUDE_PATH=${SOURCE_INCLUDE_PATH}")
message(STATUS "SOURCE_FILES_LIST=${SOURCE_FILES_LIST}")
message(STATUS "TEST_FILES_LIST=${TEST_FILES_LIST}")
add_executable(maple_tree ${SOURCE_FILES_LIST} ${TEST_FILES_LIST})

if (EXISTS "${GTEST_INSTALL}/lib64/libgtest.a")
   target_link_libraries(maple_tree ${GTEST_INSTALL}/lib64/libgtest.a pthread)
else()
   target_link_libraries(maple_tree ${GTEST_INSTALL}/lib/libgtest.a pthread)
endif()
