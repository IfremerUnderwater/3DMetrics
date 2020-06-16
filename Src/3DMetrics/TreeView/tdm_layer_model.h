#ifndef TDM_LAYER_MODEL_H
#define TDM_LAYER_MODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include "TreeView/tdm_layer_item.h"

class TdmLayersModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    // singleton
    static TdmLayersModel *instance() { return s_instance; }

    ~TdmLayersModel();

    QVariant data(const QModelIndex &_index, int _role) const Q_DECL_OVERRIDE;
    QVariant headerData(int _section, Qt::Orientation _orientation,
                        int _role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    QModelIndex index(int _row, int _column,
                      const QModelIndex &_parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &_index) const Q_DECL_OVERRIDE;

    QModelIndex index(TdmLayerItem *_item)
    {
        return createIndex(_item->childNumber(), 0, _item);
    }

    int rowCount(const QModelIndex &_parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &_parent = QModelIndex()) const Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(const QModelIndex &_index) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &_index, const QVariant &_value,
                 int _role = Qt::EditRole) Q_DECL_OVERRIDE;
    bool setHeaderData(int _section, Qt::Orientation _orientation,
                       const QVariant &_value, int _role = Qt::EditRole) Q_DECL_OVERRIDE;

    bool insertRows(int _position, int _rows,
                    const QModelIndex &_parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int _position, int _rows,
                    const QModelIndex &_parent = QModelIndex()) Q_DECL_OVERRIDE;

    // append a new data in tree at specified level
    TdmLayerItem* addLayerItem(const TdmLayerItem::LayerType _type, TdmLayerItem *_parent, QVariant &_displayedName, QVariant &_privateData);

    TdmLayerItem* rootItem() { return m_root_item; }

    // Drag & Drop
    QStringList mimeTypes() const Q_DECL_OVERRIDE;
    QMimeData *mimeData(const QModelIndexList &_indexes) const Q_DECL_OVERRIDE;
    bool dropMimeData(const QMimeData* _data, Qt::DropAction _action, int _row, int _column, const QModelIndex & _parent);

    virtual Qt::DropActions supportedDropActions() const;

    TdmLayerItem *getLayerItem(const QModelIndex &_index) const;

    // for Drag & Drop
    void removeNode(TdmLayerItem *_node);

signals:
    void signal_checkChanged(TdmLayerItem*);
    void signal_itemDropped(TdmLayerItem*);

private:
    // singleton
    TdmLayersModel(QObject *_parent = 0);
    static TdmLayersModel *s_instance;

    TdmLayerItem *m_root_item;
};

#endif // TDM_LAYER_MODEL_H
