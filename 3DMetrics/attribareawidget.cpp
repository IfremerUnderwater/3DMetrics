#include "attribareawidget.h"
#include "ui_attribareawidget.h"

AttribAreaWidget::AttribAreaWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AttribAreaWidget)
{
    ui->setupUi(this);
}

AttribAreaWidget::~AttribAreaWidget()
{
    delete ui;
}
