#ifndef EDIT_OFFSET_MODEL_H
#define EDIT_OFFSET_MODEL_H

#include <QDialog>

namespace Ui {
class EditOffsetModel;
}


namespace osg {
class Node;
}

class TdmLayerItem;
class OSGWidget;

class EditOffsetModel : public QDialog
{
    Q_OBJECT

public:
    explicit EditOffsetModel(QWidget *parent, TdmLayerItem *_item , OSGWidget *_widget);
    ~EditOffsetModel();

    double getOffsetX() const { return m_offsetX; }
    double getOffsetY() const { return m_offsetY; }
    double getOffsetZ() const { return m_offsetZ; }

    void setOffset(const double _x, const double _y, const double _z);

private:
    Ui::EditOffsetModel *ui;

    double m_offsetX;
    double m_offsetY;
    double m_offsetZ;

    TdmLayerItem *m_item;
    OSGWidget *m_widget;
    osg::Node* m_node;

private slots:
    void slot_apply();
};

#endif // EDIT_OFFSET_MODEL_H
