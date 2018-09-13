#ifndef TdmLayersModel_H
#define TdmLayersModel_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include "TreeView/tdmlayeritem.h"

class TdmLayersModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    // singleton
    static TdmLayersModel *instance() { return s_instance; }

    ~TdmLayersModel();

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) Q_DECL_OVERRIDE;
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    bool insertRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;

    // append a new data in tree at specified level
    TdmLayerItem* addData(const TdmLayerItem::LayerType _type, TdmLayerItem *parent, QVariant &displayedName, QVariant &privateData);
    TdmLayerItem* rootItem() { return m_root_item; }

    // Drag & Drop
    QStringList mimeTypes() const Q_DECL_OVERRIDE;
    QMimeData *mimeData(const QModelIndexList &indexes) const Q_DECL_OVERRIDE;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex & parent);

    virtual Qt::DropActions supportedDropActions() const;

    TdmLayerItem *getItem(const QModelIndex &index) const;
signals:
    void signal_checkChanged(TdmLayerItem*);

private:
    // singleton
    TdmLayersModel(QObject *parent = 0);
    static TdmLayersModel *s_instance;

    // for Drag & Drop
    void removeNode(TdmLayerItem *node);

    TdmLayerItem *m_root_item;
};

#endif // TdmLayersModel_H
