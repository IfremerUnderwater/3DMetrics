#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Point>

#include "Measurement/point3d.h"
#include "Measurement/measurement_line.h"

#include "profile_depth_dialog.h"
#include "ui_profile_depth_dialog.h"


#include "OSGWidget/osg_widget.h"
#include "OSGWidget/osg_widget_tool.h"

ProfileDepthDialog::ProfileDepthDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProfileDepthDialog),
    m_measLine(nullptr)
{
    ui->setupUi(this);

    QObject::connect(ui->close_btn, SIGNAL(clicked(bool)), this, SLOT(close()));
    QObject::connect(ui->drawing_widget, SIGNAL(mousemoved(float,float)), this, SLOT(onCoordChanged(float,float)));
    QObject::connect(ui->drawing_widget, SIGNAL(mouseleaved()), this, SLOT(onWidgetLeaved()));
}

ProfileDepthDialog::~ProfileDepthDialog()
{
    delete ui;
}

void ProfileDepthDialog::setMeasLine(MeasLine *_line)
{
    m_measLine = _line;

    int n = _line->nbPts();
    if(n == 0)
        return;

    QVector<Point3D> pts = _line->getArray();
    float x0 = pts[0].x;
    float y0 = pts[0].y;
    float z0 = pts[0].z;
    float d0 = 0;

    // polyline pts
    QVector<QPointF> fpts;
    fpts.append(QPointF(0,pts[0].z));

    // model points (polyline + projeted inter pts)
    QVector<QPointF> mpts;
    mpts.append(QPointF(0,pts[0].z));

    OSGWidget *w = OSGWidgetTool::instance()->getOSGWidget();

    const int NIP = 20;

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
                mpts.append(QPointF(d0 + dn,ip[2]));
            }
        }

        d0 += d;
        fpts.append(QPointF(d0,pts[i].z));

        // last
        mpts.append(QPointF(d0,pts[i].z));

        x0 = pts[i].x;
        y0 = pts[i].y;
        z0 = pts[i].z;

    }

    ui->drawing_widget->setMainPolyLine(fpts);
    ui->drawing_widget->setModelPolyLine(mpts);
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

