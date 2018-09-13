/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
    treeitem.cpp

    A container for items of data supplied by the simple tree model.
*/

#include "tdmlayeritem.h"

#include <QStringList>


TdmLayerItem::TdmLayerItem(const LayerType _type, const QVector<QVariant> &data, TdmLayerItem *parent)
{
    m_parent_item = parent;
    m_item_data = data;
    m_checked = false;
    m_layer_type = _type;
}


TdmLayerItem::~TdmLayerItem()
{
    qDeleteAll(m_child_items);
}

TdmLayerItem *TdmLayerItem::child(int number)
{
    return m_child_items.value(number);
}

int TdmLayerItem::childCount() const
{
    return m_child_items.count();
}

int TdmLayerItem::childNumber() const
{
    if (m_parent_item)
        return m_parent_item->m_child_items.indexOf(const_cast<TdmLayerItem*>(this));

    return 0;
}

int TdmLayerItem::columnCount() const
{
    return m_item_data.count();
}

QVariant TdmLayerItem::data(int column) const
{
    return m_item_data.value(column);
}

bool TdmLayerItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > m_child_items.size())
        return false;

    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns);
        TdmLayerItem *item = new TdmLayerItem(TdmLayerItem::GroupLayer, data, this);
        m_child_items.insert(position, item);
    }

    return true;
}

TdmLayerItem *TdmLayerItem::parent()
{
    return m_parent_item;
}

bool TdmLayerItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > m_child_items.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete m_child_items.takeAt(position);

    return true;
}

bool TdmLayerItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= m_item_data.size())
        return false;

    m_item_data[column] = value;
    return true;
}

bool TdmLayerItem::isEditable()
{
    return true; //return m_parent_item->parent() != 0;
}

void TdmLayerItem::removeChild(int row)
{
    m_child_items.removeAt(row);
}

void TdmLayerItem::appendChild(TdmLayerItem *node)
{
    m_child_items.append(node);
}

int TdmLayerItem::row() const
{
    if (m_parent_item)
        return m_parent_item->m_child_items.indexOf(const_cast<TdmLayerItem*>(this));

    return 0;
}

void TdmLayerItem::insertChild(int pos, TdmLayerItem *child)
{
    m_child_items.insert(pos, child);
    child->m_parent_item = this;
}
