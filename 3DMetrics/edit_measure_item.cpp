#include "edit_measure_item.h"
#include "ui_edit_measure_item.h"

edit_measure_item::edit_measure_item(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::edit_measure_item)
{
    ui->setupUi(this);
}

edit_measure_item::~edit_measure_item()
{
    delete ui;
}
