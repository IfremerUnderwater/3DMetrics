#include "measuretablewidgetitem.h"

MeasureTableWidgetItem::MeasureTableWidgetItem(int type)
    : QTableWidgetItem(type), m_item(0)
{
}

MeasureTableWidgetItem::~MeasureTableWidgetItem()
{
    delete m_item;
}

