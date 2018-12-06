#ifndef OSGWIDGETTOOL_H
#define OSGWIDGETTOOL_H

#include <QObject>

#include "Measurement/point3d.h"

class OSGWidget;

class OSGWidgetTool : public QObject
{
    Q_OBJECT
public:
    enum type {
        None,
        Point,  // Point : one point clicked = end of tool
        Line,   // polyline : multiple points
        Area    // polygon : id + closure of polyline -> polygon
    };

    // singleton
    static OSGWidgetTool *instance() { return s_instance; }
    // must be initialized
    static void initialize(OSGWidget *_osgwidget);

    OSGWidget * getOSGWidget() const { return m_osgWidget; }

    ~OSGWidgetTool();

    void startTool(const type _type);
    void endTool();

signals:
    void signal_clicked(Point3D &p);
    void signal_endTool(); // to be used to remove connections
    void signal_cancelTool();

public slots:
    void slot_mouseButtonDown(Qt::MouseButton _button, int _x, int _y);
    void slot_cancelTool();

private:
    // singleton
    OSGWidgetTool();

    static OSGWidgetTool *s_instance;

    OSGWidget *m_osgWidget;
    type m_currentType;
};

#endif // OSGWIDGETTOOL_H
