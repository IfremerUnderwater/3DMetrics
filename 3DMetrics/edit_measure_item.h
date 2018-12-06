#ifndef EDIT_MEASURE_ITEM_H
#define EDIT_MEASURE_ITEM_H

#include <QWidget>

#include "Measurement/measurement_type.h"

class QListWidgetItem;

namespace Ui {
class edit_measure_item;
}

class edit_measure_item : public QWidget
{
    Q_OBJECT

public:
    explicit edit_measure_item(QWidget *parent = 0, QListWidgetItem *item=0);
    ~edit_measure_item();

    //values
    QString fieldName();
    MeasureType::type fieldType();

    // set values - for loading values
    void setValues(QString _name, QString _type);

public slots:
    void slot_remove();

signals:
    void signal_remove(QListWidgetItem*);

private:
    Ui::edit_measure_item *ui;
    QListWidgetItem* m_item;
};

#endif // EDIT_MEASURE_ITEM_H
