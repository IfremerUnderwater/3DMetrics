#include "attribpointwidget.h"
#include "ui_attribpointwidget.h"

#include "Measure/measurepoint.h"

#include "OSGWidget/osgwidgettool.h"
#include "OSGWidget/OSGWidget.h"

#include <GeographicLib/LocalCartesian.hpp>

AttribPointWidget::AttribPointWidget(QWidget *parent) :
    QWidget(parent),
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

void AttribPointWidget::update(bool valueOk)
{
    if(m_item && valueOk)
    {
        OSGWidgetTool *tool = OSGWidgetTool::instance();
        OSGWidget *osgWidget = tool->getOSGWidget();

        double lat, lon, depth;

        // transform to lat/lon
        // *** TODO : put in OSGWidget (reference doesn't change often)
        QPointF ref_lat_lon; double ref_depth;
        osgWidget->getGeoOrigin(ref_lat_lon, ref_depth);
        GeographicLib::LocalCartesian ltp_proj;
        ltp_proj.Reset(ref_lat_lon.x(), ref_lat_lon.y(), ref_depth);
        ltp_proj.Reverse(m_item->x(), m_item->y(), m_item->z(), lat, lon, depth);

        ui->x_label->setText(QString::number(lat,'f',7));
        ui->y_label->setText(QString::number(lon,'f',7));
        ui->z_label->setText(QString::number(depth,'f',1));

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

void AttribPointWidget::slot_toolClicked(Point3D &p)
{
    m_item->setP(p);
    update();
    m_item->updateGeode();

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
