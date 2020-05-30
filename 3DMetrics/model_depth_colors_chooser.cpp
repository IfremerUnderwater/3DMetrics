#include "model_depth_colors_chooser.h"
#include "ui_model_depth_colors_chooser.h"

#include "OSGWidget/shader_color.h"

ModelDepthColorsChooser::ModelDepthColorsChooser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelDepthColorsChooser)
{
    ui->setupUi(this);

    connect(ui->close_btn, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->apply_btn, SIGNAL(clicked()), this, SLOT(slot_apply()));
    connect(ui->reset_btn, SIGNAL(clicked()), this, SLOT(slot_reset()));
    connect(ui->zmin_spin, SIGNAL(valueChanged(double)), this, SLOT(slot_zminvaluchanged()));
    connect(ui->zmax_spin, SIGNAL(valueChanged(double)), this, SLOT(slot_zmaxvaluchanged()));
    connect(ui->zmin_check, SIGNAL(clicked(bool)), this, SLOT(slot_zmindefault()));
    connect(ui->zmax_check, SIGNAL(clicked(bool)), this, SLOT(slot_zmaxdefault()));

    // fill ComboBox
    ui->palette_combo->addItem(ShaderColor::paletteName(ShaderColor::Rainbow));
    ui->palette_combo->addItem(ShaderColor::paletteName(ShaderColor::BlueToYellow));

    connect(ui->palette_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slot_paletteChanged()));

    // allways on top
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowStaysOnTopHint;
    setWindowFlags( flags);
}

ModelDepthColorsChooser::~ModelDepthColorsChooser()
{
    delete ui;
}

void ModelDepthColorsChooser::setPalette(ShaderColor::Palette _palette)
{
    m_palette = _palette;
    ui->palette_combo->setCurrentText(ShaderColor::paletteName(m_palette));
    slot_paletteChanged();
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

    emit signal_minmaxchanged(m_edit_zmin, m_edit_zmax, usedefault, m_palette);
}

void ModelDepthColorsChooser::slot_paletteChanged()
{
    if(ui->palette_combo->currentText() == ShaderColor::paletteName(ShaderColor::Rainbow))
        m_palette = ShaderColor::Rainbow;
    if(ui->palette_combo->currentText() == ShaderColor::paletteName(ShaderColor::BlueToYellow))
        m_palette = ShaderColor::BlueToYellow;
    ui->color_widget->setColorPalette(m_palette);
    ui->color_widget->update();
}
