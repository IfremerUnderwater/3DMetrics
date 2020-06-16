cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(3DMetricsProject VERSION 1.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4005")
endif(MSVC)

# define Runtime directories as config dependent folders (and create them)
set(TDM_RUNTIME_ROOT ${PROJECT_BINARY_DIR}/Run/$<$<CONFIG:Release>:Release>$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>)

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