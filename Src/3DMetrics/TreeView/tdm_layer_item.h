// TreeItem to be used in TreeModel
// Contains : layer type (Model, Measurement, Group)
//            check state
//            data QVector<QVariant>
//              data[0] : QVariant<QString> : displayed name - editable
//              data[1] : QVariant of any kind - not displayed
//

#ifndef TDM_LAYER_ITEM_H
#define TDM_LAYER_ITEM_H

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

    explicit TdmLayerItem(const LayerType _type, const QVector<QVariant> &_data, TdmLayerItem *_parent = 0);
    ~TdmLayerItem();

    TdmLayerItem *child(int _number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int _column) const;
    bool insertChildren(int _position, int _count, int _columns);

    TdmLayerItem *parent();
    bool removeChildren(int _position, int _count);

    int childNumber() const;
    bool setData(int _column, const QVariant &_value);

    bool isEditable();

    bool isChecked() const { return m_checked; }
    void setChecked( bool set ) { m_checked = set; }

    // Get the type of the layer
    LayerType type() const { return m_layer_type; }
    QString typeName() const;

    // additions for D&D
    void appendChild(TdmLayerItem *_child);
    void insertChild(int _pos, TdmLayerItem *_child);
    void removeChild(int _row);
    int row() const;

    // displayed name
    QString getName();
    // file name (may be empty)
    QString getFileName();

    // private data accessor
    template<class privateDataClass>
    privateDataClass getPrivateData()
    {
        QVariant data1 = data(1);
        return data1.value<privateDataClass>();
    }

    template<class privateDataClass>
    void setPrivateData(privateDataClass _value)
    {
        QVariant data1;
        data1.setValue<privateDataClass>(_value);
        setData(1, data1);
    }

    template<class privateDataClass>
    bool hasData()
    {
        QVariant data1 = data(1);
        return data1.canConvert<privateDataClass>();
    }

private:
    QList<TdmLayerItem*> m_child_items;
    QVector<QVariant> m_item_data;
    TdmLayerItem *m_parent_item;

    bool m_checked;

    // Type of the layer
    LayerType m_layer_type;

};


#endif // TDM_LAYER_ITEM_H
