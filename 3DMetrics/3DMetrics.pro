#-------------------------------------------------
#
# Project created by QtCreator 2016-06-13T10:17:56
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


DEFINES += WITH_OSG

QMAKE_CXXFLAGS += -std=c++11

TARGET = 3DMetrics
TEMPLATE = app


# Workaround to be removed in qt5 with qmake.conf and shadowed function --
SOURCE_DIR=$$PWD/../
CONFIG(debug, debug|release) {
    BUILD_DIR=$${SOURCE_DIR}../Build/Debug
    RUN_DIR=$${SOURCE_DIR}../Run/Debug
}
CONFIG(release, debug|release) {
    BUILD_DIR=$${SOURCE_DIR}../Build/Release
    RUN_DIR=$${SOURCE_DIR}../Run/Release
}

INCLUDEPATH += /usr/local/include/openMVG/third_party/eigen
include(../Scripts/BasicProcessing.pri)

LIBS += -losgUtil -losgDB -losgViewer -losgGA -losgText -losg -lkmlbase -lkmldom -lGeographic
INCLUDEPATH += Tools

SOURCES += main.cpp \
    mainwindow.cpp \
    OSGWidget/OSGWidget.cpp \
    Tools/line_measurement_tool.cpp \
    Tools/measurement_tool.cpp \
    measurement_saving_dialog.cpp \
    Tools/surface_measurement_tool.cpp \
    Tools/interest_point_tool.cpp \
    OSGWidget/kml_handler.cpp \
    Tools/tool_handler.cpp

HEADERS  += mainwindow.h \
    OSGWidget/OSGWidget.h \
    Tools/line_measurement_tool.h \
    Tools/measurement_tool.h \
    measurement_saving_dialog.h \
    Tools/surface_measurement_tool.h \
    Tools/interest_point_tool.h \
    OSGWidget/kml_handler.h \
    Tools/tool_handler.h \
    Tools/tool_types.h

FORMS    += \
    mainwindow.ui \
    measurement_saving_dialog.ui

RESOURCES += \
    ressources.qrc
