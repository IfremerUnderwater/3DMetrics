#include "attriblinewidget.h"
#include "ui_attriblinewidget.h"

AttribLineWidget::AttribLineWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AttribLineWidget)
{
    ui->setupUi(this);

    QObject::connect(ui->tool_btn, SIGNAL(clicked(bool)), this, SLOT(slot_clicked()));

    ui->tool_btn->setStyleSheet("background-color: red");
}

AttribLineWidget::~AttribLineWidget()
{
    delete ui;
}

void AttribLineWidget::setNbval(QString _nb)
{
    ui->pts_label->setText(_nb);
}

void AttribLineWidget::setLengthval(QString _length)
{
    ui->length_label->setText(_length);
}

void AttribLineWidget::slot_clicked()
{
    ui->tool_btn->setStyleSheet("");

}
