#include "decimation_dialog.h"
#include "ui_decimation_dialog.h"
#include "file_dialog.h"

DecimationDialog::DecimationDialog(QWidget *_parent) :
    QDialog(_parent),
    ui(new Ui::DecimationDialog)
{
    ui->setupUi(this);

    connect(ui->select_file_bt,SIGNAL(clicked(bool)),this,SLOT(slot_selectModel()));
}

DecimationDialog::~DecimationDialog()
{
    delete ui;
}

QString DecimationDialog::getModelPath()
{
    return ui->file_to_be_decimated->text();
}

double DecimationDialog::getDecimationFactor()
{
    return ui->decimation_factor_sb->value();
}

void DecimationDialog::slot_selectModel()
{
    QString filename = getOpenFileName(this,tr("Select a 3d Model to open"), "", tr("3D files (*.kml *.obj)"));
    ui->file_to_be_decimated->setText(filename);
}
