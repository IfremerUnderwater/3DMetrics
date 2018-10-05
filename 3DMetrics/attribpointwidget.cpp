#include "attribpointwidget.h"
#include "ui_attribpointwidget.h"

AttribPointWidget::AttribPointWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AttribPointWidget)
{
    ui->setupUi(this);
}

AttribPointWidget::~AttribPointWidget()
{
    delete ui;
}
