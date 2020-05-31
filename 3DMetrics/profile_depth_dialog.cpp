#include <osg/Geometry>
#include <osg/StateSet>
#include "OSGWidget/osg_widget.h"

#include <osg/Point>

#include "OSGWidget/point3d.h"
#include "Measurement/measurement_line.h"

#include "profile_depth_dialog.h"
#include "ui_profile_depth_dialog.h"

#include "OSGWidget/osg_widget_tool.h"

#include <osgGA/TrackballManipulator>


ProfileDepthDialog::ProfileDepthDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProfileDepthDialog),
    m_measLine(nullptr)
{
    ui->setupUi(this);

    ui->coords_label->setText("");

    QObject::connect(ui->close_btn, SIGNAL(clicked(bool)), this, SLOT(close()));
    QObject::connect(ui->drawing_widget, SIGNAL(mousemoved(float,float)), this, SLOT(onCoordChanged(float,float)));
    QObject::connect(ui->drawing_widget, SIGNAL(mouseleaved()), this, SLOT(onWidgetLeaved()));
}

ProfileDepthDialog::~ProfileDepthDialog()
{
    delete ui;
}

void ProfileDepthDialog::setMeasLine(MeasLine *_line, bool _topview)
{
    m_measLine = _line;

    int n = _line->nbPts();
    if(n == 0)
        return;

    OSGWidget *w = OSGWidgetTool::instance()->getOSGWidget();
    osg::Camera *camera = w->getCamera();
    osg::StateAttribute::GLModeValue light = camera->getOrCreateStateSet()->getMode(GL_LIGHTING);

    QVector<Point3D> pts = _line->getArray();
    float x0 = pts[0].x;
    float y0 = pts[0].y;
    float z0 = pts[0].z;
    float d0 = 0;

    double lat, lon, alt;
    w->xyzToLatLonAlt(x0, y0,z0, lat, lon, alt);

    // model delta in Z axis
    double delta = alt - z0;

    // polyline pts
    QVector<QPointF> fpts;
    fpts.append(QPointF(0,pts[0].z+delta));

    // model points (polyline + projeted inter pts)
    QVector<QPointF> mpts;
    mpts.append(QPointF(0,pts[0].z+delta));

    // save camera parameters
    osg::Vec3 eye, center, up;
    camera->getViewMatrixAsLookAt(eye,center,up);
    osg::Matrixd mat = w->getView()->getCameraManipulator()->getMatrix();

    osg::Vec3 drawUp = up;
    // set camera to top view
    if(_topview)
    {
        w->home();
        w->getView()->requestRedraw();
        w->frame();

        drawUp= {0.0, 1.0, 0.0};
    }

    QString upstr = "Orientation : ";
    upstr +=  QString::number(drawUp[0] ,'f',3);
    upstr += " ; ";
    upstr +=  QString::number(drawUp[1] ,'f',3);
    upstr += " ; ";
    upstr +=  QString::number(drawUp[2] ,'f',3);
    ui->orientation_label->setText(upstr);

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // compure # of intermediary points
    int NIP = 20;
    switch(n)
    {
    case 2:
        NIP = 100;
        break;
    case 3:
        NIP = 50;
        break;
    case 4:
        NIP = 40;
        break;
    case 5:
        NIP = 35;
        break;
    case 6:
        NIP = 30;
        break;
    case 7:
        NIP = 25;
        break;
    }

    for(int i=1; i<n; i++)
    {
        float d = hypot(pts[i].x - x0, pts[i].y - y0);

        // inter pts
        for(int n=1; n<NIP-1; n++)
        {
            osg::Vec3d wp;
            wp[0] =  x0 + (pts[i].x - x0) * n / NIP;
            wp[1] =  y0 + (pts[i].y - y0) * n / NIP;
            wp[2] =  z0 + (pts[i].z - z0) * n / NIP;

            osg::Vec3d ip;
            bool exists;

            w->getIntersectionPoint(wp,ip, exists);

            if(exists)
            {
                float dn = n * (d / NIP);
                mpts.append(QPointF(d0 + dn,ip[2]+delta));
            }
        }

        d0 += d;
        fpts.append(QPointF(d0,pts[i].z+delta));

        // last
        mpts.append(QPointF(d0,pts[i].z+delta));

        x0 = pts[i].x;
        y0 = pts[i].y;
        z0 = pts[i].z;
    }

    ui->drawing_widget->setMainPolyLine(fpts);
    ui->drawing_widget->setModelPolyLine(mpts);
    ui->drawing_widget->buildGraph();

    // restore camera
    if(_topview)
    {
        camera->setViewMatrixAsLookAt(eye,center,up);
        w->getView()->setCamera(camera);
        w->getView()->getCameraManipulator()->setByMatrix(mat);
        w->getView()->requestRedraw();
        camera->getOrCreateStateSet()->setMode(GL_LIGHTING, light);
    }

    QApplication::restoreOverrideCursor();
}

void ProfileDepthDialog::onCoordChanged(float _d, float _z)
{
    QString txt = QString::number(_d ,'f',1);
    txt += " m    ";
    txt += QString::number(_z ,'f',1);
    txt += " m";
    ui->coords_label->setText(txt);
}

void ProfileDepthDialog::onWidgetLeaved()
{
    ui->coords_label->setText("");
}

