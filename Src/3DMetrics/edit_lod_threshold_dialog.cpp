#include "edit_lod_threshold_dialog.h"
#include "ui_edit_lod_threshold_dialog.h"

EditLODThresholdDialog::EditLODThresholdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditLODThresholdDialog)
{
    ui->setupUi(this);

    QObject::connect(ui->okButton, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->th1SpinBox,SIGNAL(valueChanged(double)), this, SLOT(slot_changeTh1(double)));
    connect(ui->th2SpinBox,SIGNAL(valueChanged(double)), this, SLOT(slot_changeTh2(double)));
}

EditLODThresholdDialog::~EditLODThresholdDialog()
{
    delete ui;
}

void EditLODThresholdDialog::setThresholds(const double _threshold1, const double _threshold2)
{
    ui->th1SpinBox->setValue(_threshold1);
    m_threshold1 = _threshold1;
    ui->th2SpinBox->setValue(_threshold2);
    m_threshold2 = _threshold2;
}

void EditLODThresholdDialog::slot_changeTh1(double _val)
{
    m_threshold1 = _val;
}

void EditLODThresholdDialog::slot_changeTh2(double _val)
{
    m_threshold2 = _val;
}
