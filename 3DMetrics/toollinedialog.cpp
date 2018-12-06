#include "toollinedialog.h"
#include "ui_toollinedialog.h"

#include <QMouseEvent>

#include "OSGWidget/osg_widget_tool.h"
#include "OSGWidget/osg_widget.h"

#include <Measurement/measurement_line.h>

ToolLineDialog::ToolLineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolLineDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    m_geode = new osg::Geode();
    m_measureline = new MeasureLine("*temporaryline", m_geode);

    osg::Vec4f color(1.0f,0.0f,0.0f,1.0f); // red (r,g,b,a)
    m_measureline->setColor(color);

    ui->linewidget->setLine(m_measureline);
    OSGWidgetTool::instance()->getOSGWidget()->addGeode(m_geode);

    QObject::connect(ui->close_btn, SIGNAL(clicked(bool)), this, SLOT(close()));
    QObject::connect(ui->start_btn, SIGNAL(clicked(bool)), this, SLOT(start()));
    QObject::connect(ui->linewidget, SIGNAL(signal_toolEnded(QString&)), this, SLOT(slot_toolEnded(QString&)));
    ui->msg_label->setText("");
}

ToolLineDialog::~ToolLineDialog()
{
    delete ui;
}

void ToolLineDialog::mouseDoubleClickEvent( QMouseEvent * e )
{
    if ( e->button() == Qt::LeftButton )
    {
        start();
    }
}

void ToolLineDialog::closeEvent(QCloseEvent *)
{
    ui->linewidget->slot_toolEnded();
    OSGWidgetTool::instance()->getOSGWidget()->removeGeode(m_geode);
}

// ESC key hit
void ToolLineDialog::reject()
{
    ui->linewidget->slot_toolEnded();
    OSGWidgetTool::instance()->getOSGWidget()->removeGeode(m_geode);
    close();
}

void ToolLineDialog::start()
{
    OSGWidgetTool::instance()->slot_cancelTool();
    ui->msg_label->setText(tr("Right button to end"));
    ui->linewidget->clicked();
}

void ToolLineDialog::slot_toolEnded(QString&)
{
    ui->msg_label->setText(tr(""));
}
