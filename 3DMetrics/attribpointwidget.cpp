#include "attribpointwidget.h"
#include "ui_attribpointwidget.h"

#include "Measure/measurepoint.h"

#include "OSGWidget/osgwidgettool.h"

AttribPointWidget::AttribPointWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AttribPointWidget)
{
    ui->setupUi(this);

    ui->tool_label->setStyleSheet("background-color: red");
}

AttribPointWidget::~AttribPointWidget()
{
    delete ui;

    // just in case
    slot_toolEnded();
}

void AttribPointWidget::clicked()
{
//    // test
//    if(m_item)
//    {
//        m_item->setX(m_item->x()+1);

//        update();
//    }
//    //*** TODO

    // start tool
    QString msg = "Point tool started";
    emit signal_toolStarted(msg);

    OSGWidgetTool *tool = OSGWidgetTool::instance();
    connect(tool, SIGNAL(signal_clicked(Point3D&)), this, SLOT(slot_toolApply(Point3D&)));
    connect(tool, SIGNAL(signal_endTool()), this, SLOT(slot_toolEnded()));

    tool->startTool(OSGWidgetTool::Point);

}

void AttribPointWidget::update(bool valueOk)
{
    if(m_item)
    {
        ui->x_label->setText(QString::number(m_item->x(),'f',2));
        ui->y_label->setText(QString::number(m_item->y(),'f',2));
        ui->z_label->setText(QString::number(m_item->z(),'f',2));

        if(valueOk)
            ui->tool_label->setStyleSheet("");
    }
}

//void AttribPointWidget::slot_toolCanceled()
//{
//    QString msg = "Point tool canceled";
//    emit signal_toolEnded(msg);
//}

void AttribPointWidget::slot_toolApply(Point3D &p)
{
    m_item->setP(p);
    update();
    m_item->updateGeode();

    // only one point for "point"
    slot_toolEnded();
}

void AttribPointWidget::slot_toolEnded()
{
    disconnect(OSGWidgetTool::instance(), 0, this, 0);
    OSGWidgetTool::instance()->endTool();

    QString msg = "Point tool ended";
    emit signal_toolEnded(msg);
}
