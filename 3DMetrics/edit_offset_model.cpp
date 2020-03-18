#include "edit_offset_model.h"
#include "ui_edit_offset_model.h"

#include "TreeView/tdm_layer_item.h"
#include "TreeView/tdm_model_layerdata.h"

#include "OSGWidget/osg_widget.h"

EditOffsetModel::EditOffsetModel(QWidget *parent, TdmLayerItem *_item , OSGWidget *_widget) :
    QDialog(parent),
    ui(new Ui::EditOffsetModel)
{
    m_item = _item;
    m_widget = _widget;
    TDMModelLayerData layer_data = m_item->getPrivateData<TDMModelLayerData>();

    m_node = (layer_data.node().get());

    ui->setupUi(this);

    connect(ui->valid_btn, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->apply_btn, SIGNAL(clicked()), this, SLOT(slot_apply()));

    // allways on top
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowStaysOnTopHint;
    setWindowFlags( flags);
}

EditOffsetModel::~EditOffsetModel()
{
    delete ui;
}

void EditOffsetModel::slot_apply()
{
    // read values
    m_offsetX = ui->spinbox_x->value();
    m_offsetY = ui->spinbox_y->value();
    m_offsetZ = ui->spinbox_z->value();

    m_widget->onMoveNode(m_offsetX, m_offsetY ,m_offsetZ, m_node);
}

void EditOffsetModel::setOffset(const double _x, const double _y, const double _z)
{
    m_offsetX = _x;
    m_offsetY = _y;
    m_offsetZ = _z;

    ui->spinbox_x->setValue(_x);
    ui->spinbox_y->setValue(_y);
    ui->spinbox_z->setValue(_z);
}
