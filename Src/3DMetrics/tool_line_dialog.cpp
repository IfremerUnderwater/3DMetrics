#include "OSGWidget/osg_widget.h"

#include "tool_line_dialog.h"
#include "ui_tool_line_dialog.h"

#include <QMouseEvent>

#include "OSGWidget/osg_widget_tool.h"

#include <Measurement/measurement_line.h>

#include "profile_depth_dialog.h"


ToolLineDialog::ToolLineDialog(QWidget *_parent) :
    QDialog(_parent),
    ui(new Ui::ToolLineDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    m_geode = new osg::Geode();
    m_meas_line = new MeasLine("*temporaryline", m_geode);

    osg::Vec4f color(1.0f,0.0f,0.0f,1.0f); // red (r,g,b,a)
    m_meas_line->setColor(color);

    ui->line_widget->setLine(m_meas_line);
    OSGWidgetTool::instance()->getOSGWidget()->addGeode(m_geode);

    QObject::connect(ui->close_btn, SIGNAL(clicked(bool)), this, SLOT(close()));
    QObject::connect(ui->removelast_btn, SIGNAL(clicked(bool)), this, SLOT(removelast()));
    QObject::connect(ui->profile_btn, SIGNAL(clicked(bool)), this, SLOT(profile()));
    QObject::connect(ui->profile_z_btn, SIGNAL(clicked(bool)), this, SLOT(profile_z()));
    QObject::connect(ui->start_btn, SIGNAL(clicked(bool)), this, SLOT(start()));

    QObject::connect(ui->line_widget, SIGNAL(signal_toolEnded(QString&)), this, SLOT(slot_toolEnded(QString&)));
    QObject::connect(ui->line_widget, SIGNAL(signal_nbPointsChanged()), this, SLOT(slot_nbPointsChanged()));

    ui->msg_label->setText("");

    ui->profile_btn->setEnabled(false);
    ui->profile_z_btn->setEnabled(false);
}

ToolLineDialog::~ToolLineDialog()
{
    delete ui;
}

void ToolLineDialog::mouseDoubleClickEvent( QMouseEvent * _e )
{
    if ( _e->button() == Qt::LeftButton )
    {
        start();
    }
}

void ToolLineDialog::closeEvent(QCloseEvent *)
{
    ui->line_widget->slot_toolEnded();
    OSGWidgetTool::instance()->getOSGWidget()->removeGeode(m_geode);
}

// ESC key hit
void ToolLineDialog::reject()
{
    ui->line_widget->slot_toolEnded();
    OSGWidgetTool::instance()->getOSGWidget()->removeGeode(m_geode);
    close();
}

void ToolLineDialog::start()
{
    OSGWidgetTool::instance()->slot_cancelTool();
    ui->msg_label->setText(tr("Right button to end"));
    ui->line_widget->clicked();
    ui->profile_btn->setEnabled(false);
    ui->profile_z_btn->setEnabled(false);
}

void ToolLineDialog::slot_toolEnded(QString&)
{
    slot_nbPointsChanged();

    ui->msg_label->setText(tr(""));
}

void ToolLineDialog::removelast()
{
    ui->line_widget->slot_toolRemoveLastPoint();

    slot_nbPointsChanged();
}

void ToolLineDialog::profile()
{
    ProfileDepthDialog *dialog = new ProfileDepthDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setMeasLine( m_meas_line );
    dialog->show();
}


void ToolLineDialog::profile_z()
{
    ProfileDepthDialog *dialog = new ProfileDepthDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setMeasLine( m_meas_line, true );
    dialog->show();
}

void ToolLineDialog::slot_nbPointsChanged()
{
    ui->removelast_btn->setEnabled(m_meas_line->length() > 0);
    ui->profile_btn->setEnabled(m_meas_line->length() >= 2);
    ui->profile_z_btn->setEnabled(m_meas_line->length() >= 2);
}
