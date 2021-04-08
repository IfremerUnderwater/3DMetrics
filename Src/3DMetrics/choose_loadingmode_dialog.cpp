#include "choose_loadingmode_dialog.h"
#include "ui_choose_loadingmode_dialog.h"

ChooseLoadingModeDialog::ChooseLoadingModeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseLoadingModeDialog)
{
    ui->setupUi(this);

    connect(ui->ok_button,SIGNAL(clicked()),this,SLOT(ok()));
    connect(ui->points_radio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->triangles_radio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->triangles_normals_radio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->triangles_points_radio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->useLODTiles_radio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->useLODTilesDir_radio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->useSmartLODTiles_radio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->useSmartLODTilesDir_radio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->buildLOD_radio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
    connect(ui->buildTiles_radio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
}

ChooseLoadingModeDialog::~ChooseLoadingModeDialog()
{
    delete ui;
}

void ChooseLoadingModeDialog::setMode(LoadingMode _mode)
{
    switch (_mode) {
    case LoadingModePoint:
        ui->points_radio->setChecked(true);
        break;

    case LoadingModeTriangle:
        ui->triangles_radio->setChecked(true);
        break;

    case LoadingModeTriangleNormals:
        ui->triangles_normals_radio->setChecked(true);
        break;

    case LoadingModeTrianglePoint:
        ui->triangles_points_radio->setChecked(true);
        break;

    case LoadingModeLODTiles:
        ui->useLODTiles_radio->setChecked(true);
        break;

    case LoadingModeLODTilesDir:
        ui->useLODTilesDir_radio->setChecked(true);
        break;

    case LoadingModeSmartLODTiles:
        ui->useSmartLODTiles_radio->setChecked(true);
        break;

    case LoadingModeSmartLODTilesDir:
        ui->useSmartLODTilesDir_radio->setChecked(true);
        break;

    case LoadingModeBuildLODTiles:
        ui->buildLOD_radio->setChecked(true);
        break;

    case LoadingModeBuildTiles:
        ui->buildTiles_radio->setChecked(true);
        break;
    }
}

void ChooseLoadingModeDialog::radioToggled()
{
    if(ui->points_radio->isChecked())
    {
        m_mode = LoadingModePoint;
    }
    if(ui->triangles_radio->isChecked())
    {
        m_mode = LoadingModeTriangle;
    }
    if(ui->triangles_normals_radio->isChecked())
    {
        m_mode = LoadingModeTriangleNormals;
    }
    if(ui->triangles_points_radio->isChecked())
    {
        m_mode = LoadingModeTrianglePoint;
    }
    if(ui->useLODTiles_radio->isChecked())
    {
        m_mode = LoadingModeLODTiles;
    }
    if(ui->useLODTilesDir_radio->isChecked())
    {
        m_mode = LoadingModeLODTilesDir;
    }
    if(ui->useSmartLODTiles_radio->isChecked())
    {
        m_mode = LoadingModeSmartLODTiles;
    }
    if(ui->useSmartLODTilesDir_radio->isChecked())
    {
        m_mode = LoadingModeSmartLODTilesDir;
    }
    if(ui->buildLOD_radio->isChecked())
    {
        m_mode = LoadingModeBuildLODTiles;
    }
    if(ui->buildTiles_radio->isChecked())
    {
        m_mode = LoadingModeBuildTiles;
    }
}

void ChooseLoadingModeDialog::ok()
{
    radioToggled();

    accept();
}

