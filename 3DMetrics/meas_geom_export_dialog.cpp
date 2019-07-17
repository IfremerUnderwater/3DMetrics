#include <QMessageBox>

#include "meas_geom_export_dialog.h"
#include "ui_meas_geom_export_dialog.h"
#include "file_dialog.h"

MeasGeomExportDialog::MeasGeomExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasGeomExportDialog)
{
    ui->setupUi(this);

    m_point_selected = false;
    m_line_selected = false;
    m_area_selected = false;
    m_filename = "";

    connect(ui->path_pushButton,SIGNAL(clicked(bool)),this,SLOT(slot_selectPath()));
    if( m_filename == NULL && ( !ui->shapefile_btn->isChecked() || !ui->ASCII_btn->isChecked() )
            && ( !ui->points_checkBox->isChecked() || !ui->lines_checkBox->isChecked() || !ui->areas_checkBox->isChecked() ) )
    {
        disconnect(ui->valid_cancel_buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
        connect(ui->valid_cancel_buttonBox,SIGNAL(accepted()),this,SLOT(slot_apply()));
    }
    connect(ui->path_lineEdit,SIGNAL(textChanged(QString)), this,SLOT(slot_textFilenameChanged(QString)));
}

MeasGeomExportDialog::~MeasGeomExportDialog()
{
    delete ui;
}
void MeasGeomExportDialog::slot_textFilenameChanged(QString _filename)
{
    m_filename = _filename;
    ui->path_lineEdit->setText(m_filename);
}

void MeasGeomExportDialog::slot_selectPath()
{
    m_filename = getSaveFileName(this,tr("Save measurement to geometry"), "", tr("Shapefile (*.shp)"));
    slot_textFilenameChanged(m_filename);
}

void MeasGeomExportDialog::on_ASCII_btn_clicked()
{
    m_export_type = MeasGeomExportDialog::export_type::ASCII;
}

void MeasGeomExportDialog::on_shapefile_btn_clicked()
{
    m_export_type = MeasGeomExportDialog::export_type::ShapeFile;
}

void MeasGeomExportDialog::slot_apply()
{
    if( ui->points_checkBox->isChecked() )
        m_point_selected = true;
    if( ui->lines_checkBox->isChecked() )
        m_line_selected = true;
    if( ui->areas_checkBox->isChecked() )
        m_area_selected = true;
    if( m_filename == NULL )
        QMessageBox::information(this,"Error : Export measurement to geometry","Error : you didn't give a name to the file");
    if( !(ui->points_checkBox->isChecked() || ui->lines_checkBox->isChecked() || ui->areas_checkBox->isChecked()) )
        QMessageBox::information(this,"Error : Export measurement to geometry","Error : you didn't choose the data to export");
    if( !(ui->shapefile_btn->isChecked() || ui->ASCII_btn->isChecked()) )
        QMessageBox::information(this,"Error : Export measurement to geometry","Error : you didn't choose the type to export");
    if( (ui->shapefile_btn->isChecked() || ui->ASCII_btn->isChecked()) && (ui->points_checkBox->isChecked() || ui->lines_checkBox->isChecked() || ui->areas_checkBox->isChecked()) && (m_filename != NULL) )
        accept();
}
