#ifndef MEASUREMENTTOOL_H
#define MEASUREMENTTOOL_H

#include <osg/Vec3d>

#include <QDebug>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QPair>

#include <osg/Camera>

#include <osg/DisplaySettings>
#include <osg/Geode>
#include <osg/Material>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/StateSet>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgGA/EventQueue>
#include <osgGA/TrackballManipulator>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>
#include <osgUtil/Optimizer>

#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/GUIEventAdapter>

#include <cassert>

#include <stdexcept>
#include <vector>

#include <QDebug>


#include <osg/Referenced>
#include <osg/LineSegment>
#include <osg/Geometry>

#include <vector>
#include <map>

#include "tool_types.h"

class ToolHandler;

class MeasurementTool
{
public:
    MeasurementTool(ToolHandler *_tool_handler);
    virtual ~MeasurementTool();

    ///
    /// \brief pushNewPoint add a new point draw the measurement
    /// \param _point 3D point to add
    ///
    void pushNewPoint(osg::Vec3d _point);

    ///
    /// \brief draw measurement drawing
    ///
    virtual void draw()=0;


    ///
    /// \brief removeLastMeasurement remove last measurement
    ///
    virtual void removeLastMeasurement()=0;

    ///
    /// \brief removeLastMeasurement cancel current measurement
    ///
    virtual void cancelMeasurement()=0;

    ///
    /// \brief removeMeasurement remove measurement of index _meas_index
    /// \param _meas_index
    ///
    virtual void removeMeasurement(int _meas_index)=0;

    ///
    /// \brief getMeasType
    /// \return measurement type
    ///
    ToolState getMeasType();

    ///
    /// \brief getMeasTypeAndIndex
    /// \return
    ///
    QPair<ToolState,int> getMeasTypeAndIndex();

    ///
    /// \brief closeLoop add first point as the last point to close the loop
    ///
    void closeLoop();

    ///
    /// \brief getTextFormattedResult
    /// \return formatted result as a qstring
    ///
    virtual QString getTextFormattedResult()=0;


    // hide/show measurement method
    void hideShowMeasurement(int _meas_index, bool _visible);

    void resetMeasData();

    virtual void endMeasurement();

    // draw a point with given color in the provided geode
    void drawPoint(osg::Vec3d &_point, osg::Vec4 &_color, QString _point_name);
    // Join last picked point with the previous one
    void drawJunctionLineWithLastPoint(QString &line_name);

    void setMeasurementGeode(osg::ref_ptr<osg::Geode> _measurement_geode);

    virtual void onMousePress(Qt::MouseButton _button, int _x, int _y){Q_UNUSED(_button);Q_UNUSED(_x);Q_UNUSED(_y);}

protected:

    // Measurement points (used to draw points and lines)
    osg::ref_ptr<osg::Vec3dArray> m_measurement_pt;

    // Map to keep a trace of drawable (in order to be able to remove or hide them)
    QMap<QString, osg::ref_ptr<osg::Drawable> > m_geo_drawable_map;

    // Map to keep history of measurements pt
    QMap<int, osg::ref_ptr<osg::Vec3dArray> > m_measurements_pt_qmap;

    // Tool handler is in interaction with display widget
    ToolHandler *m_tool_handler;

    // Incremental measurement index
    int m_last_meas_idx;

    ToolState m_meas_type;

    // Geode that stores all measurement drawables
    osg::ref_ptr<osg::Geode> m_measurement_geode;




};

#endif // MEASUREMENTTOOL_H
