#include "attrib_area_widget.h"
#include "ui_attrib_area_widget.h"
#include "Measurement/measurement_area.h"
#include "OSGWidget/osg_widget_tool.h"

AttribAreaWidget::AttribAreaWidget(QWidget *_parent) :
    QWidget(_parent),
    ui(new Ui::AttribAreaWidget)
{
    ui->setupUi(this);

    ui->tool_label->setStyleSheet("background-color: red");
}

AttribAreaWidget::~AttribAreaWidget()
{
    delete ui;
}

void AttribAreaWidget::clicked()
{
    // start tool
    QString msg = "Area tool started";
    emit signal_toolStarted(msg);

    OSGWidgetTool *tool = OSGWidgetTool::instance();
    connect(tool, SIGNAL(signal_clicked(Point3D&)), this, SLOT(slot_toolClicked(Point3D&)));
    connect(tool, SIGNAL(signal_endTool()), this, SLOT(slot_toolEnded()));
    connect(tool, SIGNAL(signal_cancelTool()), this, SLOT(slot_toolCanceled()));
    connect(tool, SIGNAL(signal_removeLastPointTool()), this, SLOT(slot_toolRemoveLastPoint()));

    m_area_item->save();

    m_area_item->getArray().clear();
    m_area_item->computeLengthAndArea();
    m_area_item->updateGeode();
    update();

    tool->startTool(OSGWidgetTool::Area);
}

void AttribAreaWidget::update()
{
    if(m_area_item)
    {
        QString nb_pts_string = QString::number(m_area_item->nbPts());
        ui->pts_label->setText(nb_pts_string);
        QString area_string = QString::number(m_area_item->area(),'f',2);
        ui->length_label->setText(area_string + " m²");

        if(m_area_item->nbPts() > 0)
            ui->tool_label->setStyleSheet("");
        else
            ui->tool_label->setStyleSheet("background-color: red");
    }
}

void AttribAreaWidget::slot_toolEnded()
{
    // ok si nb points > 1
    update();

    disconnect(OSGWidgetTool::instance(), 0, this, 0);
    OSGWidgetTool::instance()->endTool();

    QString msg = "Area tool ended";
    emit signal_toolEnded(msg);
}

void AttribAreaWidget::slot_toolCanceled()
{
    m_area_item->restore();

    slot_toolEnded();
}


void AttribAreaWidget::slot_toolClicked(Point3D &_point)
{
    // add point
    m_area_item->getArray().push_back(_point);
    m_area_item->computeLengthAndArea();
    update();
    m_area_item->updateGeode();
}

void AttribAreaWidget::slot_toolRemoveLastPoint()
{
    // remove last point
    if(!m_area_item->getArray().empty())
    {
        m_area_item->getArray().pop_back();
        m_area_item->computeLength();
        update();
        m_area_item->updateGeode();
    }

}
