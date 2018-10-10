#include "attribpointwidget.h"
#include "ui_attribpointwidget.h"

#include "Measure/measurepoint.h"

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
}

void AttribPointWidget::clicked()
{
    // test
    if(m_item)
    {
        m_item->setX(m_item->x()+1);

        update();
    }
    //*** TODO
}

void AttribPointWidget::update()
{
    if(m_item)
    {
        ui->x_label->setText(QString::number(m_item->x(),'f',2));
        ui->y_label->setText(QString::number(m_item->y(),'f',2));
        ui->z_label->setText(QString::number(m_item->z(),'f',2));

        ui->tool_label->setStyleSheet("");
    }
}
