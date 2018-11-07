#include "toolpointdialog.h"
#include "ui_toolpointdialog.h"

#include <QMouseEvent>

#include "OSGWidget/osgwidgettool.h"
#include "OSGWidget/OSGWidget.h"

#include <Measure/measurepoint.h>

ToolPointDialog::ToolPointDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolPointDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    m_geode = new osg::Geode();
    m_measurepoint = new MeasurePoint("*temporarypoint", m_geode);

    osg::Vec4f color(1.0f,0.0f,0.0f,1.0f); // red (r,g,b,a)
    m_measurepoint->setColor(color);

    ui->pointwidget->setPoint(m_measurepoint,false);
    OSGWidgetTool::instance()->getOSGWidget()->addGeode(m_geode);

    QObject::connect(ui->close_btn, SIGNAL(clicked(bool)), this, SLOT(close()));
    QObject::connect(ui->start_btn, SIGNAL(clicked(bool)), this, SLOT(start()));
}

ToolPointDialog::~ToolPointDialog()
{
    delete ui;
}


void ToolPointDialog::mouseDoubleClickEvent( QMouseEvent * e )
{
    if ( e->button() == Qt::LeftButton )
    {
        start();
    }
}

void ToolPointDialog::closeEvent(QCloseEvent *)
{
    ui->pointwidget->slot_toolEnded();
    OSGWidgetTool::instance()->getOSGWidget()->removeGeode(m_geode);
}

// ESC key hit
void ToolPointDialog::reject()
{
    ui->pointwidget->slot_toolEnded();
    OSGWidgetTool::instance()->getOSGWidget()->removeGeode(m_geode);
    close();
}

void ToolPointDialog::start()
{
    OSGWidgetTool::instance()->slot_cancelTool();

    ui->pointwidget->clicked();
}
