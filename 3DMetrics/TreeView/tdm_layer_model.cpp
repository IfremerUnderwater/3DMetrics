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
    QVector<QVariant> root_data;
    // Needed to create two column...
    root_data << "header";
    root_data << "data"; // not shown but used to store informations

    m_root_item = new TdmLayerItem(TdmLayerItem::GroupLayer, root_data);
    m_root_item->setChecked(true);
}


TdmLayerItem* TdmLayersModel::addLayerItem(const TdmLayerItem::LayerType _type, TdmLayerItem *_parent, QVariant &_displayedName, QVariant &_privateData)
{
    beginInsertRows(QModelIndex(),m_root_item->childCount(),m_root_item->childCount());

    QVector<QVariant> datas;
    datas << _displayedName;
    datas << _privateData;
    TdmLayerItem *item = new TdmLayerItem(_type, datas);
    _parent->insertChild(_parent->childCount(),item);

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

QVariant TdmLayersModel::data(const QModelIndex &_index, int _role) const
{
    if (!_index.isValid())
        return QVariant();


    TdmLayerItem *item1 = static_cast<TdmLayerItem*>(_index.internalPointer());
    TdmLayerItem *item = getLayerItem(_index);

    if ( _role == Qt::CheckStateRole && _index.column() == 0 && item->isEditable())
    {
        return static_cast< int >( item1->isChecked() ? Qt::Checked : Qt::Unchecked );
    }

    if(_role == Qt::DecorationRole)
    {
        QIcon icon;
        switch(item1->type())
        {
        case TdmLayerItem::GroupLayer:
            icon = QIcon::fromTheme(":/icons/ressources/folder.svg");
            break;
        case TdmLayerItem::MeasurementLayer:
            icon = QIcon::fromTheme(":/icons/ressources/layers-outline.svg");
            break;
        case TdmLayerItem::ModelLayer:
            icon = QIcon::fromTheme(":/icons/ressources/image-filter-hdr.svg");
            break;
        }

        QVariant v(icon);

        return v;
    }

    if (_role != Qt::DisplayRole && _role != Qt::EditRole)
        return QVariant();

    return item->data(_index.column());
}

Qt::ItemFlags TdmLayersModel::flags(const QModelIndex &_index) const
{
    if (!_index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled; // to allow drop in toplevel

    Qt::ItemFlags flags = Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;

    TdmLayerItem *item = getLayerItem(_index);
    if(item != 0)
    {
        if(item == m_root_item)
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;

        return (item->type() == TdmLayerItem::GroupLayer ? Qt::ItemIsDropEnabled : Qt::NoItemFlags) | flags;
    }
    return Qt::ItemIsEditable | Qt::ItemIsDropEnabled | Qt::ItemIsUserCheckable | QAbstractItemModel::flags(_index);
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

QVariant TdmLayersModel::headerData(int _section, Qt::Orientation _orientation,
                                    int _role) const
{
    if (_orientation == Qt::Horizontal && _role == Qt::DisplayRole)
        return m_root_item->data(_section);

    return QVariant();
}

QModelIndex TdmLayersModel::index(int _row, int _column, const QModelIndex &_parent) const
{
    if (_parent.isValid() && _parent.column() != 0)
        return QModelIndex();

    TdmLayerItem *parent_item = getLayerItem(_parent);

    TdmLayerItem *child_item = parent_item->child(_row);
    if (child_item)
        return createIndex(_row, _column, child_item);
    else
        return QModelIndex();
}

bool TdmLayersModel::insertRows(int _position, int _rows, const QModelIndex &_parent)
{
    TdmLayerItem *parent_item = getLayerItem(_parent);
    bool success;

    beginInsertRows(_parent, _position, _position + _rows - 1);
    success = parent_item->insertChildren(_position, _rows, m_root_item->columnCount());
    endInsertRows();

    return success;
}

QModelIndex TdmLayersModel::parent(const QModelIndex &_index) const
{
    if (!_index.isValid())
        return QModelIndex();

    TdmLayerItem *child_item = getLayerItem(_index);
    TdmLayerItem *parent_item = child_item->parent();

    if (parent_item == m_root_item)
        return QModelIndex();

    return createIndex(parent_item->childNumber(), 0, parent_item);
}

bool TdmLayersModel::removeRows(int _position, int _rows, const QModelIndex &_parent)
{
    TdmLayerItem *parent_item = getLayerItem(_parent);
    bool success = true;

    beginRemoveRows(_parent, _position, _position + _rows - 1);
    success = parent_item->removeChildren(_position, _rows);
    endRemoveRows();

    return success;
}

int TdmLayersModel::rowCount(const QModelIndex &_parent) const
{
    TdmLayerItem *parent_item = getLayerItem(_parent);

    return parent_item->childCount();
}

bool TdmLayersModel::setData(const QModelIndex &_index, const QVariant &_value, int _role)
{
    TdmLayerItem *item = getLayerItem(_index);

    if(_role == Qt::CheckStateRole)
    {
        if(!item->isEditable())
            return false;

        if(item->isChecked())
            item->setChecked(false);
        else
            item->setChecked(true);

        emit dataChanged(_index, _index);
        emit signal_checkChanged(item);

        return true;
    }

    if (_role != Qt::EditRole)
        return false;

    bool result = item->setData(_index.column(), _value);

    if (result)
        emit dataChanged(_index, _index);

    return result;
}

bool TdmLayersModel::setHeaderData(int _section, Qt::Orientation _orientation,
                                   const QVariant &_value, int _role)
{
    if (_role != Qt::EditRole || _orientation != Qt::Horizontal)
        return false;

    bool result = m_root_item->setData(_section, _value);

    if (result)
        emit headerDataChanged(_orientation, _section, _section);

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
QMimeData *TdmLayersModel::mimeData(const QModelIndexList &_indexes) const
{
    QMimeData *mime_data = new QMimeData;
    QByteArray data; //a kind of RAW format for datas

    //QDataStream is independant on the OS or proc architecture
    //serialization of C++'s basic data types, like char, short, int, char *, etc.
    //Serialization of more complex data is accomplished
    //by breaking up the data into primitive units.
    QDataStream stream(&data, QIODevice::WriteOnly);
    QList<TdmLayerItem *> nodes;

    //
    foreach (const QModelIndex &index, _indexes) {
        TdmLayerItem *node = getLayerItem(index);
        if (!nodes.contains(node))
            nodes << node;
    }
    stream << QCoreApplication::applicationPid();
    stream << nodes.count();
    foreach (TdmLayerItem *node, nodes) {
        stream << reinterpret_cast<qlonglong>(node);
    }
    mime_data->setData(s_treeNodeMimeType, data);
    return mime_data;
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
    qint64 sender_pid;
    stream >> sender_pid;
    if (sender_pid != QCoreApplication::applicationPid()) {
        // Let's not cast pointers that come from another process...
        return false;
    }
    TdmLayerItem *parent_node = getLayerItem(_parent);
    Q_ASSERT(parent_node);
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
        qlonglong node_ptr;
        stream >> node_ptr;
        TdmLayerItem *node = reinterpret_cast<TdmLayerItem *>(node_ptr);

        // Adjust destination row for the case of moving an item
        // within the same parent, to a position further down.
        // Its own removal will reduce the final row number by one.
        if (node->row() < _row && parent_node == node->parent())
            --_row;

        // Remove from old position
        removeNode(node);

        // Insert at new position
        //qDebug() << "Inserting into" << parent << row;
        beginInsertRows(_parent, _row, _row);
        parent_node->insertChild(_row, node);
        endInsertRows();

        // notify dropped item
        emit signal_itemDropped(node);

        ++_row;
    }
    return true;
}

void TdmLayersModel::removeNode(TdmLayerItem *_node)
{
    const int row = _node->row();
    QModelIndex idx = createIndex(row, 0, _node);
    beginRemoveRows(idx.parent(), row, row);
    _node->parent()->removeChild(row);
    endRemoveRows();
}
