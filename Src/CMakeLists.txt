cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(3DMetricsProject VERSION 1.0.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

INCLUDE_DIRECTORIES(/opt/local/include)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

find_package (Eigen3 3.3 REQUIRED NO_MODULE)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4005")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif(MSVC)

string(TIMESTAMP BUILD_DATE "%Y-%m-%d")

configure_file(3DMetricsConfig.h.in 3DMetricsConfig.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# define Runtime directories as config dependent folders (and create them)
set(TDM_RUNTIME_ROOT ${PROJECT_BINARY_DIR}/Run/$<$<CONFIG:Release>:Release>$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>)

add_custom_target(copy_install ALL
 COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/../Install ${PROJECT_BINARY_DIR}/Install
 COMMENT "Copying install folder ..."
)

add_custom_target(copy_styles ALL
 COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/../Styles ${TDM_RUNTIME_ROOT}/Styles
 COMMENT "Copying styles folder ..."
)

add_custom_target(copy_help ALL
 COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/../Help ${TDM_RUNTIME_ROOT}/Help
 COMMENT "Copying help folder ..."
)

configure_file(../Install/generate_installer.bat.in Install/generate_installer.bat)
configure_file(../Install/packages/3DMetrics/meta/package.xml.in Install/packages/3DMetrics/meta/package.xml)
set(BINARYTOOL_HOMEDIR @HomeDir@)
configure_file(../Install/config/config.xml.in Install/config/config.xml)

find_package(Qt5 COMPONENTS Core Widgets OpenGL REQUIRED)

find_package(GDAL REQUIRED)

find_package(GeographicLib REQUIRED)

find_package(OpenSceneGraph REQUIRED osgDB osgUtil osgGA osgViewer)
# libOpenThreads & libosg automatically searched

find_package(LibKML REQUIRED)

##X11
#find_package(X11 REQUIRED)
#link_libraries(${X11_LIBRARIES})
#include_directories(${X11_INCLUDE_DIR})

# qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

add_subdirectory(Libraries)
add_subdirectory(3DMetrics)
add_subdirectory(Tools)
