# add the executable
add_executable(osgconv
    OrientationConverter.cpp 
    osgconv.cpp)
	
	
set_target_properties(osgconv PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TDM_RUNTIME_ROOT}/$<0:>)
	
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

target_link_libraries(osgconv PRIVATE
     ${OPENSCENEGRAPH_LIBRARIES})
