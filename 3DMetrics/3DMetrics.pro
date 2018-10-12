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

win32{
INCLUDEPATH += C:\msys64\mingw64\include\eigen3
}
unix{
INCLUDEPATH += /usr/local/include/openMVG/third_party/eigen
}

include(../Scripts/BasicProcessing.pri)

LIBS += -losgUtil -losgDB -losgViewer -losgGA -losgText -losg -lOpenThreads -lkmlbase -lkmldom -lGeographic

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
    Tools/tool_handler.cpp \
    TDMLayers/tdmlayer.cpp \
    TDMLayers/tdmmaplayerregistry.cpp \
    tdmgui.cpp \
    TreeView/tdmlayeritem.cpp \
    TreeView/tdmlayersmodel.cpp \
    filedialog.cpp \
    TreeView/tdmmodellayerdata.cpp \
    TreeView/tdmmeasurelayerdata.cpp \
    edit_measure_dialog.cpp \
    edit_measure_item.cpp \
    Measure/measuretype.cpp \
    Measure/measurepattern.cpp \
    attribpointwidget.cpp \
    attriblinewidget.cpp \
    attribareawidget.cpp \
    Measure/measurepoint.cpp \
    Measure/measurestring.cpp \
    Measure/measureitem.cpp \
    Measure/point3d.cpp \
    Measure/measureline.cpp \
    Measure/measurearea.cpp \
    measuretablewidgetitem.cpp \
    Measure/osgmeasurerow.cpp \
    OSGWidget/osgwidgettool.cpp

HEADERS  += mainwindow.h \
    OSGWidget/OSGWidget.h \
    Tools/line_measurement_tool.h \
    Tools/measurement_tool.h \
    measurement_saving_dialog.h \
    Tools/surface_measurement_tool.h \
    Tools/interest_point_tool.h \
    OSGWidget/kml_handler.h \
    Tools/tool_handler.h \
    Tools/tool_types.h \
    TDMLayers/tdmlayer.h \
    TDMLayers/tdmmaplayerregistry.h \
    tdmgui.h \
    TreeView/tdmlayeritem.h \
    TreeView/tdmlayersmodel.h \
    filedialog.h \
    TreeView/tdmmodellayerdata.h \
    TreeView/tdmmeasurelayerdata.h \
    edit_measure_dialog.h \
    edit_measure_item.h \
    Measure/measuretype.h \
    Measure/measurepattern.h \
    attribpointwidget.h \
    attriblinewidget.h \
    attribareawidget.h \
    Measure/measurepoint.h \
    Measure/measurestring.h \
    Measure/measureitem.h \
    Measure/point3d.h \
    Measure/measureline.h \
    Measure/measurearea.h \
    measuretablewidgetitem.h \
    Measure/osgmeasurerow.h \
    OSGWidget/osgwidgettool.h

FORMS    += \
    mainwindow.ui \
    measurement_saving_dialog.ui \
    tdmgui.ui \
    edit_measure_dialog.ui \
    edit_measure_item.ui \
    attribpointwidget.ui \
    attriblinewidget.ui \
    attribareawidget.ui

RESOURCES += \
    ressources.qrc
