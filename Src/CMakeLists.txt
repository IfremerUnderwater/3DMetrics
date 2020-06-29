cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(3DMetricsProject VERSION 0.5.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


#path to OpenSceneGraph
list(APPEND CMAKE_MODULE_PATH /home/data/ThirdPartyLibs/OpenSceneGraph-3.6.2/CMakeModules)

#path to Geographic lib
list(APPEND CMAKE_MODULE_PATH /home/data/ThirdPartyLibs/GeographicLib-1.50.1/cmake)
#set(GeographicLib_DIR /home/data/ThirdPartyLibs/GeographicLib-1.50.1/cmake)

#path to libkml
list(APPEND CMAKE_MODULE_PATH /home/data/ThirdPartyLibs/libkml-master/build)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

#list(APPEND CMAKE_MODULE_PATH /home/data/ThirdPartyLibs/eigen-3.3.7/build/cmake)
set(Eigen3_DIR /home/data/ThirdPartyLibs/eigen-3.3.7/build/cmake)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4005")
endif(MSVC)

string(TIMESTAMP BUILD_DATE "%Y-%m-%d")

# define Runtime directories as config dependent folders (and create them)
set(TDM_RUNTIME_ROOT ${PROJECT_BINARY_DIR}/Run/$<$<CONFIG:Release>:Release>$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>)

add_custom_target(copy_install ALL
 COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/../Install ${PROJECT_BINARY_DIR}/Install
 COMMENT "Copying install folder ..."
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
