#include "edit_meas_item.h"
#include "ui_edit_meas_item.h"
#include <QListWidget>
#include <QMessageBox>

EditMeasItem::EditMeasItem(QWidget *parent, QListWidgetItem *item) :
    QWidget(parent),
    ui(new Ui::EditMeasItem)
{
    m_item = item;
    ui->setupUi(this);

    // fill combobox
    for(int i=0; i<MeasType::_Last; i++)
    {
        QVariant v(i);
        ui->types_combo->addItem(MeasType::value((MeasType::type)i), v);
    }
    ui->types_combo->setCurrentIndex(0);

    QObject::connect(ui->delete_btn, SIGNAL(clicked(bool)), this, SLOT(slot_remove()));
}

EditMeasItem::~EditMeasItem()
{
    delete ui;
}

void EditMeasItem::slot_remove()
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

QString EditMeasItem::fieldName()
{
    return ui->name->text();
}

MeasType::type EditMeasItem::fieldType()
{
    QVariant v = ui->types_combo->currentData();
    int i = v.toInt();
    if(i<0 || i >= MeasType::_Last)
        return MeasType::_Last;

    return (MeasType::type)i;
}

void EditMeasItem::setValues(QString _name, QString _type)
{
    ui->name->setText(_name);

    MeasType::type t = MeasType::valueOf(_type);
    if(t >= 0 && t < MeasType::_Last)
        ui->types_combo->setCurrentIndex(t);
}

