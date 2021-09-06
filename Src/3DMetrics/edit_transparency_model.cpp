#include <string>

#include "edit_transparency_model.h"
#include "ui_edit_transparency_model.h"

#include "TreeView/tdm_layer_item.h"
#include "TreeView/tdm_model_layerdata.h"

#include "OSGWidget/osg_widget.h"

EditTransparencyModel::EditTransparencyModel(QWidget *parent, TdmLayerItem *_item, OSGWidget *_widget) :
    QDialog(parent),
    ui(new Ui::EditTransparencyModel), m_transparency_value(0)
{
    m_item = _item;
    m_widget = _widget;
    TDMModelLayerData layer_data = m_item->getPrivateData<TDMModelLayerData>();

    m_node = (layer_data.node().get());

    ui->setupUi(this);
    ui->transparency_label->setText(QString::number(m_transparency_value));
    ui->transparency_value_slider->setSliderPosition(m_transparency_value);
    ui->transparency_value_slider->setTickInterval(100);
    ui->transparency_value_slider->setSingleStep(1);


    connect(ui->transparency_value_slider,SIGNAL(valueChanged(int)), this, SLOT(slot_changeTransparencyValue(int)));
    connect(ui->valid_btn, SIGNAL(clicked()), this, SLOT(accept()));

    // allways on top
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowStaysOnTopHint;
    setWindowFlags( flags);
}

EditTransparencyModel::~EditTransparencyModel()
{
    delete ui;
}

void EditTransparencyModel::slot_changeTransparencyValue(int _transparency_value)
{
    double double_transparency = ( (double)_transparency_value )/100.0;
    setTransparency(double_transparency);

    if(m_node == nullptr)
    {
        close();
        return;
    }
    m_widget->setNodeTransparency(m_node, double_transparency);

    TDMModelLayerData layer_data = m_item->getPrivateData<TDMModelLayerData>();
    layer_data.setTransparencyValue(double_transparency);
    m_item->setPrivateData<TDMModelLayerData>(layer_data);
}

void EditTransparencyModel::setTransparency(double _transparency_value)
{
    m_transparency_value = _transparency_value;
    ui->transparency_label->setText(QString::number((int)(100*m_transparency_value)));
    ui->transparency_value_slider->setSliderPosition((int)(100*m_transparency_value));
}

