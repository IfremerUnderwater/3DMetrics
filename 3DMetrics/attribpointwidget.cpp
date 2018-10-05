#include "attribpointwidget.h"
#include "ui_attribpointwidget.h"

AttribPointWidget::AttribPointWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AttribPointWidget)
{
    ui->setupUi(this);

    QObject::connect(ui->tool_btn, SIGNAL(clicked(bool)), this, SLOT(slot_clicked()));

    ui->tool_btn->setStyleSheet("background-color: red");
}

AttribPointWidget::~AttribPointWidget()
{
    delete ui;
}

void AttribPointWidget::setXval(QString _x)
{
    ui->x_label->setText(_x);
}

void AttribPointWidget::setYval(QString _y)
{
    ui->y_label->setText(_y);
}

void AttribPointWidget::setZval(QString _z)
{
    ui->z_label->setText(_z);
}

void AttribPointWidget::slot_clicked()
{
    ui->tool_btn->setStyleSheet("");

    //*** TODO
}
