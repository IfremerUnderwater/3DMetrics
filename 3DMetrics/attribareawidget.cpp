#include "attribareawidget.h"
#include "ui_attribareawidget.h"

AttribAreaWidget::AttribAreaWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AttribAreaWidget)
{
    ui->setupUi(this);

    QObject::connect(ui->tool_btn, SIGNAL(clicked(bool)), this, SLOT(slot_clicked()));

    ui->tool_btn->setStyleSheet("background-color: red");
}

AttribAreaWidget::~AttribAreaWidget()
{
    delete ui;
}


void AttribAreaWidget::setNbval(QString _nb)
{
    ui->pts_label->setText(_nb);
}

void AttribAreaWidget::setAreaval(QString _area)
{
    ui->length_label->setText(_area);
}

void AttribAreaWidget::slot_clicked()
{
    ui->tool_btn->setStyleSheet("");
}
