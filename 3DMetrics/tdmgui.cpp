#include "tdmgui.h"
#include "ui_tdmgui.h"

//#include "TDMLayers/tdmlayer.h"
//#include "TDMLayers/tdmmaplayerregistry.h"

#include "TreeView/tdmlayeritem.h"
#include "TreeView/tdmlayersmodel.h"

#include "filedialog.h"

TDMGui::TDMGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TDMGui)
{
    ui->setupUi(this);
    //    TDMLayerRegistry *reg = TDMLayerRegistry::instance();

    //    reg->addMapLayer(new TDMLayer(TDMLayer::ModelLayer, tr("Models")));

    //    ui->tree_widget->setModel(reg);

    ui->tree_widget->setModel(TdmLayersModel::instance());
    ui->tree_widget->hideColumn(1);

    QObject::connect(ui->open_3d_model_action, SIGNAL(triggered()), this, SLOT(slot_open3dModel()));
    QObject::connect(ui->open_measurement_file_action, SIGNAL(triggered()), this, SLOT(slot_openMeasureFile()));
    QObject::connect(ui->new_group_action, SIGNAL(triggered()), this, SLOT(slot_newGroup()));

    // check state
    QObject::connect(TdmLayersModel::instance(),SIGNAL(signal_checkChanged(TdmLayerItem*)),this,SLOT(slot_checkChanged(TdmLayerItem*)));

    QObject::connect(ui->tree_widget->selectionModel(),
                     SIGNAL(selectionChanged(const QItemSelection &,
                                             const QItemSelection &)),
                     this, SLOT(slot_selectionChanged()));
}

TDMGui::~TDMGui()
{
    delete ui;
}



void TDMGui::slot_open3dModel()
{

    // Problem on Linux Ubuntu : to be replaced
    //
    //    QString model_file = QFileDialog::getOpenFileName(
    //                this,
    //                "Select one 3d Model to open");

    QString fileName = getOpenFileName(this,tr("Select one 3d Model to open"), "", tr("3D files (*.kml *.obj)"));
    if(fileName.length() > 0)
    {
        TdmLayersModel *model = TdmLayersModel::instance(); //static_cast<TreeModel*>(ui->tree_widget->model());
        QFileInfo info(fileName);
        QVariant data(info.fileName());
        QVariant dummy(fileName);

        TdmLayerItem *added = model->addData(TdmLayerItem::ModelLayer, model->rootItem(), data, dummy);
        added->setChecked(true);

        QApplication::setOverrideCursor(Qt::WaitCursor);
        ui->display_widget->setSceneFromFile(fileName.toStdString());
        QApplication::restoreOverrideCursor();
    }
}

void TDMGui::slot_openMeasureFile()
{
    TdmLayersModel *model = TdmLayersModel::instance();
    QVariant data("Dummy MeasureFile");
    QVariant dummy("dummy");
    TdmLayerItem *added = model->addData(TdmLayerItem::MeasurementLayer, model->rootItem(), data, dummy);
    added->setChecked(true);
}

void TDMGui::slot_newGroup()
{
    TdmLayersModel *model = TdmLayersModel::instance();
    QVariant data("Group");
    QVariant dummy("group");
    TdmLayerItem *added = model->addData(TdmLayerItem::GroupLayer, model->rootItem(), data, dummy);
    added->setChecked(false);
}


void TDMGui::slot_selectionChanged()
{
    QTreeView *view = ui->tree_widget;

    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    bool hasCurrent = view->selectionModel()->currentIndex().isValid();

    if (hasCurrent) {
        view->closePersistentEditor(view->selectionModel()->currentIndex());
        TdmLayerItem *selected = TdmLayersModel::instance()->getItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            statusBar()->showMessage(tr("%1 - %2").arg(selected->data(0).toString()).arg(selected->data(1).toString()));
        }
        //        int row = view->selectionModel()->currentIndex().row();
        //        int column = view->selectionModel()->currentIndex().column();
        //        if (view->selectionModel()->currentIndex().parent().isValid())
        //        {
        //            statusBar()->showMessage(tr("Position: (%1,%2)").arg(row).arg(column));
        //        }
        //        else
        //        {
        //            statusBar()->showMessage(tr("Position: (%1,%2) in top level").arg(row).arg(column));
        //        }
    }
}

void TDMGui::slot_checkChanged(TdmLayerItem *item)
{
    bool checked = item->isChecked();
}
