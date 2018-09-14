#include "tdmgui.h"
#include "ui_tdmgui.h"

//#include "TDMLayers/tdmlayer.h"
//#include "TDMLayers/tdmmaplayerregistry.h"

#include "TreeView/tdmlayeritem.h"
#include "TreeView/tdmlayersmodel.h"

#include "TreeView/tdmmodellayerdata.h"

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

    // check state on e treeview item
    QObject::connect(TdmLayersModel::instance(),SIGNAL(signal_checkChanged(TdmLayerItem*)),this,SLOT(slot_checkChanged(TdmLayerItem*)));

    // treeview selection changed
    QObject::connect(ui->tree_widget->selectionModel(),
                     SIGNAL(selectionChanged(const QItemSelection &,
                                             const QItemSelection &)),
                     this, SLOT(slot_selectionChanged()));

    // treeview contextuel menu
    ui->tree_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tree_widget,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(slot_contextMenu(const QPoint &)));
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

        QApplication::setOverrideCursor(Qt::WaitCursor);

        osg::ref_ptr<osg::Node> node = ui->display_widget->createNodeFromFile(fileName.toStdString());

        TDMModelLayerData modelData(fileName, node);

        TdmLayersModel *model = TdmLayersModel::instance(); //static_cast<TreeModel*>(ui->tree_widget->model());
        QFileInfo info(fileName);
        QVariant name(info.fileName());
        QVariant data;
        data.setValue(modelData);

        TdmLayerItem *added = model->addData(TdmLayerItem::ModelLayer, model->rootItem(), name, data);
        added->setChecked(true);

        ui->display_widget->addNodeToScene(node);

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
    TdmLayerItem *parent = model->rootItem();

    QTreeView *view = ui->tree_widget;
    bool hasSelection = !view->selectionModel()->selection().isEmpty();

    bool hasCurrent = view->selectionModel()->currentIndex().isValid();
    if (hasSelection && hasCurrent)
    {
        TdmLayerItem *selected = TdmLayersModel::instance()->getItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr && selected->type() == TdmLayerItem::GroupLayer)
        {
            parent = selected;
        }
    }

    QVariant data("Group");
    QVariant dummy("group");
    TdmLayerItem *added = model->addData(TdmLayerItem::GroupLayer, parent, data, dummy);
    added->setChecked(false);

    slot_unselect();
}


void TDMGui::slot_selectionChanged()
{
    QTreeView *view = ui->tree_widget;

    //bool hasSelection = !view->selectionModel()->selection().isEmpty();
    bool hasCurrent = view->selectionModel()->currentIndex().isValid();

    if (hasCurrent) {
        view->closePersistentEditor(view->selectionModel()->currentIndex());
        TdmLayerItem *selected = TdmLayersModel::instance()->getItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            QVariant data1 = selected->data(1);
            TDMModelLayerData lda = data1.value<TDMModelLayerData>();
            if(lda.fileName().length())
                data1 = lda.fileName();
            statusBar()->showMessage(tr("%1 - %2").arg(selected->data(0).toString()).arg(data1.toString()));
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
    QVariant data1 = item->data(1);
    TDMModelLayerData lda = data1.value<TDMModelLayerData>();
    if(lda.fileName().length() > 0)
    {
        lda.node()->setNodeMask(checked ? 0xFFFFFFFF : 0);
    }
}

void TDMGui::slot_contextMenu(const QPoint &)
{
    QMenu *menu = new QMenu;
    QTreeView *view = ui->tree_widget;

    bool hasCurrent = view->selectionModel()->currentIndex().isValid();
    if(!hasCurrent)
    {
        menu->addAction(tr("Create new group"), this, SLOT(slot_newGroup()));
        menu->exec(QCursor::pos());
        return;
    }

    view->closePersistentEditor(view->selectionModel()->currentIndex());

    menu->addAction(tr("Delete item"), this, SLOT(slot_deleteRow()));
    menu->addAction(tr("Move item to toplevel"), this, SLOT(slot_moveToToplevel()));
    menu->addSeparator();
    menu->addAction(tr("Create new group"), this, SLOT(slot_newGroup()));
    menu->addSeparator();
    menu->addAction(tr("Unselect"), this, SLOT(slot_unselect()));

    menu->exec(QCursor::pos());
}

// recursively delete datas (in column 1 - not shown in treeview)
void TDMGui::deleteTreeItemsData(TdmLayerItem *item)
{
    if(item == nullptr)
        return;

    if(item->type() == TdmLayerItem::ModelLayer)
    {
        // delete node in osgwidget
        QVariant data1 = item->data(1);
        TDMModelLayerData lda = data1.value<TDMModelLayerData>();
        ui->display_widget->removeNodeFromScene(lda.node());
    }
    if(item->type() == TdmLayerItem::MeasurementLayer)
    {
        //*** todo
    }
    if(item->type() == TdmLayerItem::GroupLayer)
    {
        for(int i=0; i<item->childCount(); i++)
            deleteTreeItemsData(item->child(i));
    }
}


void TDMGui::slot_deleteRow()
{
    QTreeView *view = ui->tree_widget;

    //bool hasSelection = !view->selectionModel()->selection().isEmpty();
    bool hasCurrent = view->selectionModel()->currentIndex().isValid();

    if (hasCurrent)
    {
        view->closePersistentEditor(view->selectionModel()->currentIndex());

        QModelIndex index = view->selectionModel()->currentIndex();
        QAbstractItemModel *model = view->model();
        TdmLayerItem *item = (static_cast<TdmLayersModel*>(model))->getItem(index);
        deleteTreeItemsData(item);
        // delete node in view
        model->removeRow(index.row(), index.parent());
    }
}

void TDMGui::slot_moveToToplevel()
{
    QTreeView *view = ui->tree_widget;

    bool hasCurrent = view->selectionModel()->currentIndex().isValid();

    if (hasCurrent)
    {
        view->closePersistentEditor(view->selectionModel()->currentIndex());

        // simulate drag/drop to root (hidden) node
        QModelIndex index = view->selectionModel()->currentIndex();
        QAbstractItemModel *model = view->model();

        QModelIndexList list;
        list << index;
        QMimeData *mime = model->mimeData(list);
        model->dropMimeData(mime, Qt::MoveAction, 0, 0, QModelIndex());
    }
}

void TDMGui::slot_unselect()
{
    QTreeView *view = ui->tree_widget;
    view->closePersistentEditor(view->selectionModel()->currentIndex());

    view->selectionModel()->clear();
    view->selectionModel()->clearSelection();

}
