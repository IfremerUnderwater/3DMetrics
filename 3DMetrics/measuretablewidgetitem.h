#ifndef MEASURETABLEWIDGETITEM_H
#define MEASURETABLEWIDGETITEM_H

#include <QTableWidgetItem>

#include "Measurement/measurement_item.h"

class MeasureTableWidgetItem : public QTableWidgetItem
{
public:
    MeasureTableWidgetItem(int type = QTableWidgetItem::UserType+1);
    virtual ~MeasureTableWidgetItem();

    void setMeasureItem(MeasureItem *_item) { m_item = _item; }
    MeasureItem *measureItem() const { return m_item; }

private:
    MeasureItem *m_item;
};

#endif // MEASURETABLEWIDGETITEM_H
