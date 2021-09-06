#include "meas_table_widget_item.h"

MeasTableWidgetItem::MeasTableWidgetItem(int _type)
    : QTableWidgetItem(_type), m_item(0)
{
}

MeasTableWidgetItem::~MeasTableWidgetItem()
{
    delete m_item;
}

