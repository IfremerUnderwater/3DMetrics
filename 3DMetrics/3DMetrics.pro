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
mac{
INCLUDEPATH += /opt/local/include/eigen3
INCLUDEPATH += /opt/local/include/
LIBS += -L/opt/local/lib
}

DESTDIR = $${RUN_DIR}

include(../Scripts/BasicProcessing.pri)

LIBS += -losgUtil -losgDB -losgViewer -losgGA -losgText -losg -lOpenThreads -lkmlbase -lkmldom -lGeographic -lgdal -losgVolume

INCLUDEPATH += Tools

SOURCES += main.cpp \
    OSGWidget/kml_handler.cpp \
    tdmgui.cpp \
    Measurement/point3d.cpp \
    Measurement/measurement_area.cpp \
    Measurement/measurement_item.cpp \
    Measurement/measurement_point.cpp \
    Measurement/measurement_line.cpp \
    Measurement/measurement_string.cpp \
    Measurement/measurement_pattern.cpp \
    Measurement/measurement_type.cpp \
    Measurement/osg_measurement_row.cpp \
    OSGWidget/osg_widget.cpp \
    TreeView/tdm_measurement_layerdata.cpp \
    TreeView/tdm_model_layerdata.cpp \
    Measurement/measurement_category.cpp \
    Measurement/tdm_categories_manager.cpp \
    decimation_dialog.cpp \
    osg_axes.cpp \
    TreeView/tdm_layer_item.cpp \
    TreeView/tdm_layer_model.cpp \
    attrib_area_widget.cpp \
    OSGWidget/osg_widget_tool.cpp \
    attrib_categories_widget.cpp \
    attrib_line_widget.cpp \
    attrib_point_widget.cpp \
    file_dialog.cpp \
    file_open_thread.cpp \
    tool_area_dialog.cpp \
    tool_line_dialog.cpp \
    tool_point_dialog.cpp \
    OSGWidget/box_visitor.cpp \
    OSGWidget/area_computation_visitor.cpp \
    meas_table_widget_item.cpp \
    edit_meas_dialog.cpp \
    edit_meas_item.cpp \
    about_dialog.cpp \
    meas_geom_export_dialog.cpp \
    edit_transparency_model.cpp \
    edit_offset_model.cpp \
    slope_tool.cpp \
    Tools/geometry.cpp \
    choose_loadingmode_dialog.cpp \
    z_scale_dialog.cpp \
    profile_depth_dialog.cpp \
    paint_widget.cpp \
    OSGWidget/measure_picker_tool.cpp \
    OSGWidget/minmax_computation_visitor.cpp \
    OSGWidget/geometry_type_count_visitor.cpp \
    model_depth_colors_chooser.cpp \
    color_widget.cpp \
    OSGWidget/shader_color.cpp \
    OSGWidget/overlay_widget.cpp

HEADERS  += \
    OSGWidget/kml_handler.h \
    tdmgui.h \
    Measurement/point3d.h \
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
    decimation_dialog.h \
    osg_axes.h \
    attrib_area_widget.h \
    attrib_categories_widget.h \
    attrib_line_widget.h \
    attrib_point_widget.h \
    file_dialog.h \
    file_open_thread.h \
    tool_area_dialog.h \
    tool_line_dialog.h \
    tool_point_dialog.h \
    OSGWidget/box_visitor.h \
    OSGWidget/area_computation_visitor.h \
    meas_table_widget_item.h \
    edit_meas_dialog.h \
    edit_meas_item.h \
    about_dialog.h \
    meas_geom_export_dialog.h \
    edit_transparency_model.h \
    edit_offset_model.h \
    slope_tool.h \
    Tools/geometry.h \
    choose_loadingmode_dialog.h \
    OSGWidget/loading_mode.h \
    z_scale_dialog.h \
    profile_depth_dialog.h \
    paint_widget.h \
    OSGWidget/measure_picker_tool.h \
    OSGWidget/minmax_computation_visitor.h \
    OSGWidget/geometry_type_count_visitor.h \
    model_depth_colors_chooser.h \
    color_widget.h \
    OSGWidget/shader_color.h \
    OSGWidget/overlay_widget.h

FORMS    += \
    tdmgui.ui \
    decimation_dialog.ui \
    attrib_area_widget.ui \
    attrib_categories_widget.ui \
    attrib_line_widget.ui \
    attrib_point_widget.ui \
    tool_area_dialog.ui \
    tool_line_dialog.ui \
    tool_point_dialog.ui \
    edit_meas_dialog.ui \
    edit_meas_item.ui \
    about_dialog.ui \
    meas_geom_export_dialog.ui \
    edit_transparency_model.ui \
    edit_offset_model.ui \
    slope_tool.ui \
    choose_loadingmode_dialog.ui \
    z_scale_dialog.ui \
    profile_depth_dialog.ui \
    model_depth_colors_chooser.ui

RESOURCES += \
    ressources.qrc

RC_ICONS = ./ressources/3dm_icon.ico
