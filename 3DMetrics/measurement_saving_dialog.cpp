#include "measurement_saving_dialog.h"
#include "ui_measurement_saving_dialog.h"
#include "mainwindow.h"
#include <QDebug>

using namespace std;

MeasurementSavingDialog::MeasurementSavingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeasurementSavingDialog)

{
    ui->setupUi(this);

    // read categories
    string line;
    ifstream cat_file ("categories.txt");
    if (cat_file)
    {
        int i=0;
        while (getline( cat_file, line ) )
        {
            m_category_names.push_back(QString::fromStdString(line));
            ui->categoryComboBox->insertItem(i, m_category_names[i]);
            i++;
        }
        cat_file.close();
    }
    else cout << "Unable to open file";

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

