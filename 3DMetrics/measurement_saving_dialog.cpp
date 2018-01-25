#include "measurement_saving_dialog.h"
#include "ui_measurement_saving_dialog.h"
#include "mainwindow.h"
#include <QDebug>


MeasurementSavingDialog::MeasurementSavingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasurementSavingDialog),
    m_measur_counter(0)

{
    ui->setupUi(this);


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



    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(slot_addMeasFormValues()));
    QObject::connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(sl_clearMeasForm()));
    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(sl_clearPreviousMeasFormValues()));

    QObject::connect(this, SIGNAL(rejected()), this, SLOT(sl_clearMeasForm()));

}

MeasurementSavingDialog::~MeasurementSavingDialog()
{
    delete ui;
}

void MeasurementSavingDialog::setDistanceSurfaceMeasValueAndType(double _measurement, ToolState _measurement_type, int _measurement_index)
{
    QString measurementText;
    measurementText = QString::number(_measurement);

    ui->typeOfMeasurEdit->setText(QString(_measurement_type));
    ui->measurResultLabel->setText(measurementText);

    m_measurement_index = _measurement_index;
}

void MeasurementSavingDialog::setInterestPointMeasValueAndType(QString _coordinates, ToolState _measurement_type, int _measurement_index)
{
    ui->typeOfMeasurEdit->setText(QString(_measurement_type));
    ui->measurResultLabel->setText(_coordinates);

    m_measurement_index = _measurement_index;
}


void MeasurementSavingDialog::slot_addMeasFormValues()
{
    m_measur_counter++;

    QString _measur_name = ui->nameOfMeasurementLineEdit->text();

    QString _category = ui->categoryComboBox->currentText();
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    //QString _measur_type = ui->typeOfMeasurEdit->text();
    ToolState _measur_type = LINE_MEASUREMENT_STATE;

    QString _measur_result = ui->measurResultLabel->text();

    QString _temperature = ui->temperatureLineEdit->text();

    QString _comments = ui->commentsText->toPlainText();

    emit sig_getMeasFormValues(_measur_name, _measur_type, _category, _temperature, _measur_result, m_measurement_index, _comments);
}



void MeasurementSavingDialog::sl_clearMeasForm()
{
    ui->nameOfMeasurementLineEdit->clear();
    ui->commentsText->clear();
    ui->temperatureLineEdit->clear();

    if (ui->typeOfMeasurEdit->text() == "Distance measurement")
    {
        emit si_distanceMeasurementFormCanceled();
    }

    else if(ui->typeOfMeasurEdit->text() == "Surface measurement")
    {
        emit si_surfaceMeasurementFormCanceled();
    }

    else if(ui->typeOfMeasurEdit->text() == "Interest point measurement")
    {
        emit si_interestPointMeasurementFormCanceled();
    }


}

void MeasurementSavingDialog::sl_clearPreviousMeasFormValues()
{
    ui->nameOfMeasurementLineEdit->clear();
    ui->commentsText->clear();
    ui->temperatureLineEdit->clear();

}

