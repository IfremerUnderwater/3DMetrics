#include "attribareawidget.h"
#include "ui_attribareawidget.h"
#include "Measurement/measurement_area.h"
#include "OSGWidget/osg_widget_tool.h"

AttribAreaWidget::AttribAreaWidget(QWidget *parent) :
    QWidget(parent),
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

    m_item->save();

    m_item->getArray().clear();
    m_item->computeLengthAndArea();
    m_item->updateGeode();
    update();

    tool->startTool(OSGWidgetTool::Area);
}

void AttribAreaWidget::update()
{
    if(m_item)
    {
        QString snb = QString::number(m_item->nbPts());
        ui->pts_label->setText(snb);
        QString slg = QString::number(m_item->area(),'f',2);
        ui->length_label->setText(slg + " m²");

        if(m_item->nbPts() > 0)
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
    m_item->restore();

    slot_toolEnded();
}


void AttribAreaWidget::slot_toolClicked(Point3D &p)
{
    // add point
    m_item->getArray().push_back(p);
    m_item->computeLengthAndArea();
    update();
    m_item->updateGeode();
}

void AttribAreaWidget::slot_toolRemoveLastPoint()
{
    // remove last point
    if(!m_item->getArray().empty())
    {
        m_item->getArray().pop_back();
        m_item->computeLength();
        update();
        m_item->updateGeode();
    }

}
