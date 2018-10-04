#include "edit_measure_dialog.h"
#include "ui_edit_measure_dialog.h"

edit_measure_dialog::edit_measure_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::edit_measure_dialog)
{
    ui->setupUi(this);
}

edit_measure_dialog::~edit_measure_dialog()
{
    delete ui;
}
