#include "model_depth_colors_chooser.h"
#include "ui_model_depth_colors_chooser.h"

ModelDepthColorsChooser::ModelDepthColorsChooser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelDepthColorsChooser)
{
    ui->setupUi(this);


    connect(ui->close_btn, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->apply_btn, SIGNAL(clicked()), this, SLOT(slot_apply()));
    connect(ui->reset_btn, SIGNAL(clicked()), this, SLOT(slot_reset()));
    connect(ui->zmin_spin, SIGNAL(valueChanged(double)), this, SLOT(slot_zminvaluchanged()));
    connect(ui->zmax_spin, SIGNAL(valueChanged(double)), this, SLOT(slot_zmaxvaluchanged()));
    connect(ui->zmin_check, SIGNAL(clicked(bool)), this, SLOT(slot_zmindefault()));
    connect(ui->zmax_check, SIGNAL(clicked(bool)), this, SLOT(slot_zmaxdefault()));

    // allways on top
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowStaysOnTopHint;
    setWindowFlags( flags);
}

ModelDepthColorsChooser::~ModelDepthColorsChooser()
{
    delete ui;
}

void ModelDepthColorsChooser::slot_zminvaluchanged()
{
    // zmin
    m_edit_zmin = ui->zmin_spin->value();
    if(m_edit_zmin != m_zmin)
    {
        ui->zmin_check->setChecked(false);
    }

    ui->color_widget->setEdit_zmin(m_edit_zmin);
    ui->color_widget->update();
}

void ModelDepthColorsChooser::slot_zmaxvaluchanged()
{
    // zmax
    m_edit_zmax = ui->zmax_spin->value();
    if(m_edit_zmax != m_zmax)
    {
        ui->zmax_check->setChecked(false);
    }

    ui->color_widget->setEdit_zmax(m_edit_zmax);
    ui->color_widget->update();
}


void ModelDepthColorsChooser::slot_zmindefault()
{
    if(ui->zmin_check->isChecked())
    {
        m_edit_zmin = m_zmin;
        ui->zmin_spin->setValue(m_zmin);
        ui->zmin_check->setChecked(true);
        ui->color_widget->setEdit_zmin(m_edit_zmin);
        ui->color_widget->update();
    }
}

void ModelDepthColorsChooser::slot_zmaxdefault()
{
    if(ui->zmax_check->isChecked())
    {
        m_edit_zmax = m_zmax;
        ui->zmax_spin->setValue(m_zmax);
        ui->zmax_check->setChecked(true);
        ui->color_widget->setEdit_zmax(m_edit_zmax);
        ui->color_widget->update();
    }
}

void ModelDepthColorsChooser::slot_reset()
{
    m_edit_zmin = m_zmin;
    m_edit_zmax = m_zmax;
    ui->zmin_spin->setValue(m_edit_zmin);
    ui->zmax_spin->setValue(m_edit_zmax);
    ui->zmin_check->setChecked(true);
    ui->zmax_check->setChecked(true);

    ui->color_widget->setZmin(m_zmin);
    ui->color_widget->setZmax(m_zmax);
    ui->color_widget->setEdit_zmin(m_edit_zmin);
    ui->color_widget->setEdit_zmax(m_edit_zmax);

    ui->color_widget->update();
}

void ModelDepthColorsChooser::slot_apply()
{
    bool usedefault = true;
    if(!ui->zmin_check->isChecked())
        usedefault = false;
    if(!ui->zmax_check->isChecked())
        usedefault = false;

    emit signal_minmaxchanged(m_edit_zmin, m_edit_zmax, usedefault);
}
