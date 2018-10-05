#include "attriblinewidget.h"
#include "ui_attriblinewidget.h"

AttribLineWidget::AttribLineWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AttribLineWidget)
{
    ui->setupUi(this);
}

AttribLineWidget::~AttribLineWidget()
{
    delete ui;
}
