#ifndef MEASUREMENTTOOL_H
#define MEASUREMENTTOOL_H

#include <osg/Vec3d>

#include <QDebug>
#include <QKeyEvent>
#include <QWheelEvent>

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

class ToolHandler;

class MeasurementTool
{
public:
    MeasurementTool(ToolHandler *_tool_handler);
    virtual ~MeasurementTool();
    void pushNewPoint(osg::Vec3d _point);
    virtual void draw()=0;
    virtual void removeLastMeasurement()=0;
    virtual QString getTypeOfMeasur()=0;
    virtual void removeMeasurement(int _meas_index)=0;
    void closeLoop();
    int getNumberOfPoints();

    // hide/show measurement method
    void hideShowMeasurement(int _meas_index, bool _visible);
    void resetModelData();
    void endMeasurement();

    // get points and lines coordinates
    QMap<int, osg::ref_ptr<osg::Vec3dArray> > getMeasurementsHistoryQmap();

    // get informations about points and lines
    QMap<QString, osg::ref_ptr<osg::Drawable> > getPointQmap() const;

    QMap<int,int> getMeasurPtsNumber();
    QMap<int,int> getMeasurLinesNumber();

    // draw a point with given color in the provided geode
    void drawPoint(osg::Vec3d &_point, osg::Vec4 &_color, QString _point_name);
    // Join last picked point with the previous one
    void drawJunctionLineWithLastPoint(QString &line_name);

    void setMeasurementGeode(osg::ref_ptr<osg::Geode> _measurement_geode);

protected:

    virtual void onMousePress(Qt::MouseButton _button, int _x, int _y){Q_UNUSED(_button);Q_UNUSED(_x);Q_UNUSED(_y);}

    // Measurement points (used to draw points and lines)
    osg::ref_ptr<osg::Vec3dArray> m_measurement_pt;

    // Map to keep a trace of drawable (in order to be able to remove or hide them)
    QMap<QString, osg::ref_ptr<osg::Drawable> > m_geo_drawable_map;

    // Map to keep history of measurements pt
    QMap<int, osg::ref_ptr<osg::Vec3dArray> > m_measurements_pt_qmap;

    // Map that stores pts & lines number in each measurement
    QMap<int,int> m_meas_points_number;
    QMap<int,int> m_meas_lines_number;

    int m_measurement_counter;

    int m_lines_counter;

    QString m_measur_type;

    // Geode that stores all measurement drawables
    osg::ref_ptr<osg::Geode> m_measurement_geode;

    // Tool handler is in interaction with display widget
    ToolHandler *m_tool_handler;


};

#endif // MEASUREMENTTOOL_H
