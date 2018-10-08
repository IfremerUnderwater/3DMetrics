#include "attribpointwidget.h"
#include "ui_attribpointwidget.h"

#include "Measure/measurepoint.h"

AttribPointWidget::AttribPointWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AttribPointWidget)
{
    ui->setupUi(this);

    //QObject::connect(ui->tool_btn, SIGNAL(clicked(bool)), this, SLOT(slot_clicked()));
    //ui->tool_btn->setStyleSheet("background-color: red");
    ui->tool_label->setStyleSheet("background-color: red");
}

AttribPointWidget::~AttribPointWidget()
{
    delete ui;
}

//void AttribPointWidget::setXval(QString _x)
//{
//    ui->x_label->setText(_x);
//}

//void AttribPointWidget::setYval(QString _y)
//{
//    ui->y_label->setText(_y);
//}

//void AttribPointWidget::setZval(QString _z)
//{
//    ui->z_label->setText(_z);
//}

void AttribPointWidget::clicked()
{
    //ui->tool_btn->setStyleSheet("");
    ui->tool_label->setStyleSheet("");

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
    }
}
