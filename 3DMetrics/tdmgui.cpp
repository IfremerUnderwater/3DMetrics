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

#include "edit_measure_dialog.h"

#include "attribpointwidget.h"
#include "attriblinewidget.h"
#include "attribareawidget.h"

TDMGui::TDMGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TDMGui)
{
    qRegisterMetaType<MeasurePattern>();

    ui->setupUi(this);
    //    TDMLayerRegistry *reg = TDMLayerRegistry::instance();

    //    reg->addMapLayer(new TDMLayer(TDMLayer::ModelLayer, tr("Models")));

    //    ui->tree_widget->setModel(reg);

    ui->tree_widget->setModel(TdmLayersModel::instance());
    ui->tree_widget->hideColumn(1);

    QObject::connect(ui->open_3d_model_action, SIGNAL(triggered()), this, SLOT(slot_open3dModel()));
    QObject::connect(ui->open_measurement_file_action, SIGNAL(triggered()), this, SLOT(slot_openMeasureFile()));
    QObject::connect(ui->quit_action, SIGNAL(triggered()), this, SLOT(close()));

    // check state on the treeview item
    QObject::connect(TdmLayersModel::instance(),SIGNAL(signal_checkChanged(TdmLayerItem*)),this,SLOT(slot_checkChanged(TdmLayerItem*)));

    // treeview selection changed
    QObject::connect(ui->tree_widget->selectionModel(),
                     SIGNAL(selectionChanged(const QItemSelection &,
                                             const QItemSelection &)),
                     this, SLOT(slot_selectionChanged()));

    //item dropped in treeview - manage visibililty
    QObject::connect(TdmLayersModel::instance(),SIGNAL(signal_itemDropped(TdmLayerItem*)),this,SLOT(slot_itemDropped(TdmLayerItem*)));

    // treeview contextual menu
    ui->tree_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tree_widget,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(slot_treeViewContextMenu(const QPoint &)));

    // line numbers
    ui->attrib_table->verticalHeader()->setVisible(true);

    // tablewidget contextual menu
    ui->attrib_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->attrib_table,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(slot_attribTableContextMenu(const QPoint &)));

    // general tools
    connect(ui->focusing_tool_action,SIGNAL(triggered()), this, SLOT(slot_focussingTool()));

    // measurement tools
    ui->line_tool->setEnabled(false);
    ui->surface_tool->setEnabled(false);
    ui->pick_point->setEnabled(false);
    ui->cancel_measurement->setEnabled(false);

    // file menu
    ui->save_measurement_file_action->setEnabled(false);

    updateAttributeTable(0);
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
            view->selectionModel()->currentIndex();
        }
    }

    QVariant data(tr("New Measurement"));
    //std::shared_ptr<ToolHandler> th(new ToolHandler(ui->display_widget));
    MeasurePattern pattern;
    QString dummy("measure");
    TDMMeasureLayerData modelData(dummy, pattern);
    QVariant tool;
    tool.setValue(modelData);
    TdmLayerItem *added = model->addLayerItem(TdmLayerItem::MeasurementLayer, parent, data, tool);
    added->setChecked(true);

    // select created item
    view->selectionModel()->clear();
    view->selectionModel()->clearSelection();
    QModelIndex index = model->index(added);
    view->setExpanded(index.parent(),true);
    view->selectionModel()->select(index,QItemSelectionModel::ClearAndSelect);
    view->edit(index);

    ui->save_measurement_file_action->setEnabled(true);
    updateAttributeTable(0);
    slot_selectionChanged();
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

    QVariant data(tr("New Group"));
    QVariant dummy("group");
    TdmLayerItem *added = model->addLayerItem(TdmLayerItem::GroupLayer, parent, data, dummy);
    added->setChecked(true);

    slot_unselect();

    // select created item
    view->selectionModel()->clear();
    view->selectionModel()->clearSelection();
    QModelIndex index = model->index(added);
    view->setExpanded(index.parent(),true);
    view->selectionModel()->select(index,QItemSelectionModel::ClearAndSelect);
    view->edit(index);

    ui->save_measurement_file_action->setEnabled(false);
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

        updateAttributeTable(selected);

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
            //lda.tool()->getGeode()->setNodeMask(itemChecked && checked ? 0xFFFFFFFF : 0);
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

void TDMGui::slot_treeViewContextMenu(const QPoint &)
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

    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    view->closePersistentEditor(view->selectionModel()->currentIndex());


    if (hasSelection && hasCurrent)
    {
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                menu->addAction(tr("Edit measure"), this, SLOT(slot_editMeasurement()));
                menu->addSeparator();
            }
        }
    }

    menu->addAction(tr("Rename"), this, SLOT(slot_renameTreeItem()));
    menu->addSeparator();
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
            //ui->display_widget->removeGeode(lda.tool()->getGeode());
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
    slot_unselect();
}

void TDMGui::slot_renameTreeItem()
{
    QTreeView *view = ui->tree_widget;

    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    bool hasCurrent = view->selectionModel()->currentIndex().isValid();

    if (hasSelection && hasCurrent)
    {
        view->closePersistentEditor(view->selectionModel()->currentIndex());

        QModelIndex index = view->selectionModel()->currentIndex();
        view->edit(index);
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

    updateAttributeTable(0);
}

void TDMGui::slot_focussingTool()
{
    ui->display_widget->home();
}

void TDMGui::slot_editMeasurement()
{
    QTreeView *view = ui->tree_widget;

    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    bool hasCurrent = view->selectionModel()->currentIndex().isValid();

    if (hasSelection && hasCurrent)
    {
        view->closePersistentEditor(view->selectionModel()->currentIndex());
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                QString name = selected->data(0).toString();
                // Show dialog
                edit_measure_dialog *dlg = new edit_measure_dialog(this);

                QVariant data1 = selected->data(1);
                TDMMeasureLayerData lda = data1.value<TDMMeasureLayerData>();
                qDebug() << "init dialog" << lda.pattern().getNbFields();
                dlg->setPattern(lda.pattern());
                dlg->setWindowTitle(name);
                dlg->setModal(true);

                QObject::connect(dlg,SIGNAL(signal_apply(MeasurePattern)),
                                 this,SLOT(slot_patternChanged(MeasurePattern)));

                dlg->show();
            }
        }
    }
}

void TDMGui::slot_patternChanged(MeasurePattern pattern)
{
    QTreeView *view = ui->tree_widget;

    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    bool hasCurrent = view->selectionModel()->currentIndex().isValid();

    if (hasSelection && hasCurrent)
    {
        view->closePersistentEditor(view->selectionModel()->currentIndex());
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                QVariant data1 = selected->data(1);
                TDMMeasureLayerData lda = data1.value<TDMMeasureLayerData>();
                lda.pattern().clear();
                for(int i=0; i<pattern.getNbFields(); i++)
                    lda.pattern().addField(pattern.fieldName(i), pattern.fieldType(i));
                data1.setValue<TDMMeasureLayerData>(lda);
                qDebug() << "slot_patternChanged" << lda.pattern().getNbFields();
                selected->setData(1,data1);
                updateAttributeTable(selected);
            }
        }
    }
}

void TDMGui::updateAttributeTable(TdmLayerItem *item)
{
    QTableWidget *table = ui->attrib_table;
    if(item != nullptr && item->type() == TdmLayerItem::MeasurementLayer)
    {
        QVariant data1 = item->data(1);
        TDMMeasureLayerData lda = data1.value<TDMMeasureLayerData>();
        int nbfields = lda.pattern().getNbFields();

        table->setColumnCount(nbfields+1);
        QStringList headers;
        headers << ""; //tr("[+]");

        for(int i=0; i<lda.pattern().getNbFields(); i++)
        {
            QString head = lda.pattern().fieldName(i); // + "\n(" + lda.pattern().fieldTypeName(i) + ")";
            headers << head;
        }
        table->setHorizontalHeaderLabels(headers);
        table->verticalHeader()->setVisible(true);
        table->setRowCount(0);
        m_current = lda.pattern();
    }
    else
    {
        table->setColumnCount(1);
        QStringList headers;
        headers << ""; //tr("[+]");
        table->setHorizontalHeaderLabels(headers);

        table->setRowCount(0);
        m_current.clear();
    }
    table->setColumnWidth(0,30);
    QTableWidgetItem* headerItem = table->horizontalHeaderItem(0);
    if (headerItem)
        headerItem->setToolTip("Visibility");
}

void TDMGui::slot_attribTableContextMenu(const QPoint &)
{
    QMenu *menu = new QMenu;
    QTableWidget *table = ui->attrib_table;

    menu->addAction(tr("Add line"), this, SLOT(slot_addAttributeLine()));

    bool hasCurrent = table->selectionModel()->currentIndex().isValid();
    //    if(!hasCurrent)
    //    {
    //        menu->addAction(tr("Create new group"), this, SLOT(slot_newGroup()));
    //        menu->addAction(tr("Create new measurement"), this, SLOT(slot_newMeasurement()));
    //        menu->exec(QCursor::pos());
    //        return;
    //    }

    bool hasSelection = !table->selectionModel()->selection().isEmpty();

    if (hasSelection && hasCurrent)
    {
        menu->addAction(tr("Delete line"), this, SLOT(slot_deleteAttributeLine()));

        //        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
        //                    view->selectionModel()->currentIndex());
        //        if(selected != nullptr)
        //        {
        //            if(selected->type() == TdmLayerItem::MeasurementLayer)
        //            {
        //                menu->addAction(tr("Edit measure"), this, SLOT(slot_editMeasurement()));
        //                menu->addSeparator();
        //            }
        //        }
    }

    //    menu->addAction(tr("Rename"), this, SLOT(slot_renameTreeItem()));
    //    menu->addSeparator();
    //    menu->addAction(tr("Delete item"), this, SLOT(slot_deleteRow()));
    //    menu->addAction(tr("Move item to toplevel"), this, SLOT(slot_moveToToplevel()));
    //    menu->addSeparator();
    //    menu->addAction(tr("Create new group"), this, SLOT(slot_newGroup()));
    //    menu->addSeparator();
    //    menu->addAction(tr("Create new measurement"), this, SLOT(slot_newMeasurement()));
    //    menu->addSeparator();
    //    menu->addAction(tr("Unselect"), this, SLOT(slot_unselect()));

    menu->exec(QCursor::pos());
}

void TDMGui::slot_addAttributeLine()
{
    QTableWidget *table = ui->attrib_table;
    int nbrows = table->rowCount();
    table->setRowCount(nbrows+1);

    QTableWidgetItem *checkbox = new QTableWidgetItem();
    checkbox->setCheckState(Qt::Checked);
    checkbox->setSizeHint(QSize(20,20));
    table->setItem(nbrows, 0, checkbox);

    // process items in line
    for(int i=1; i<table->columnCount(); i++)
    {
        MeasureType::type type = m_current.fieldType(i-1);
        switch(type)
        {
        case MeasureType::Line:
            // line edit widget
        {
            QTableWidgetItem *pwidget = new QTableWidgetItem();
            table->setItem(nbrows, i, pwidget);
            AttribLineWidget *line = new AttribLineWidget();
            line->setNbval("");
            line->setLengthval("");
            table->setCellWidget(nbrows,i, line);
            int height = table->rowHeight(nbrows);
            int minheight = line->height() + 2;
            if(minheight > height)
                table->setRowHeight(nbrows,minheight);
        }
            break;

        case MeasureType::Point:
            // point edit widget
        {
            QTableWidgetItem *pwidget = new QTableWidgetItem();
            table->setItem(nbrows, i, pwidget);
            AttribPointWidget *point = new AttribPointWidget();
            point->setXval("");
            point->setYval("");
            point->setZval("");
            table->setCellWidget(nbrows,i, point);
            int height = table->rowHeight(nbrows);
            int minheight = point->height() + 2;
            if(minheight > height)
                table->setRowHeight(nbrows,minheight);
        }
            break;

        case MeasureType::Perimeter:
            // perimeter edit widget
        {
            QTableWidgetItem *pwidget = new QTableWidgetItem();
            table->setItem(nbrows, i, pwidget);
            AttribAreaWidget *area = new AttribAreaWidget();
            area->setNbval("");
            area->setAreaval("");
            table->setCellWidget(nbrows,i, area);
            int height = table->rowHeight(nbrows);
            int minheight = area->height() + 2;
            if(minheight > height)
                table->setRowHeight(nbrows,minheight);
        }
            break;

        default:
            // string - default editable text line
            //            QTableWidgetItem *str = new QTableWidgetItem();
            //            QColor color(192,192,192);
            //            str->setBackgroundColor(color);
            //            table->setItem(nbrows, i, str);
            break;
        }
    }
}

void TDMGui::slot_deleteAttributeLine()
{
    QTableWidget *table = ui->attrib_table;

    bool hasCurrent = table->selectionModel()->currentIndex().isValid();
    bool hasSelection = !table->selectionModel()->selection().isEmpty();
    if (hasSelection && hasCurrent)
    {
        //*** TODO : remove actual data
        table->removeRow(table->currentRow());
    }
}

