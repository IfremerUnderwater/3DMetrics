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
INCLUDEPATH += /usr/include/eigen3
}

DESTDIR = $${RUN_DIR}

include(../Scripts/BasicProcessing.pri)

LIBS += -losgUtil -losgDB -losgViewer -losgGA -losgText -losg -lOpenThreads -lkmlbase -lkmldom -lGeographic

INCLUDEPATH += Tools

SOURCES += main.cpp \
    OSGWidget/kml_handler.cpp \
    tdmgui.cpp \
    filedialog.cpp \
    edit_measure_dialog.cpp \
    edit_measure_item.cpp \
    attribpointwidget.cpp \
    attriblinewidget.cpp \
    attribareawidget.cpp \
    Measurement/point3d.cpp \
    measuretablewidgetitem.cpp \
    OSGWidget/osgwidgettool.cpp \
    toolpointdialog.cpp \
    toollinedialog.cpp \
    toolareadialog.cpp \
    Measurement/measurement_area.cpp \
    Measurement/measurement_item.cpp \
    Measurement/measurement_point.cpp \
    Measurement/measurement_line.cpp \
    Measurement/measurement_string.cpp \
    Measurement/measurement_pattern.cpp \
    Measurement/measurement_type.cpp \
    Measurement/osg_measurement_row.cpp \
    OSGWidget/osg_widget.cpp \
    TreeView/tdmlayer_item.cpp \
    TreeView/tdmlayer_model.cpp \
    TreeView/tdm_measurement_layerdata.cpp \
    TreeView/tdm_model_layerdata.cpp \
    Measurement/measurement_category.cpp \
    Measurement/tdm_categories_manager.cpp \
    attribcategorieswidget.cpp \
    decimation_dialog.cpp

HEADERS  += \
    OSGWidget/kml_handler.h \
    tdmgui.h \
    filedialog.h \
    edit_measure_dialog.h \
    edit_measure_item.h \
    attribpointwidget.h \
    attriblinewidget.h \
    attribareawidget.h \
    Measurement/point3d.h \
    measuretablewidgetitem.h \
    toolpointdialog.h \
    toollinedialog.h \
    toolareadialog.h \
    Measurement/measurement_area.h \
    Measurement/measurement_line.h \
    Measurement/measurement_item.h \
    Measurement/measurement_pattern.h \
    Measurement/measurement_point.h \
    Measurement/measurement_string.h \
    Measurement/measurement_type.h \
    Measurement/osg_measurement_row.h \
    OSGWidget/osg_widget.h \
    OSGWidget/osg_widget_tool.h \
    TreeView/tdm_layer_item.h \
    TreeView/tdm_layer_model.h \
    TreeView/tdm_measurement_layerdata.h \
    TreeView/tdm_model_layerdata.h \
    Measurement/measurement_category.h \
    Measurement/tdm_categories_manager.h \
    attribcategorieswidget.h \
    decimation_dialog.h

FORMS    += \
    tdmgui.ui \
    edit_measure_dialog.ui \
    edit_measure_item.ui \
    attribpointwidget.ui \
    attriblinewidget.ui \
    attribareawidget.ui \
    toolpointdialog.ui \
    toollinedialog.ui \
    toolareadialog.ui \
    attribcategorieswidget.ui \
    decimation_dialog.ui

RESOURCES += \
    ressources.qrc
