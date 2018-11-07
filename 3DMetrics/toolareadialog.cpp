#include "toolareadialog.h"
#include "ui_toolareadialog.h"


#include <QMouseEvent>

#include "OSGWidget/osgwidgettool.h"
#include "OSGWidget/OSGWidget.h"

#include <Measure/measurearea.h>

ToolAreaDialog::ToolAreaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolAreaDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    m_geode = new osg::Geode();
    m_measurearea= new MeasureArea("*temporaryarea", m_geode);

    osg::Vec4f color(1.0f,0.0f,0.0f,1.0f); // red (r,g,b,a)
    m_measurearea->setColor(color);

    ui->areawidget->setArea(m_measurearea);
    OSGWidgetTool::instance()->getOSGWidget()->addGeode(m_geode);

    QObject::connect(ui->close_btn, SIGNAL(clicked(bool)), this, SLOT(close()));
    QObject::connect(ui->start_btn, SIGNAL(clicked(bool)), this, SLOT(start()));
    QObject::connect(ui->areawidget, SIGNAL(signal_toolEnded(QString&)), this, SLOT(slot_toolEnded(QString&)));
    ui->msg_label->setText("");
}

ToolAreaDialog::~ToolAreaDialog()
{
    delete ui;
}

void ToolAreaDialog::mouseDoubleClickEvent( QMouseEvent * e )
{
    if ( e->button() == Qt::LeftButton )
    {
        start();
    }
}

void ToolAreaDialog::closeEvent(QCloseEvent *)
{
    ui->areawidget->slot_toolEnded();
    OSGWidgetTool::instance()->getOSGWidget()->removeGeode(m_geode);
}

// ESC key hit
void ToolAreaDialog::reject()
{
    ui->areawidget->slot_toolEnded();
    OSGWidgetTool::instance()->getOSGWidget()->removeGeode(m_geode);
    close();
}

void ToolAreaDialog::start()
{
    OSGWidgetTool::instance()->slot_cancelTool();
    ui->msg_label->setText(tr("Right button to end"));
    ui->areawidget->clicked();
}

void ToolAreaDialog::slot_toolEnded(QString&)
{
    ui->msg_label->setText(tr(""));
}
