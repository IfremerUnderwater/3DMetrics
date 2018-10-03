#include <QCloseEvent>

#include "tdmgui.h"
#include "ui_tdmgui.h"

//#include "TDMLayers/tdmlayer.h"
//#include "TDMLayers/tdmmaplayerregistry.h"

#include "TreeView/tdmlayeritem.h"
#include "TreeView/tdmlayersmodel.h"

#include "TreeView/tdmmodellayerdata.h"
#include "TreeView/tdmmeasurelayerdata.h"

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
    QObject::connect(ui->quit_action, SIGNAL(triggered()), this, SLOT(close()));

    // check state on e treeview item
    QObject::connect(TdmLayersModel::instance(),SIGNAL(signal_checkChanged(TdmLayerItem*)),this,SLOT(slot_checkChanged(TdmLayerItem*)));

    // treeview selection changed
    QObject::connect(ui->tree_widget->selectionModel(),
                     SIGNAL(selectionChanged(const QItemSelection &,
                                             const QItemSelection &)),
                     this, SLOT(slot_selectionChanged()));

    //item dropped in treeview - manage visibililty
    QObject::connect(TdmLayersModel::instance(),SIGNAL(signal_itemDropped(TdmLayerItem*)),this,SLOT(slot_itemDropped(TdmLayerItem*)));


    // treeview contextuel menu
    ui->tree_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tree_widget,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(slot_contextMenu(const QPoint &)));

    // tools
    ui->line_tool->setEnabled(false);
    ui->surface_tool->setEnabled(false);
    ui->pick_point->setEnabled(false);
    ui->cancel_measurement->setEnabled(false);

    // file menu
    ui->save_measurement_file_action->setEnabled(false);
}

TDMGui::~TDMGui()
{
    delete ui;
}

// ask on close
void TDMGui::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, tr("Close 3DMetrics"),
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes)
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
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

        TdmLayerItem *added = model->addLayerItem(TdmLayerItem::ModelLayer, model->rootItem(), name, data);
        added->setChecked(true);

        ui->display_widget->addNodeToScene(node);

        QApplication::restoreOverrideCursor();
    }
    else
    {
        QMessageBox::information(this, tr("Error : 3d Model"), tr("Error : you didn't open a 3d model"));
    }
}

void TDMGui::slot_newMeasurement()
{
    TdmLayersModel *model = TdmLayersModel::instance();
    QVariant data("New Measurement");
    std::shared_ptr<ToolHandler> th(new ToolHandler(ui->display_widget));
    QString dummy("measure");
    TDMMeasureLayerData modelData(dummy, th);
    QVariant tool;
    tool.setValue(modelData);
    TdmLayerItem *added = model->addLayerItem(TdmLayerItem::MeasurementLayer, model->rootItem(), data, tool);
    added->setChecked(true);
}

void TDMGui::slot_openMeasureFile()
{
    // TODO

//    TdmLayersModel *model = TdmLayersModel::instance();
//    QVariant data("Dummy MeasureFile");
//    std::shared_ptr<ToolHandler> th(new ToolHandler(ui->display_widget));
//    QString dummy("dummy");
//    TDMMeasureLayerData modelData(dummy, th);
//    QVariant tool;
//    tool.setValue(modelData);
//    TdmLayerItem *added = model->addLayerItem(TdmLayerItem::MeasurementLayer, model->rootItem(), data, tool);
//    added->setChecked(true);
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
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr && selected->type() == TdmLayerItem::GroupLayer)
        {
            parent = selected;
        }
    }

    QVariant data("Group");
    QVariant dummy("group");
    TdmLayerItem *added = model->addLayerItem(TdmLayerItem::GroupLayer, parent, data, dummy);
    added->setChecked(true);

    slot_unselect();
}


void TDMGui::slot_selectionChanged()
{
    QTreeView *view = ui->tree_widget;

    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    bool hasCurrent = view->selectionModel()->currentIndex().isValid();

    ui->line_tool->setEnabled(false);
    ui->surface_tool->setEnabled(false);
    ui->pick_point->setEnabled(false);
    ui->cancel_measurement->setEnabled(false);
    //*** TODO disconnect actions

    if (hasSelection && hasCurrent) {
        view->closePersistentEditor(view->selectionModel()->currentIndex());
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            ui->save_measurement_file_action->setEnabled(false);
            QVariant data1 = selected->data(1);
            if(selected->type() == TdmLayerItem::ModelLayer)
            {
                TDMModelLayerData lda = data1.value<TDMModelLayerData>();
                if(lda.fileName().length())
                    data1 = lda.fileName();
            }
            else if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                TDMMeasureLayerData lda = data1.value<TDMMeasureLayerData>();
                if(lda.fileName().length())
                    data1 = lda.fileName();
                ui->line_tool->setEnabled(true);
                ui->surface_tool->setEnabled(true);
                ui->pick_point->setEnabled(true);
                ui->cancel_measurement->setEnabled(true);
                ui->save_measurement_file_action->setEnabled(true);
                //*** TODO connect actions
            }

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
    else
        statusBar()->showMessage("");
}

void TDMGui::manageCheckStateForChildren(TdmLayerItem *item, bool checked)
{
    if(item == nullptr)
        return;

    bool itemChecked = item->isChecked();
    if(item->type() == TdmLayerItem::ModelLayer)
    {
        QVariant data1 = item->data(1);
        if(data1.canConvert<TDMModelLayerData>())
        {
            TDMModelLayerData lda = data1.value<TDMModelLayerData>();
            lda.node()->setNodeMask(itemChecked && checked ? 0xFFFFFFFF : 0);
        }
    }
    if(item->type() == TdmLayerItem::MeasurementLayer)
    {
        QVariant data1 = item->data(1);
        if(data1.canConvert<TDMMeasureLayerData>())
        {
            // TODO : test
            TDMMeasureLayerData lda = data1.value<TDMMeasureLayerData>();
            lda.tool()->getGeode()->setNodeMask(itemChecked && checked ? 0xFFFFFFFF : 0);
        }
    }
    if(item->type() == TdmLayerItem::GroupLayer)
    {
        for(int i=0; i<item->childCount(); i++)
            manageCheckStateForChildren(item->child(i), checked && itemChecked);
    }

}

void TDMGui::slot_itemDropped(TdmLayerItem*item)
{
    if(item == nullptr)
        return;
    if(item->parent() == nullptr)
        return;

    manageCheckStateForChildren(item->parent(), item->parent()->isChecked());
}

void TDMGui::slot_checkChanged(TdmLayerItem *item)
{
    manageCheckStateForChildren(item, item->isChecked());
}

void TDMGui::slot_contextMenu(const QPoint &)
{
    QMenu *menu = new QMenu;
    QTreeView *view = ui->tree_widget;

    bool hasCurrent = view->selectionModel()->currentIndex().isValid();
    if(!hasCurrent)
    {
        menu->addAction(tr("Create new group"), this, SLOT(slot_newGroup()));
        menu->addAction(tr("Create new measurement"), this, SLOT(slot_newMeasurement()));
        menu->exec(QCursor::pos());
        return;
    }

    view->closePersistentEditor(view->selectionModel()->currentIndex());

    menu->addAction(tr("Delete item"), this, SLOT(slot_deleteRow()));
    menu->addAction(tr("Move item to toplevel"), this, SLOT(slot_moveToToplevel()));
    menu->addSeparator();
    menu->addAction(tr("Create new group"), this, SLOT(slot_newGroup()));
    menu->addSeparator();
    menu->addAction(tr("Create new measurement"), this, SLOT(slot_newMeasurement()));
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
        //*** TODO test
        QVariant data1 = item->data(1);
        if(data1.canConvert<TDMMeasureLayerData>())
        {
            // TODO : test
            TDMMeasureLayerData lda = data1.value<TDMMeasureLayerData>();
            ui->display_widget->removeGeode(lda.tool()->getGeode());
        }
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

    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    bool hasCurrent = view->selectionModel()->currentIndex().isValid();

    if (hasSelection && hasCurrent)
    {
        QModelIndex index = view->selectionModel()->currentIndex();
        QAbstractItemModel *model = view->model();
        TdmLayerItem *item = (static_cast<TdmLayersModel*>(model))->getLayerItem(index);

        QString msg = tr("Do you want to delete %1:\n%2").arg(item->typeName()).arg(item->data(0).toString());
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, tr("Delete Row Confirmation"),
                                                                    msg,
                                                                    QMessageBox::Cancel | QMessageBox::Ok,
                                                                    QMessageBox::Cancel);
        if (resBtn != QMessageBox::Ok)
        {
            return;
        }

        view->closePersistentEditor(view->selectionModel()->currentIndex());

        deleteTreeItemsData(item);
        // delete node in view
        model->removeRow(index.row(), index.parent());
    }
}

void TDMGui::slot_moveToToplevel()
{
    QTreeView *view = ui->tree_widget;

    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    bool hasCurrent = view->selectionModel()->currentIndex().isValid();

    if (hasSelection && hasCurrent)
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
    slot_unselect();
}

void TDMGui::slot_unselect()
{
    QTreeView *view = ui->tree_widget;
    view->closePersistentEditor(view->selectionModel()->currentIndex());

    view->selectionModel()->clear();
    view->selectionModel()->clearSelection();

    ui->save_measurement_file_action->setEnabled(false);
}
