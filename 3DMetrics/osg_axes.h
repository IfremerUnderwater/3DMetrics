#ifndef OSG_AXES_H
#define OSG_AXES_H

#include <QWidget>
#include <osg/Geode>
#include "Measurement/point3d.h"


class OSGAxes : public QWidget
{
    Q_OBJECT

public:
    explicit OSGAxes(QWidget *_parent = 0);
    ~OSGAxes();

    void clicked();
    double x() const { return m_point.x; }
    void setX(double _x) { m_point.x = _x; }

    double y() const { return m_point.y; }
    void setY(double _y) { m_point.y = _y; }

    double z() const { return m_point.z; }
    void setZ(double _z) { m_point.z = _z; }

    Point3D point() const { return m_point; }
    void setP(Point3D _point) { m_point = _point; }

    osg::ref_ptr<osg::Geode> geode() const { return m_geode; }

    double getScale() const { return m_scale; }
    void setScale(double _scale) { m_scale = _scale; }

    void mouseDoubleClickEvent( QMouseEvent * _e );
    void start();
    void show();
    void removeAxe();

public slots:

    void slot_toolEndedAxes();
    void slot_toolClickedAxes(Point3D &_point);

signals:
     void signal_toolStarted(QString &_info);
private:
    Point3D m_point;
    osg::ref_ptr<osg::Geode> m_geode;
    double m_scale;
};

#endif // OSG_AXES_H
