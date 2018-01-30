#include "measurement_saving_dialog.h"
#include "ui_measurement_saving_dialog.h"
#include "mainwindow.h"
#include <QDebug>


MeasurementSavingDialog::MeasurementSavingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasurementSavingDialog)

{
    ui->setupUi(this);


    // Huuuuuuuuuurkkkkkkkkkkk cannot stay like that ...............................................
    m_category_names[BASALTE]="Basalte";
    m_category_names[SUBSTRAT_BRUN_ROUGE]="Substrat brun rouge";
    m_category_names[SUBSTRAT_BRUN_AVEC_FILAMENTS_BACTERIENS]="Substrat brun avec filaments bactériens";
    m_category_names[ANHYDRITE]="anhydrite";
    m_category_names[COUVERTURE_MICROBIENNE]="couverture microbienne";
    m_category_names[MOULIERES_AVEC_MOULES_DE_GRANDE_TAILLE_AVEC_FILAMENTS_BACTERIENS]="moulières avec moules de grande taille avec filaments bactériens";
    m_category_names[MOULIERES_AVEC_MOULES_DE_GRANDE_TAILLE_SANS_FILAMENTS_BACTERIENS]="moulières avec moules de grande taille sans filaments bactériens";
    m_category_names[MOULIERES_AVEC_MOULES_DE_TAILLES_MOYENNES_AVEC_FILAMENTS_BACTERIENS]="Moulières avec moules de tailles moyennes avec filaments bactériens";
    m_category_names[MOULIERES_AVEC_MOULES_DE_TAILLES_MOYENNES_SANS_FILAMENTS_BACTERIENS]="Moulières avec moules de tailles moyennes sans filaments bactériens";
    m_category_names[PETITES_MOULES_EPARSES_AVEC_FILAMENTS_BACTERIENS]="Petites moules éparses avec filaments bactériens";
    m_category_names[PETITES_MOULES_EPARSES_SANS_FILAMENTS_BACTERIENS]="Petites moules éparses sans filaments bactériens";
    m_category_names[CREVETTES]="Crevettes";
    m_category_names[GASTEROPODES]="Gastéropodes";


    ui->categoryComboBox->insertItem(0, m_category_names[0]);
    ui->categoryComboBox->insertItem(1, m_category_names[1]);
    ui->categoryComboBox->insertItem(2, m_category_names[2]);
    ui->categoryComboBox->insertItem(3, m_category_names[3]);
    ui->categoryComboBox->insertItem(4, m_category_names[4]);
    ui->categoryComboBox->insertItem(5, m_category_names[5]);
    ui->categoryComboBox->insertItem(6, m_category_names[6]);
    ui->categoryComboBox->insertItem(7, m_category_names[7]);
    ui->categoryComboBox->insertItem(8, m_category_names[8]);
    ui->categoryComboBox->insertItem(9, m_category_names[9]);
    ui->categoryComboBox->insertItem(10, m_category_names[10]);
    ui->categoryComboBox->insertItem(11, m_category_names[11]);
    ui->categoryComboBox->insertItem(12, m_category_names[12]);


    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(sl_acceptSaving()));
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(hide()));
    QObject::connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(hide()));
    QObject::connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(sl_cancelSaving()));

}

MeasurementSavingDialog::~MeasurementSavingDialog()
{
    delete ui;
}

void MeasurementSavingDialog::setMeasFields(QString _measurement_formatted, QString _measurement_type)
{
    ui->typeOfMeasurEdit->setText(_measurement_type);
    ui->measurResultLabel->setText(_measurement_formatted);
}


void MeasurementSavingDialog::sl_cancelSaving()
{
    cleanMeasFormValues();
    emit si_measFormCanceled();
}

void MeasurementSavingDialog::sl_acceptSaving()
{
    emit si_measFormAccepted();
}

void MeasurementSavingDialog::cleanMeasFormValues()
{
    ui->nameOfMeasurementLineEdit->clear();
    ui->commentsText->clear();
    ui->temperatureLineEdit->clear();

}

QString MeasurementSavingDialog::getMeasName()
{
    return ui->nameOfMeasurementLineEdit->text();
}

QString MeasurementSavingDialog::getMeasTemp()
{
    return ui->temperatureLineEdit->text();
}

QString MeasurementSavingDialog::getMeasComment()
{
    return ui->commentsText->toPlainText();
}

QString MeasurementSavingDialog::getMeasCategory()
{
    return ui->categoryComboBox->currentText();
}

