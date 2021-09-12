set(MODULE_ROOT_PATH ${CMAKE_SOURCE_DIR}/src)

message(STATUS "include root path")

set(SOURCE_FILES  ${MODULE_ROOT_PATH}/maple_tree/src/maple_tree_utils.cpp
                ${MODULE_ROOT_PATH}/maple_tree/src/maple_tree_node.cpp	
                ${MODULE_ROOT_PATH}/maple_tree/src/maple_tree.cpp	
		${MODULE_ROOT_PATH}/maple_tree/src/maple_tree_state.cpp	
                )
message(STATUS "SOURCE_FILES:${SOURCE_FILES}")
#add test source file
set(SOURCE_FILES_LIST ${SOURCE_FILES_LIST} ${SOURCE_FILES} PARENT_SCOPE)
set(SOURCE_INCLUDE_PATH "${MODULE_ROOT_PATH}/maple_tree/include"  PARENT_SCOPE)
message(STATUS "SOURCE_INCLUDE_PATH:${SOURCE_INCLUDE_PATH}")
