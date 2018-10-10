#include "attribareawidget.h"
#include "ui_attribareawidget.h"
#include "Measure/measurearea.h"

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
    ui->tool_label->setStyleSheet("");
    if(m_item)
    {
        //*** TODO
        // test
        double t = m_item->getArray().length();
        Point3D pt;
        pt.x = t;
        pt.y = t+1;
        pt.z = (t+2)*t / 20.0;
        m_item->getArray().append(pt);
        m_item->computeLengthAndArea();
        update();
    }
}

void AttribAreaWidget::update()
{
    if(m_item)
    {
        QString snb = QString::number(m_item->nbPts());
        ui->pts_label->setText(snb);
        QString slg = QString::number(m_item->area(),'f',2);
        ui->length_label->setText(slg);

        if(m_item->nbPts() > 0)
            ui->tool_label->setStyleSheet("");
        else
            ui->tool_label->setStyleSheet("background-color: red");
    }
}
