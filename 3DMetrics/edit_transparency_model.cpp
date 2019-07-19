#include <string>

#include "edit_transparency_model.h"
#include "ui_edit_transparency_model.h"

EditTransparencyModel::EditTransparencyModel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditTransparencyModel)
{
    ui->setupUi(this);
    ui->transparency_value_slider->setTickInterval(100);
    ui->transparency_value_slider->setSingleStep(1);
    ui->transparency_label->setText(0);

    connect(ui->transparency_value_slider,SIGNAL(valueChanged(int)), this, SLOT(slot_changeTransparencyValue(int)));
    connect(ui->valid_btn, SIGNAL(clicked()), this, SLOT(accept()));
}

EditTransparencyModel::~EditTransparencyModel()
{
    delete ui;
}

void EditTransparencyModel::slot_changeTransparencyValue(int _transparency_value)
{
    ui->transparency_label->setText(QString::number(_transparency_value));
    emit signal_onChangedTransparencyValue(_transparency_value);
}
