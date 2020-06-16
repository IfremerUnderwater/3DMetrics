#ifndef MEAS_TABLE_WIDGET_ITEM_H
#define MEAS_TABLE_WIDGET_ITEM_H

#include <QTableWidgetItem>

#include "Measurement/measurement_item.h"

class MeasTableWidgetItem : public QTableWidgetItem
{
public:
    MeasTableWidgetItem(int _type = QTableWidgetItem::UserType+1);
    virtual ~MeasTableWidgetItem();

    void setMeasItem(MeasItem *_item) { m_item = _item; }
    MeasItem *measItem() const { return m_item; }

private:
    MeasItem *m_item;
};

#endif // MEAS_TABLE_WIDGET_ITEM_H
