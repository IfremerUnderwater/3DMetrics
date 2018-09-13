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

#include "treeitem.h"
#include "treemodel.h"

TreeModel *TreeModel::s_instance = new TreeModel();

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    // Needed to create one column...
    rootData << "header";
    rootData << "data"; // not shown but to store informations

    m_root_item = new TreeItem(TreeItem::GroupLayer, rootData);

//    m_root_item->insertChildren(m_root_item->childCount(), 1, m_root_item->columnCount());
//    m_models_item = m_root_item->child(m_root_item->childCount() - 1);
//    QVariant qv1(tr("Models"));
//    m_models_item->setData(0, qv1);

//    m_root_item->insertChildren(m_root_item->childCount(), 1, m_root_item->columnCount());
//    m_measures_item = m_root_item->child(m_root_item->childCount() - 1);
//    QVariant qv2(tr("Measures"));
//    m_measures_item->setData(0, qv2);

//    m_root_item->insertChildren(m_root_item->childCount(), 1, m_root_item->columnCount());
//    m_groups_item = m_root_item->child(m_root_item->childCount() - 1);
//    QVariant qv3(tr("Groups"));
//    m_groups_item->setData(0, qv3);
}


TreeItem* TreeModel::addData(const TreeItem::LayerType _type, TreeItem *parent, QVariant &col0, QVariant &col1)
{
    beginInsertRows(QModelIndex(),m_root_item->childCount(),m_root_item->childCount());

    QVector<QVariant> datas;
    datas << col0;
    datas << col1;
    TreeItem *item = new TreeItem(_type, datas);
    parent->insertChild(parent->childCount(),item);

//    bool result = m_root_item->insertChildren(m_root_item->childCount(), 1, m_root_item->columnCount());
//    TreeItem *item = m_root_item->child(m_root_item->childCount() - 1);
//    item->setData(0, col0);
//    item->setData(1, col1);
    endInsertRows();

    return item;
}

TreeModel::~TreeModel()
{
    delete m_root_item;
}

int TreeModel::columnCount(const QModelIndex & /* parent */) const
{
    return m_root_item->columnCount();
    //return 1; // 1 column for display
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();


    TreeItem *item1 = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *item = getItem(index);

    if ( role == Qt::CheckStateRole && index.column() == 0 && item->isEditable())
    {
        return static_cast< int >( item1->isChecked() ? Qt::Checked : Qt::Unchecked );
    }

    if(role == Qt::DecorationRole)
    {
        QIcon icon; // = QIcon::fromTheme("edit-undo");
        switch(item1->type())
        {
        case TreeItem::GroupLayer:
            icon = QIcon::fromTheme("folder-new");
            break;
        case TreeItem::MeasurementLayer:
            icon = QIcon::fromTheme("document-properties");
            break;
        case TreeItem::ModelLayer:
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

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags flags = Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;

    TreeItem *item = getItem(index);
    if(item != 0)
    {
        return (item->type() == TreeItem::GroupLayer ? Qt::ItemIsDropEnabled : Qt::NoItemFlags) | flags;
    }
    return Qt::ItemIsEditable | Qt::ItemIsDropEnabled | Qt::ItemIsUserCheckable | QAbstractItemModel::flags(index);
}

TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return m_root_item;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_root_item->data(section);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    TreeItem *parentItem = getItem(parent);

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChildren(position, rows, m_root_item->columnCount());
    endInsertRows();

    return success;
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = getItem(index);
    TreeItem *parentItem = childItem->parent();

    if (parentItem == m_root_item)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem = getItem(parent);

    return parentItem->childCount();
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    TreeItem *item = getItem(index);

    if(role == Qt::CheckStateRole)
    {
        //qDebug()<<"Ischecked"<<item->isChecked();
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

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = m_root_item->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

Qt::DropActions TreeModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

static const char s_treeNodeMimeType[] = "application/x-treenode";
//returns the mime type
QStringList TreeModel::mimeTypes() const
{
    return QStringList() << s_treeNodeMimeType;
}

//receives a list of model indexes list
QMimeData *TreeModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData;
    QByteArray data; //a kind of RAW format for datas

    //QDataStream is independant on the OS or proc architecture
    //serialization of C++'s basic data types, like char, short, int, char *, etc.
    //Serialization of more complex data is accomplished
    //by breaking up the data into primitive units.
    QDataStream stream(&data, QIODevice::WriteOnly);
    QList<TreeItem *> nodes;

    //
    foreach (const QModelIndex &index, indexes) {
        TreeItem *node = getItem(index);
        if (!nodes.contains(node))
            nodes << node;
    }
    stream << QCoreApplication::applicationPid();
    stream << nodes.count();
    foreach (TreeItem *node, nodes) {
        stream << reinterpret_cast<qlonglong>(node);
    }
    mimeData->setData(s_treeNodeMimeType, data);
    return mimeData;
}

bool TreeModel::dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_ASSERT(action == Qt::MoveAction);
    Q_UNUSED(column);
    //test if the data type is the good one
    if (!mimeData->hasFormat(s_treeNodeMimeType)) {
        return false;
    }
    QByteArray data = mimeData->data(s_treeNodeMimeType);
    QDataStream stream(&data, QIODevice::ReadOnly);
    qint64 senderPid;
    stream >> senderPid;
    if (senderPid != QCoreApplication::applicationPid()) {
        // Let's not cast pointers that come from another process...
        return false;
    }
    TreeItem *parentNode = getItem(parent);
    Q_ASSERT(parentNode);
    int count;
    stream >> count;
    if (row == -1) {
        // valid index means: drop onto item. I chose that this should insert
        // a child item, because this is the only way to create the first child of an item...
        // This explains why Qt calls it parent: unless you just support replacing, this
        // is really the future parent of the dropped items.
        if (parent.isValid())
            row = 0;
        else
            // invalid index means: append at bottom, after last toplevel
            row = rowCount(parent);
    }
    for (int i = 0; i < count; ++i) {
        // Decode data from the QMimeData
        qlonglong nodePtr;
        stream >> nodePtr;
        TreeItem *node = reinterpret_cast<TreeItem *>(nodePtr);

        // Adjust destination row for the case of moving an item
        // within the same parent, to a position further down.
        // Its own removal will reduce the final row number by one.
        if (node->row() < row && parentNode == node->parent())
            --row;

        // Remove from old position
        removeNode(node);

        // Insert at new position
        //qDebug() << "Inserting into" << parent << row;
        beginInsertRows(parent, row, row);
        parentNode->insertChild(row, node);
        endInsertRows();
        ++row;
    }
    return true;
}

void TreeModel::removeNode(TreeItem *node)
{
    const int row = node->row();
    QModelIndex idx = createIndex(row, 0, node);
    beginRemoveRows(idx.parent(), row, row);
    node->parent()->removeChild(row);
    endRemoveRows();
}
