#include "model_loadingmode_dialog.h"
#include "ui_model_loadingmode_dialog.h"

ModelLoadingModeDialog::ModelLoadingModeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelLoadingModeDialog)
{
    ui->setupUi(this);

    connect(ui->okButton,SIGNAL(clicked()),this,SLOT(ok()));
    connect(ui->cancelButton,SIGNAL(clicked()),this,SLOT(reject()));

    connect(ui->defaultRadio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));

    connect(ui->buildOSGBRadio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->buildSmartLODRadio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->buildSmartLODTilesRadio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->useOSGBRadio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->useSmartLODRadio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->useSmartLODTilesFolderRadio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->useSmartLODTilesRadio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->useTilesFolderRadio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->useTilesRadio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));

    ui->defaultRadio->setChecked(true);
    radioToggled();
}

ModelLoadingModeDialog::~ModelLoadingModeDialog()
{
    delete ui;
}


void ModelLoadingModeDialog::radioToggled()
{
    ui->saveCompoudCheck->setEnabled(false);
    ui->XspinBox->setEnabled(false);
    ui->Xlabel->setEnabled(false);
    ui->YspinBox->setEnabled(false);
    ui->Ylabel->setEnabled(false);

    if(ui->defaultRadio->isChecked())
    {
        m_mode = LoadingModeDefault;
    }

    if(ui->useOSGBRadio->isChecked())
    {
        m_mode = LoadingModeUseOSGB;
    }

    if(ui->buildOSGBRadio->isChecked())
    {
        m_mode = LoadingModeBuildOSGB;
    }

    if(ui->buildSmartLODRadio->isChecked())
    {
        m_mode = LoadingModeBuildAndUseSmartLOD;
        ui->saveCompoudCheck->setEnabled(true);
    }

    if(ui->useSmartLODRadio->isChecked())
    {
        m_mode = LoadingModeUseSmartLOD;
    }

    if(ui->useTilesRadio->isChecked())
    {
        m_mode = LoadingModeLODTiles;
    }
    if(ui->useTilesFolderRadio->isChecked())
    {
        m_mode = LoadingModeLODTilesDir;
    }

    if(ui->useSmartLODTilesRadio->isChecked())
    {
        m_mode = LoadingModeSmartLODTiles;
    }
    if(ui->useSmartLODTilesFolderRadio->isChecked())
    {
        m_mode = LoadingModeSmartLODTilesDir;
    }

    if(ui->buildSmartLODTilesRadio->isChecked())
    {
        m_mode = LoadingModeBuildLODTiles;
        ui->saveCompoudCheck->setEnabled(true);
        ui->XspinBox->setEnabled(true);
        ui->YspinBox->setEnabled(true);
        ui->Xlabel->setEnabled(true);
        ui->Ylabel->setEnabled(true);
    }
}

void ModelLoadingModeDialog::ok()
{
    radioToggled();

    m_saveCompoundLOD = ui->saveCompoudCheck->isChecked();
    m_nXTiles = ui->XspinBox->value();
    m_nYTiles = ui->YspinBox->value();

    accept();
}
