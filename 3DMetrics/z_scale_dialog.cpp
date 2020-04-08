#include "z_scale_dialog.h"
#include "ui_z_scale_dialog.h"

#include "OSGWidget/osg_widget.h"
#include "OSGWidget/osg_widget_tool.h"

ZScaleDialog::ZScaleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZScaleDialog)
{
    ui->setupUi(this);

    QObject::connect(ui->close_btn, SIGNAL(clicked(bool)), this, SLOT(close()));
    QObject::connect(ui->apply_btn, SIGNAL(clicked(bool)), this, SLOT(slot_apply()));
    QObject::connect(ui->reset_btn, SIGNAL(clicked(bool)), this, SLOT(slot_reset()));
    connect(ui->zscale_slider,SIGNAL(valueChanged(int)), this, SLOT(slot_changeZScale(int)));

    setZScale(OSGWidgetTool::instance()->getOSGWidget()->getZScale());
}

ZScaleDialog::~ZScaleDialog()
{
    delete ui;
}

void ZScaleDialog::setZScale(double _zscale)
{
    m_zscale = _zscale;

    int val = _zscale / 10.0;
    ui->zscale_slider->setValue(val);
    ui->value_label->setText( QString::number(_zscale,'f',1));
}

void ZScaleDialog::slot_changeZScale(int _val)
{
    m_zscale = _val * 0.1;

    ui->value_label->setText( QString::number(m_zscale,'f',1));
}

void ZScaleDialog::slot_apply()
{
    OSGWidgetTool::instance()->getOSGWidget()->setZScale(m_zscale);
}

void ZScaleDialog::slot_reset()
{
    ui->zscale_slider->setValue(10);
}
