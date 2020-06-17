# add the executable
add_executable(3DMetrics
    main.cpp
	tdmgui.cpp
    OSGWidget/kml_handler.cpp
	OSGWidget/point3d.cpp
	OSGWidget/osg_widget_tool.cpp
    OSGWidget/osg_widget.cpp
    OSGWidget/box_visitor.cpp
    OSGWidget/area_computation_visitor.cpp
    OSGWidget/measurement_picker_tool.cpp
    OSGWidget/minmax_computation_visitor.cpp
    OSGWidget/geometry_type_count_visitor.cpp
    OSGWidget/shader_color.cpp
    OSGWidget/overlay_widget.cpp
    Measurement/measurement_area.cpp
    Measurement/measurement_item.cpp
    Measurement/measurement_point.cpp
    Measurement/measurement_line.cpp
    Measurement/measurement_string.cpp
    Measurement/measurement_pattern.cpp
    Measurement/measurement_type.cpp
    Measurement/osg_measurement_row.cpp
    Measurement/measurement_category.cpp
    Measurement/tdm_categories_manager.cpp
    TreeView/tdm_measurement_layerdata.cpp
    TreeView/tdm_model_layerdata.cpp
	TreeView/tdm_layer_item.cpp
    TreeView/tdm_layer_model.cpp
    decimation_dialog.cpp
    osg_axes.cpp
    attrib_area_widget.cpp
    attrib_categories_widget.cpp
    attrib_line_widget.cpp
    attrib_point_widget.cpp
    file_dialog.cpp
    file_open_thread.cpp
    tool_area_dialog.cpp
    tool_line_dialog.cpp
    tool_point_dialog.cpp
    meas_table_widget_item.cpp
    edit_meas_dialog.cpp
    edit_meas_item.cpp
    about_dialog.cpp
    meas_geom_export_dialog.cpp
    edit_transparency_model.cpp
    edit_offset_model.cpp
    slope_tool.cpp
    Tools/geometry.cpp
    choose_loadingmode_dialog.cpp
    z_scale_dialog.cpp
    profile_depth_dialog.cpp
    paint_widget.cpp
    model_depth_colors_chooser.cpp
    color_widget.cpp
    qtable_arrowkey_detector.cpp
	ressources.qrc
	3DMetrics.rc)
	
	
set_target_properties(3DMetrics PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TDM_RUNTIME_ROOT}/$<0:>)
	
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(3DMetrics PUBLIC
     BasicProcessing
     Qt5::Core
     Qt5::Widgets
     Qt5::OpenGL
	 GDAL::GDAL
	 ${GeographicLib_LIBRARIES}
     ${OPENSCENEGRAPH_LIBRARIES}
	 kmldom kmlbase)
