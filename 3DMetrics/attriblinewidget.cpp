#include "attriblinewidget.h"
#include "ui_attriblinewidget.h"

#include "Measurement/measurement_line.h"
#include "OSGWidget/osg_widget_tool.h"

AttribLineWidget::AttribLineWidget(QWidget *parent) :
    QWidget(parent),
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

    m_item->save();

    m_item->getArray().clear();
    m_item->computeLength();
    m_item->updateGeode();
    update();

    tool->startTool(OSGWidgetTool::Line);
}

void AttribLineWidget::update()
{
    if(m_item)
    {
        QString snb = QString::number(m_item->nbPts());
        ui->pts_label->setText(snb);
        QString slg = QString::number(m_item->length(),'f',2);
        ui->length_label->setText(slg + " m");

        if(m_item->nbPts() > 0)
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

    m_item->cancel();
}

void AttribLineWidget::slot_toolCanceled()
{
    m_item->restore();

    slot_toolEnded();
}

void AttribLineWidget::slot_toolClicked(Point3D &p)
{
    // add point
    m_item->getArray().push_back(p);
    m_item->computeLength();
    update();
    m_item->updateGeode();
}
