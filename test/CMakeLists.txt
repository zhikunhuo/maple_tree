set(MODULE_ROOT_PATH ${CMAKE_SOURCE_DIR}/test)

message(STATUS "include root path")

set(CASE_FILES  ${MODULE_ROOT_PATH}/main.cpp 
                )
message(STATUS "CASE_FILES:${CASE_FILES}")
#add test source file
set(TEST_SOURCE_FILES ${CASE_FILES} PARENT_SCOPE)
