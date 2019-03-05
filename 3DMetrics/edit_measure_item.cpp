#include "edit_measure_item.h"
#include "ui_edit_measure_item.h"
#include <QListWidget>
#include <QMessageBox>

edit_measure_item::edit_measure_item(QWidget *parent, QListWidgetItem *item) :
    QWidget(parent),
    ui(new Ui::edit_measure_item)
{
    m_item = item;
    ui->setupUi(this);

    // fill combobox
    for(int i=0; i<MeasureType::_Last; i++)
    {
        QVariant v(i);
        ui->types_combo->addItem(MeasureType::value((MeasureType::type)i), v);
    }
    ui->types_combo->setCurrentIndex(0);

    QObject::connect(ui->delete_btn, SIGNAL(clicked(bool)), this, SLOT(slot_remove()));
}

edit_measure_item::~edit_measure_item()
{
    delete ui;
}

void edit_measure_item::slot_remove()
{
    QString msg = tr("Do you want to remove %1").arg(ui->name->text());
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, tr("Row removal Confirmation"),
                                                                msg,
                                                                QMessageBox::Cancel | QMessageBox::Ok,
                                                                QMessageBox::Cancel);
    if (resBtn == QMessageBox::Ok)
    {
        emit signal_remove(m_item);
    }
}

QString edit_measure_item::fieldName()
{
    return ui->name->text();
}

MeasureType::type edit_measure_item::fieldType()
{
    QVariant v = ui->types_combo->currentData();
    int i = v.toInt();
    if(i<0 || i >= MeasureType::_Last)
        return MeasureType::_Last;

    return (MeasureType::type)i;
}

void edit_measure_item::setValues(QString _name, QString _type)
{
    ui->name->setText(_name);

    MeasureType::type t = MeasureType::valueOf(_type);
    if(t >= 0 && t < MeasureType::_Last)
        ui->types_combo->setCurrentIndex(t);
}

