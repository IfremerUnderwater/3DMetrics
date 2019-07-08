#include "meas_geom_export_dialog.h"
#include "ui_meas_geom_export_dialog.h"
#include "file_dialog.h"

MeasGeomExportDialog::MeasGeomExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasGeomExportDialog)
{
    ui->setupUi(this);

    connect(ui->path_pushButton,SIGNAL(clicked(bool)),this,SLOT(slot_selectPath()));
    if( m_filename == NULL || !ui->shapefile_btn->isChecked() || !ui->ASCII_btn->isChecked() ){
        disconnect(ui->valid_cancel_buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
    }
}

MeasGeomExportDialog::~MeasGeomExportDialog()
{
    delete ui;
}

void MeasGeomExportDialog::slot_selectPath()
{
    m_filename = getSaveFileName(this,tr("Save measurement to geometry"), "", tr("Shapefile (*.shp)"));
    ui->path_lineEdit->setText(m_filename);
    connect(ui->valid_cancel_buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
}

void MeasGeomExportDialog::on_ASCII_btn_clicked()
{
    m_export_type = MeasGeomExportDialog::export_type::ASCII;
    connect(ui->valid_cancel_buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
}

void MeasGeomExportDialog::on_shapefile_btn_clicked()
{
    m_export_type = MeasGeomExportDialog::export_type::ShapeFile;
    connect(ui->valid_cancel_buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
}
