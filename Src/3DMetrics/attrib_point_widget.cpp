#include "OSGWidget/osg_widget.h"

#include "attrib_point_widget.h"
#include "ui_attrib_point_widget.h"

#include "Measurement/measurement_point.h"

#include "OSGWidget/osg_widget_tool.h"

#include <GeographicLib/LocalCartesian.hpp>

AttribPointWidget::AttribPointWidget(QWidget *_parent) :
    QWidget(_parent),
    ui(new Ui::AttribPointWidget)
{
    ui->setupUi(this);

    ui->tool_label->setStyleSheet("background-color: red");
}

AttribPointWidget::~AttribPointWidget()
{
    delete ui;

    // just in case
    slot_toolEnded();
}

void AttribPointWidget::clicked()
{
    // start tool
    QString msg = "Point tool started";
    emit signal_toolStarted(msg);

    OSGWidgetTool *tool = OSGWidgetTool::instance();
    connect(tool, SIGNAL(signal_clicked(Point3D&)), this, SLOT(slot_toolClicked(Point3D&)));
    connect(tool, SIGNAL(signal_endTool()), this, SLOT(slot_toolEnded()));

    tool->startTool(OSGWidgetTool::Point);
}

void AttribPointWidget::update(bool _value_ok)
{
    if(m_point_item && _value_ok)
    {
        OSGWidgetTool *tool = OSGWidgetTool::instance();
        OSGWidget *osg_widget = tool->getOSGWidget();

        double lat=0, lon=0, alt=0;

        // transform to lat/lon
        //        // *** TODO : put in OSGWidget (reference doesn't change often)
        //        QPointF ref_lat_lon; double ref_alt;
        //        osgWidget->getGeoOrigin(ref_lat_lon, ref_alt);
        //        GeographicLib::LocalCartesian ltp_proj;
        //        ltp_proj.Reset(ref_lat_lon.x(), ref_lat_lon.y(), ref_alt);
        //        ltp_proj.Reverse(m_item->x(), m_item->y(), m_item->z(), lat, lon, alt);
        osg_widget->xyzToLatLonAlt(m_point_item->x(), m_point_item->y(), m_point_item->z(), lat, lon, alt);

        ui->x_label->setText(QString::number(fabs(lat),'f',7) + (lat >= 0 ? "N" : "S"));
        ui->y_label->setText(QString::number(fabs(lon),'f',7) + (lon >= 0 ? "E" : "W"));
        ui->z_label->setText(QString::number(alt,'f',1) + " m");

        ui->tool_label->setStyleSheet("");
    }
    else
    {
        ui->x_label->setText("");
        ui->y_label->setText("");
        ui->z_label->setText("");

        ui->tool_label->setStyleSheet("background-color: red");
    }
}

void AttribPointWidget::slot_toolClicked(Point3D &_point)
{
    m_point_item->setP(_point);
    update();
    m_point_item->updateGeode();

    // only one point for "point"
    slot_toolEnded();
}

void AttribPointWidget::slot_toolEnded()
{
    disconnect(OSGWidgetTool::instance(), 0, this, 0);
    OSGWidgetTool::instance()->endTool();

    QString msg = "Point tool ended";
    emit signal_toolEnded(msg);
}
