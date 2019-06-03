#include "tool_point_dialog.h"
#include "ui_tool_point_dialog.h"

#include <QMouseEvent>

#include "OSGWidget/osg_widget_tool.h"
#include "OSGWidget/osg_widget.h"

#include <Measurement/measurement_point.h>

ToolPointDialog::ToolPointDialog(QWidget *_parent) :
    QDialog(_parent),
    ui(new Ui::ToolPointDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    m_geode = new osg::Geode();
    m_meas_point = new MeasPoint("*temporarypoint", m_geode);

    osg::Vec4f color(1.0f,0.0f,0.0f,1.0f); // red (r,g,b,a)
    m_meas_point->setColor(color);

    ui->point_widget->setPoint(m_meas_point,false);
    OSGWidgetTool::instance()->getOSGWidget()->addGeode(m_geode);

    QObject::connect(ui->close_btn, SIGNAL(clicked(bool)), this, SLOT(close()));
    QObject::connect(ui->start_btn, SIGNAL(clicked(bool)), this, SLOT(start()));
}

ToolPointDialog::~ToolPointDialog()
{
    delete ui;
}


void ToolPointDialog::mouseDoubleClickEvent( QMouseEvent * _e )
{
    if ( _e->button() == Qt::LeftButton )
    {
        start();
    }
}

void ToolPointDialog::closeEvent(QCloseEvent *)
{
    ui->point_widget->slot_toolEnded();
    OSGWidgetTool::instance()->getOSGWidget()->removeGeode(m_geode);
}

// ESC key hit
void ToolPointDialog::reject()
{
    ui->point_widget->slot_toolEnded();
    OSGWidgetTool::instance()->getOSGWidget()->removeGeode(m_geode);
    close();
}

void ToolPointDialog::start()
{
    OSGWidgetTool::instance()->slot_cancelTool();

    ui->point_widget->clicked();
}
