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

#include <QtWidgets>

#include "tdm_layer_item.h"
#include "tdm_layer_model.h"

TdmLayersModel *TdmLayersModel::s_instance = new TdmLayersModel();

TdmLayersModel::TdmLayersModel(QObject *_parent)
    : QAbstractItemModel(_parent)
{
    QVector<QVariant> rootData;
    // Needed to create two column...
    rootData << "header";
    rootData << "data"; // not shown but used to store informations

    m_root_item = new TdmLayerItem(TdmLayerItem::GroupLayer, rootData);
    m_root_item->setChecked(true);
}


TdmLayerItem* TdmLayersModel::addLayerItem(const TdmLayerItem::LayerType _type, TdmLayerItem *parent, QVariant &_displayedName, QVariant &_privateData)
{
    beginInsertRows(QModelIndex(),m_root_item->childCount(),m_root_item->childCount());

    QVector<QVariant> datas;
    datas << _displayedName;
    datas << _privateData;
    TdmLayerItem *item = new TdmLayerItem(_type, datas);
    parent->insertChild(parent->childCount(),item);

    endInsertRows();

    return item;
}

TdmLayersModel::~TdmLayersModel()
{
    delete m_root_item;
}

int TdmLayersModel::columnCount(const QModelIndex & /* parent */) const
{
    // 2 colums (1 hidden used to store datas)
    return m_root_item->columnCount();
}

QVariant TdmLayersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();


    TdmLayerItem *item1 = static_cast<TdmLayerItem*>(index.internalPointer());
    TdmLayerItem *item = getLayerItem(index);

    if ( role == Qt::CheckStateRole && index.column() == 0 && item->isEditable())
    {
        return static_cast< int >( item1->isChecked() ? Qt::Checked : Qt::Unchecked );
    }

    if(role == Qt::DecorationRole)
    {
        QIcon icon; // = QIcon::fromTheme("edit-undo");
        switch(item1->type())
        {
        case TdmLayerItem::GroupLayer:
            icon = QIcon::fromTheme("folder-new");
            break;
        case TdmLayerItem::MeasurementLayer:
            icon = QIcon::fromTheme("document-properties");
            break;
        case TdmLayerItem::ModelLayer:
            icon = QIcon::fromTheme("document-open");
            break;
        }

        QVariant v(icon);

        return v;
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    return item->data(index.column());
}

Qt::ItemFlags TdmLayersModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled; // to allow drop in toplevel

    Qt::ItemFlags flags = Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;

    TdmLayerItem *item = getLayerItem(index);
    if(item != 0)
    {
        if(item == m_root_item)
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;

        return (item->type() == TdmLayerItem::GroupLayer ? Qt::ItemIsDropEnabled : Qt::NoItemFlags) | flags;
    }
    return Qt::ItemIsEditable | Qt::ItemIsDropEnabled | Qt::ItemIsUserCheckable | QAbstractItemModel::flags(index);
}

TdmLayerItem *TdmLayersModel::getLayerItem(const QModelIndex &_index) const
{
    if (_index.isValid()) {
        TdmLayerItem *item = static_cast<TdmLayerItem*>(_index.internalPointer());
        if (item)
            return item;
    }
    return m_root_item;
}

QVariant TdmLayersModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_root_item->data(section);

    return QVariant();
}

QModelIndex TdmLayersModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    TdmLayerItem *parentItem = getLayerItem(parent);

    TdmLayerItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

bool TdmLayersModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    TdmLayerItem *parentItem = getLayerItem(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, m_root_item->columnCount());
    endInsertRows();

    return success;
}

QModelIndex TdmLayersModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TdmLayerItem *childItem = getLayerItem(index);
    TdmLayerItem *parentItem = childItem->parent();

    if (parentItem == m_root_item)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool TdmLayersModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    TdmLayerItem *parentItem = getLayerItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

int TdmLayersModel::rowCount(const QModelIndex &parent) const
{
    TdmLayerItem *parentItem = getLayerItem(parent);

    return parentItem->childCount();
}

bool TdmLayersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    TdmLayerItem *item = getLayerItem(index);

    if(role == Qt::CheckStateRole)
    {
        if(!item->isEditable())
            return false;

        if(item->isChecked())
            item->setChecked(false);
        else
            item->setChecked(true);

        emit dataChanged(index, index);
        emit signal_checkChanged(item);

        return true;
    }

    if (role != Qt::EditRole)
        return false;

    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index);

    return result;
}

bool TdmLayersModel::setHeaderData(int section, Qt::Orientation orientation,
                                   const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = m_root_item->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

Qt::DropActions TdmLayersModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

static const char s_treeNodeMimeType[] = "application/x-treenode";
//returns the mime type
QStringList TdmLayersModel::mimeTypes() const
{
    return QStringList() << s_treeNodeMimeType;
}

//receives a list of model indexes list
QMimeData *TdmLayersModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData;
    QByteArray data; //a kind of RAW format for datas

    //QDataStream is independant on the OS or proc architecture
    //serialization of C++'s basic data types, like char, short, int, char *, etc.
    //Serialization of more complex data is accomplished
    //by breaking up the data into primitive units.
    QDataStream stream(&data, QIODevice::WriteOnly);
    QList<TdmLayerItem *> nodes;

    //
    foreach (const QModelIndex &index, indexes) {
        TdmLayerItem *node = getLayerItem(index);
        if (!nodes.contains(node))
            nodes << node;
    }
    stream << QCoreApplication::applicationPid();
    stream << nodes.count();
    foreach (TdmLayerItem *node, nodes) {
        stream << reinterpret_cast<qlonglong>(node);
    }
    mimeData->setData(s_treeNodeMimeType, data);
    return mimeData;
}

bool TdmLayersModel::dropMimeData(const QMimeData *_data, Qt::DropAction _action, int _row, int _column, const QModelIndex &_parent)
{
    Q_ASSERT(_action == Qt::MoveAction);
    Q_UNUSED(_column);
    //test if the data type is the good one
    if (!_data->hasFormat(s_treeNodeMimeType)) {
        return false;
    }
    QByteArray data = _data->data(s_treeNodeMimeType);
    QDataStream stream(&data, QIODevice::ReadOnly);
    qint64 senderPid;
    stream >> senderPid;
    if (senderPid != QCoreApplication::applicationPid()) {
        // Let's not cast pointers that come from another process...
        return false;
    }
    TdmLayerItem *parentNode = getLayerItem(_parent);
    Q_ASSERT(parentNode);
    int count;
    stream >> count;
    if (_row == -1) {
        // valid index means: drop onto item. I chose that this should insert
        // a child item, because this is the only way to create the first child of an item...
        // This explains why Qt calls it parent: unless you just support replacing, this
        // is really the future parent of the dropped items.
        if (_parent.isValid())
            _row = 0;
        else
            // invalid index means: append at bottom, after last toplevel
            _row = rowCount(_parent);
    }
    for (int i = 0; i < count; ++i) {
        // Decode data from the QMimeData
        qlonglong nodePtr;
        stream >> nodePtr;
        TdmLayerItem *node = reinterpret_cast<TdmLayerItem *>(nodePtr);

        // Adjust destination row for the case of moving an item
        // within the same parent, to a position further down.
        // Its own removal will reduce the final row number by one.
        if (node->row() < _row && parentNode == node->parent())
            --_row;

        // Remove from old position
        removeNode(node);

        // Insert at new position
        //qDebug() << "Inserting into" << parent << row;
        beginInsertRows(_parent, _row, _row);
        parentNode->insertChild(_row, node);
        endInsertRows();

        // notify dropped item
        emit signal_itemDropped(node);

        ++_row;
    }
    return true;
}

void TdmLayersModel::removeNode(TdmLayerItem *node)
{
    const int row = node->row();
    QModelIndex idx = createIndex(row, 0, node);
    beginRemoveRows(idx.parent(), row, row);
    node->parent()->removeChild(row);
    endRemoveRows();
}