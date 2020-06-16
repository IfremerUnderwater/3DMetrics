#ifndef EDIT_MEAS_ITEM_H
#define EDIT_MEAS_ITEM_H

#include <QWidget>
#include <ui_edit_meas_item.h>

#include "Measurement/measurement_type.h"

class QListWidgetItem;

namespace Ui {
class EditMeasItem;
}

class EditMeasItem : public QWidget
{
    Q_OBJECT

public:
    explicit EditMeasItem(QWidget *parent = 0, QListWidgetItem *item=0);
    ~EditMeasItem();

    //values
    QString fieldName();
    MeasType::type fieldType();

    // set values - for loading values
    void setValues(QString _name, QString _type);

public slots:
    void slot_remove();

signals:
    void signal_remove(QListWidgetItem*);

private:
    Ui::EditMeasItem *ui;
    QListWidgetItem* m_item;
};

#endif // EDIT_MEAS_ITEM_H
