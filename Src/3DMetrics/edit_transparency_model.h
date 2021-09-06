#ifndef EDIT_TRANSPARENCY_MODEL_H
#define EDIT_TRANSPARENCY_MODEL_H

#include <QDialog>

namespace Ui {
class EditTransparencyModel;
}

namespace osg {
class Node;
}

class TdmLayerItem;
class OSGWidget;

class EditTransparencyModel : public QDialog
{
    Q_OBJECT

public:
    explicit EditTransparencyModel(QWidget *parent, TdmLayerItem *_item , OSGWidget *_widget);
    ~EditTransparencyModel();

    double getTransparencyValue() { return m_transparency_value; }
    void setTransparency(double _transparency_value);

private:
    Ui::EditTransparencyModel *ui;
    double m_transparency_value;

    TdmLayerItem *m_item;
    OSGWidget *m_widget;
    osg::Node* m_node;

private slots:
    void slot_changeTransparencyValue(int);
};

#endif // EDIT_TRANSPARENCY_MODEL_H
