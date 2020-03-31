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
    connect(ui->triangles_points_radio,SIGNAL(toggled(bool)),this,SLOT(radioToggled()));
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

    case LoadingModeTrianglePoint:
        ui->triangles_points_radio->setChecked(true);
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
    if(ui->triangles_points_radio->isChecked())
    {
        m_mode = LoadingModeTrianglePoint;
    }
}

void ChooseLoadingModeDialog::ok()
{
    radioToggled();

    accept();
}

