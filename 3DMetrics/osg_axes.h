#ifndef OSGAXES_H
#define OSGAXES_H

#include <QWidget>
#include <osg/Geode>
#include "Measurement/point3d.h"


class OSGAxes : public QWidget
{
    Q_OBJECT

public:
    explicit OSGAxes(QWidget *parent = 0);
    ~OSGAxes();

    void clicked();
    double x() const { return m_p.x; }
    void setX(double _x) { m_p.x = _x; }

    double y() const { return m_p.y; }
    void setY(double _y) { m_p.y = _y; }

    double z() const { return m_p.z; }
    void setZ(double _z) { m_p.z = _z; }

    Point3D p() const { return m_p; }
    void setP(Point3D _p) { m_p = _p; }


    Point3D m_p;
    osg::Vec4f m_colorGreen;
    osg::ref_ptr<osg::Geode> m_geode;

    osg::ref_ptr<osg::Geode> geode() const { return m_geode; }
    void mouseDoubleClickEvent( QMouseEvent * e );
    void start();
    void show();
    void removeAxe();

public slots:

    void slot_toolEndedAxes();
    void slot_toolClickedAxes(Point3D &p);

signals:
     void signal_toolStarted(QString &info);
private:

};

#endif // OSGAXES_H
