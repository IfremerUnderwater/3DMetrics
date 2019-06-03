#include "tool_area_dialog.h"
#include "ui_tool_area_dialog.h"


#include <QMouseEvent>

#include "OSGWidget/osg_widget_tool.h"
#include "OSGWidget/osg_widget.h"

#include <Measurement/measurement_area.h>

ToolAreaDialog::ToolAreaDialog(QWidget *_parent) :
    QDialog(_parent),
    ui(new Ui::ToolAreaDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    m_geode = new osg::Geode();
    m_meas_area= new MeasArea("*temporaryarea", m_geode);

    osg::Vec4f color(1.0f,0.0f,0.0f,1.0f); // red (r,g,b,a)
    m_meas_area->setColor(color);

    ui->area_widget->setArea(m_meas_area);
    OSGWidgetTool::instance()->getOSGWidget()->addGeode(m_geode);

    QObject::connect(ui->close_btn, SIGNAL(clicked(bool)), this, SLOT(close()));
    QObject::connect(ui->start_btn, SIGNAL(clicked(bool)), this, SLOT(start()));
    QObject::connect(ui->area_widget, SIGNAL(signal_toolEnded(QString&)), this, SLOT(slot_toolEnded(QString&)));
    ui->msg_label->setText("");
}

ToolAreaDialog::~ToolAreaDialog()
{
    delete ui;
}

void ToolAreaDialog::mouseDoubleClickEvent( QMouseEvent * _e )
{
    if ( _e->button() == Qt::LeftButton )
    {
        start();
    }
}

void ToolAreaDialog::closeEvent(QCloseEvent *)
{
    ui->area_widget->slot_toolEnded();
    OSGWidgetTool::instance()->getOSGWidget()->removeGeode(m_geode);
}

// ESC key hit
void ToolAreaDialog::reject()
{
    ui->area_widget->slot_toolEnded();
    OSGWidgetTool::instance()->getOSGWidget()->removeGeode(m_geode);
    close();
}

void ToolAreaDialog::start()
{
    OSGWidgetTool::instance()->slot_cancelTool();
    ui->msg_label->setText(tr("Right button to end"));
    ui->area_widget->clicked();
}

void ToolAreaDialog::slot_toolEnded(QString&)
{
    ui->msg_label->setText(tr(""));
}
