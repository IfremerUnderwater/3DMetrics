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


class MeasurementTool
{
public:
    MeasurementTool();
    virtual ~MeasurementTool();
    void pushNewPoint(osg::ref_ptr<osg::Geode> &_measurement_geode, osg::Vec3d _point);
    virtual void draw(osg::ref_ptr<osg::Geode> &_measurement_geode)=0;
    virtual void removeLastMeasurement(osg::ref_ptr<osg::Geode> &_measurement_geode)=0;
    virtual QString getTypeOfMeasur()=0;
    virtual void removeMeasurement(osg::ref_ptr<osg::Geode> &_measurement_geode, int _meas_index)=0;
    void closeLoop(osg::ref_ptr<osg::Geode> &_measurement_geode);
    int getNumberOfPoints();

    // hide/show measurement method
    void hideShowMeasurement(osg::ref_ptr<osg::Geode> &_measurement_geode, int _meas_index, bool _visible);
    void resetModelData();
    void endMeasurement();

    // get points and lines coordinates
    QMap<int, osg::ref_ptr<osg::Vec3dArray> > getMeasurementsHistoryQmap();

    // get informations about points and lines
    QMap<QString, osg::ref_ptr<osg::Drawable> > getPointQmap() const;

    QMap<int,int> getMeasurPtsNumber();
    QMap<int,int> getMeasurLinesNumber();

    // draw a point with given color in the provided geode
    void drawPoint(osg::ref_ptr<osg::Geode> &_measurement_geode, osg::Vec3d &_point, osg::Vec4 &_color, QString _point_name);
    // Join last picked point with the previous one
    void drawJunctionLineWithLastPoint(osg::ref_ptr<osg::Geode> &_measurement_geode, QString &line_name);

protected:
    osg::ref_ptr<osg::Vec3dArray> m_measurement_pt;
    QMap<int, osg::ref_ptr<osg::Vec3dArray> > m_measurements_history_qmap;
    int m_measurement_counter;

    // contains points and lines
    QMap<QString, osg::ref_ptr<osg::Drawable> > m_geo_drawable_map;

    //use to delete or create measurements
    QMap<int,QString> m_measur_qmap;

    QMap<int,int> m_meas_points_number;
    QMap<int,int> m_meas_lines_number;
    int m_lines_counter;

    QString m_measur_type;


};

#endif // MEASUREMENTTOOL_H
