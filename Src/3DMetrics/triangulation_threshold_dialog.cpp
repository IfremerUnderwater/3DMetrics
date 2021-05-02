#include "triangulation_threshold_dialog.h"
#include "ui_triangulation_threshold_dialog.h"
#include <QDoubleValidator>

TriangulationThresholdDialog::TriangulationThresholdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TriangulationThresholdDialog)
{
    ui->setupUi(this);

    m_doubleValidator = new QDoubleValidator(0.0,1000000000.0,1,this);
    m_doubleValidator->setLocale(QLocale::C);

    ui->dxTh_edit->setValidator(m_doubleValidator);
    ui->dyTh_edit->setValidator(m_doubleValidator);
    ui->dzTh_edit->setValidator(m_doubleValidator);
    ui->triangleTh_edit->setValidator(m_doubleValidator);

    connect(ui->cancelButton, SIGNAL(clicked(bool)),this,SLOT(reject()));
    connect(ui->okButton, SIGNAL(clicked(bool)),this,SLOT(accept()));
}

TriangulationThresholdDialog::~TriangulationThresholdDialog()
{
    delete ui;
}

double TriangulationThresholdDialog::threshold() const
{
    return m_threshold;
}

void TriangulationThresholdDialog::setThreshold(double threshold)
{
    m_threshold = threshold;
    QString value = QString::number(threshold,'f', 1);
    ui->triangleTh_edit->setText(value);
}

double TriangulationThresholdDialog::dXthreshold() const
{
    return m_dXthreshold;
}

void TriangulationThresholdDialog::setDXthreshold(double dXthreshold)
{
    m_dXthreshold = dXthreshold;
    QString value = QString::number(dXthreshold,'f', 1);
    ui->dxTh_edit->setText(value);
}

double TriangulationThresholdDialog::dYthreshold() const
{
    return m_dYthreshold;
}

void TriangulationThresholdDialog::setDYthreshold(double dYthreshold)
{
    m_dYthreshold = dYthreshold;
    QString value = QString::number(dYthreshold,'f', 1);
    ui->dyTh_edit->setText(value);
}

double TriangulationThresholdDialog::dZthreshold() const
{
    return m_dZthreshold;
}

void TriangulationThresholdDialog::setDZthreshold(double dZthreshold)
{
    m_dZthreshold = dZthreshold;
    QString value = QString::number(dZthreshold,'f', 1);
    ui->dzTh_edit->setText(value);
}

void TriangulationThresholdDialog::accept()
{
    // read values
    m_threshold = ui->triangleTh_edit->text().toDouble();
    m_dXthreshold = ui->dxTh_edit->text().toDouble();
    m_dYthreshold = ui->dyTh_edit->text().toDouble();
    m_dZthreshold = ui->dzTh_edit->text().toDouble();

    QDialog::accept();
}
