#include "OSGWidget/osg_widget.h"

#include "attrib_line_widget.h"
#include "ui_attrib_line_widget.h"

#include "Measurement/measurement_line.h"
#include "OSGWidget/osg_widget_tool.h"

AttribLineWidget::AttribLineWidget(QWidget *_parent) :
    QWidget(_parent),
    ui(new Ui::AttribLineWidget)
{
    ui->setupUi(this);

    ui->tool_label->setStyleSheet("background-color: red");
}

AttribLineWidget::~AttribLineWidget()
{
    delete ui;
}

void AttribLineWidget::clicked()
{
    // start tool
    QString msg = "Line tool started";
    emit signal_toolStarted(msg);

    OSGWidgetTool *tool = OSGWidgetTool::instance();
    connect(tool, SIGNAL(signal_clicked(Point3D&)), this, SLOT(slot_toolClicked(Point3D&)));
    connect(tool, SIGNAL(signal_endTool()), this, SLOT(slot_toolEnded()));
    connect(tool, SIGNAL(signal_cancelTool()), this, SLOT(slot_toolCanceled()));
    connect(tool, SIGNAL(signal_removeLastPointTool()), this, SLOT(slot_toolRemoveLastPoint()));

    m_line_item->save();

    m_line_item->getArray().clear();
    m_line_item->computeLength();
    m_line_item->updateGeode();
    update();

    tool->startTool(OSGWidgetTool::Line);
}

void AttribLineWidget::update()
{
    if(m_line_item)
    {
        QString nb_pts_string = QString::number(m_line_item->nbPts());
        ui->pts_label->setText(nb_pts_string);
        QString length_string = QString::number(m_line_item->length(),'f',2);
        ui->length_label->setText(length_string + " m");

        if(m_line_item->nbPts() > 0)
            ui->tool_label->setStyleSheet("");
        else
            ui->tool_label->setStyleSheet("background-color: red");
    }
}

void AttribLineWidget::slot_toolEnded()
{
    // ok si nb points >= 2
    update();

    disconnect(OSGWidgetTool::instance(), 0, this, 0);
    OSGWidgetTool::instance()->endTool();

    QString msg = "Line tool ended";

    emit signal_toolEnded(msg);

    m_line_item->cancel();
}

void AttribLineWidget::slot_toolCanceled()
{
    m_line_item->restore();

    slot_toolEnded();
}

void AttribLineWidget::slot_toolClicked(Point3D &_point)
{
    // add point
    m_line_item->getArray().push_back(_point);
    m_line_item->computeLength();
    update();
    m_line_item->updateGeode();
}

void AttribLineWidget::slot_toolRemoveLastPoint()
{
    // remove last point
    if(!m_line_item->getArray().empty())
    {
        m_line_item->getArray().pop_back();
        m_line_item->computeLength();
        update();
        m_line_item->updateGeode();
    }

}
