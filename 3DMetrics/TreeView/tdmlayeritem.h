// TreeItem to be used in TreeModel
// Contains : layer type (Model, Measurement, Group)
//            check state
//            data QVector<QVariant>
//              data[0] : QVariant<QString> : displayed name - editable
//              data[1] : QVariant of any kind - not displayed
//

#ifndef TdmLayerItem_H
#define TdmLayerItem_H

#include <QList>
#include <QVariant>
#include <QVector>

class TdmLayerItem
{
public:
    enum LayerType
    {
        ModelLayer,
        MeasurementLayer,
        GroupLayer
    };

    explicit TdmLayerItem(const LayerType _type, const QVector<QVariant> &data, TdmLayerItem *parent = 0);
    ~TdmLayerItem();

    TdmLayerItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool insertChildren(int position, int count, int columns);

    TdmLayerItem *parent();
    bool removeChildren(int position, int count);

    int childNumber() const;
    bool setData(int column, const QVariant &value);

    bool isEditable();

    bool isChecked() const { return m_checked; }
    void setChecked( bool set ) { m_checked = set; }

    // Get the type of the layer
    LayerType type() const { return m_layer_type; }
    QString typeName() const;

    // additions for D&D
    void appendChild(TdmLayerItem *child);
    void insertChild(int pos, TdmLayerItem *child);
    void removeChild(int row);
    int row() const;

private:
    QList<TdmLayerItem*> m_child_items;
    QVector<QVariant> m_item_data;
    TdmLayerItem *m_parent_item;

    bool m_checked;

    // Type of the layer
    LayerType m_layer_type;

};


#endif // TdmLayerItem_H
