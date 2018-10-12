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

#include "measuretablewidgetitem.h"
#include "Measure/measurestring.h"
#include "Measure/measurepoint.h"
#include "Measure/measureline.h"
#include "Measure/measurearea.h"
#include "Measure/osgmeasurerow.h"

#include "OSGWidget/osgwidgettool.h"

TDMGui::TDMGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TDMGui),
    m_currentItem(0)
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
    QObject::connect(ui->save_measurement_file_action, SIGNAL(triggered()), this, SLOT(slot_saveMeasureFile()));
    QObject::connect(ui->quit_action, SIGNAL(triggered()), this, SLOT(close()));

    // check state on the treeview item
    QObject::connect(TdmLayersModel::instance(),SIGNAL(signal_checkChanged(TdmLayerItem*)),this,SLOT(slot_checkChanged(TdmLayerItem*)));

    // treeview selection changed
    QObject::connect(ui->tree_widget->selectionModel(),
                     SIGNAL(selectionChanged(const QItemSelection &,
                                             const QItemSelection &)),
                     this, SLOT(slot_selectionChanged(const QItemSelection &,
                                                      const QItemSelection &)));

    //item dropped in treeview - manage visibililty
    QObject::connect(TdmLayersModel::instance(),SIGNAL(signal_itemDropped(TdmLayerItem*)),this,SLOT(slot_itemDropped(TdmLayerItem*)));

    // treeview contextual menu
    ui->tree_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tree_widget,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(slot_treeViewContextMenu(const QPoint &)));

    // line numbers
    ui->attrib_table->verticalHeader()->setVisible(true);
    QHeaderView *verticalHeader = ui->attrib_table->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::ResizeToContents);
    verticalHeader->setDefaultSectionSize(60);

    // tablewidget contextual menu
    ui->attrib_table->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->attrib_table,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(slot_attribTableContextMenu(const QPoint &)));
    connect(ui->attrib_table,SIGNAL(cellDoubleClicked(int, int)), this, SLOT(slot_attribTableDoubleClick(int, int)));
    connect(ui->attrib_table, SIGNAL(cellChanged(int, int)), this, SLOT(slot_attribTableCellChanged(int, int)));

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

    // tools
    OSGWidgetTool::initialize(ui->display_widget);
    connect(ui->display_widget, SIGNAL(signal_startTool(QString&)), this, SLOT(slot_messageStartTool(QString&)));
    connect(ui->display_widget, SIGNAL(signal_cancelTool(QString&)), this, SLOT(slot_messageCancelTool(QString&)));
    connect(ui->display_widget, SIGNAL(signal_endTool(QString&)), this, SLOT(slot_messageEndTool(QString&)));

    // docking ????? pb with OSG widget
    connect(ui->display_widget_dock, SIGNAL(topLevelChanged(bool)), this, SLOT(slot_displayToplevelChanged(bool)));
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

        QModelIndex index = model->index(added);
        selectItem(index);

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
    osg::ref_ptr<osg::Group> group = new osg::Group();
    ui->display_widget->addGroup(group);
    TDMMeasureLayerData modelData(dummy, pattern, group);
    QVariant tool;
    tool.setValue(modelData);
    TdmLayerItem *added = model->addLayerItem(TdmLayerItem::MeasurementLayer, parent, data, tool);
    added->setChecked(true);

    QModelIndex index = model->index(added);
    // select created item
    selectItem(index);
    view->setExpanded(index.parent(),true);
    view->edit(index);

    ui->save_measurement_file_action->setEnabled(true);
    updateAttributeTable(0);
    QItemSelection is;
    slot_selectionChanged(is, is);
}

void TDMGui::slot_openMeasureFile()
{
    QString fileName = getOpenFileName(this,tr("Select measure file to open"), "", tr("Json files (*.json)"));
    if(fileName.length() > 0)
    {
        QFile f(fileName);
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray ba = f.readAll();
        f.close();

        // create line
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

        // load pattern
        MeasurePattern pattern;
        bool res = pattern.loadFromJson(ba);
        if(!res)
        {
            QMessageBox::critical(this, tr("Error : measure file"), tr("Error : invalid file"));
            return;
        }

        QFileInfo fi(f.fileName());
        QVariant data(fi.fileName());

        osg::ref_ptr<osg::Group> group = new osg::Group();
        ui->display_widget->addGroup(group);
        TDMMeasureLayerData modelData(f.fileName(), pattern, group);
        modelData.setFileName(fi.filePath());
        QVariant tool;
        tool.setValue(modelData);
        TdmLayerItem *added = model->addLayerItem(TdmLayerItem::MeasurementLayer, parent, data, tool);
        added->setChecked(true);

        QVariant data1;
        data1.setValue<TDMMeasureLayerData>(modelData);
        added->setData(1,data1);
        updateAttributeTable(added);

        TDMMeasureLayerData *localData = new TDMMeasureLayerData(modelData);
        m_currentItem = localData;
        // load data
        QJsonDocument doc = pattern.get();
        m_current = pattern;
        loadData(doc, true);
        modelData.rows() = localData->rows();

        data1.setValue<TDMMeasureLayerData>(modelData);
        added->setData(1,data1);

        ui->save_measurement_file_action->setEnabled(true);

        QModelIndex index = model->index(added);
        view->setExpanded(index.parent(),true);

        // select created item
        selectItem(index);
    }
    else
    {
        QMessageBox::critical(this, tr("Error : measure file"), tr("Error : you didn't open measure file"));
    }
}

void TDMGui::loadData(QJsonDocument &_doc, bool _buildOsg)
{
    QTableWidget *table = ui->attrib_table;

    QJsonArray array = _doc.object().value("Data").toArray();
    for(int rowindex=0; rowindex<array.count(); rowindex++)
    {
        //row
        table->setRowCount(rowindex+1);
        QJsonArray row = array.at(rowindex).toArray();

        osgMeasureRow *osgRow = 0;
        if(_buildOsg)
        {
            osgRow = new osgMeasureRow(m_current);
            m_currentItem->addRow(osgRow, rowindex);
        }
        else
            osgRow = m_currentItem->rows().at(rowindex);

        // checkbox
        QTableWidgetItem *checkbox = new QTableWidgetItem();
        checkbox->setCheckState(Qt::Checked);
        checkbox->setSizeHint(QSize(20,20));
        table->setItem(rowindex, 0, checkbox);

        // columns
        for(int c=0; c<m_current.getNbFields(); c++)
        {
            int i = c+1;
            QJsonObject obj = row.at(c).toObject();

            MeasureType::type type = m_current.fieldType(c);
            switch(type)
            {
            case MeasureType::Line:
                // line edit widget
            {
                MeasureTableWidgetItem *pwidget = new MeasureTableWidgetItem();

                MeasureLine *l = new MeasureLine(m_current.fieldName(c),osgRow->get(c));
                l->decode(obj);
                pwidget->setMeasureItem(l);
                table->setItem(rowindex, i, pwidget);
                AttribLineWidget *line = new AttribLineWidget();
                line->setLine(l);
                table->setCellWidget(rowindex,i, line);
                int height = table->rowHeight(rowindex);
                int minheight = line->height() + 2;
                if(minheight > height)
                    table->setRowHeight(rowindex,minheight);
            }
                break;

            case MeasureType::Point:
                // point edit widget
            {
                MeasureTableWidgetItem *pwidget = new MeasureTableWidgetItem();
                MeasurePoint *p = new MeasurePoint(m_current.fieldName(c),osgRow->get(c));
                p->decode(obj);
                if(_buildOsg)
                {
                    p->updateGeode();
                }
                pwidget->setMeasureItem(p);
                table->setItem(rowindex, i, pwidget);
                AttribPointWidget *point = new AttribPointWidget();
                point->setPoint(p);
                table->setCellWidget(rowindex,i, point);
                int height = table->rowHeight(rowindex);
                int minheight = point->height() + 2;
                if(minheight > height)
                    table->setRowHeight(rowindex,minheight);
            }
                break;

            case MeasureType::Perimeter:
                // perimeter edit widget
            {
                MeasureTableWidgetItem *pwidget = new MeasureTableWidgetItem();
                MeasureArea *a = new MeasureArea(m_current.fieldName(c),osgRow->get(c));
                a->decode(obj);
                pwidget->setMeasureItem(a);
                table->setItem(rowindex, i, pwidget);
                AttribAreaWidget *area = new AttribAreaWidget();
                area->setArea(a);
                table->setCellWidget(rowindex,i, area);
                int height = table->rowHeight(rowindex);
                int minheight = area->height() + 2;
                if(minheight > height)
                    table->setRowHeight(rowindex,minheight);
            }
                break;

            default:
                // string - default editable text line
            {
                MeasureTableWidgetItem *pwidget = new MeasureTableWidgetItem();
                MeasureString *s = new MeasureString(m_current.fieldName(c));
                s->decode(obj);
                pwidget->setMeasureItem(s);
                pwidget->setText(s->value());
                table->setItem(rowindex, i, pwidget);
            }
                break;
            }
        }
    }
}

void TDMGui::slot_saveMeasureFile()
{
    // check measure selected
    QTreeView *view = ui->tree_widget;

    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    bool hasCurrent = view->selectionModel()->currentIndex().isValid();
    bool ok = false;

    TDMMeasureLayerData lda;

    if (hasSelection && hasCurrent) {
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            QVariant data1 = selected->data(1);
            if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                lda = data1.value<TDMMeasureLayerData>();
                ok = true;
            }
        }
    }

    if(!ok)
        return;

    // save in file
    QString name = getSaveFileName(this, tr("Save measurement"), "",
                                   "*.json");
    QFileInfo fileinfo(name);

    // check filename is not empty
    if(fileinfo.fileName().isEmpty()){
        QMessageBox::critical(this, tr("Error : save measurement"), tr("Error : you didn't give a name to the file"));
        return;
    }

    // add suffix if needed
    if (fileinfo.suffix() != "json"){
        name += ".json";
        fileinfo.setFile(name);
    }

    QFile file(name);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::critical(this, tr("Error : save measurement file"), tr("Error : cannot open file for saving, check path writing rights"));
        return;
    }

    // build json object
    QJsonDocument json = lda.pattern().get();
    // add data
    saveData(json);

    // write
    QString json_string = json.toJson();
    file.write(json_string.toUtf8());
    file.close();

    // store file name
    lda.setFileName(fileinfo.filePath());
    TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                view->selectionModel()->currentIndex());
    QVariant fname = fileinfo.fileName();
    selected->setData(0,fname);
    QVariant data1;
    data1.setValue(lda);
    selected->setData(1, data1);
}

void TDMGui::saveData(QJsonDocument &_doc)
{
    QTableWidget *table = ui->attrib_table;
    QJsonArray array;

    for(int i=0; i<table->rowCount(); i++)
    {
        // add row
        QJsonArray row;
        for(int c=1; c<table->columnCount(); c++)
        {
            // add field
            MeasureTableWidgetItem *pwidget = (MeasureTableWidgetItem *)table->item(i,c);
            MeasureItem *it = pwidget->measureItem();
            QJsonObject o;
            it->encode(o);
            row.append(o);
        }
        array.append(row);
    }

    QJsonObject rootobj = _doc.object();
    rootobj.insert("Data",array);
    _doc.setObject(rootobj);
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

    QModelIndex index = model->index(added);
    view->setExpanded(index.parent(),true);
    view->edit(index);

    // select created item
    selectItem(index);

    ui->save_measurement_file_action->setEnabled(false);
}

void TDMGui::slot_selectionChanged(const QItemSelection& /*_sel*/, const QItemSelection& _desel)
{
    if(_desel.length() > 0)
    {
        if(!_desel.first().isEmpty() && _desel.first().isValid())
        {
            QModelIndex prev = _desel.first().indexes().first();
            TdmLayerItem *prevselected = TdmLayersModel::instance()->getLayerItem(prev);
            if(prevselected != nullptr && prevselected->type() == TdmLayerItem::MeasurementLayer)
            {
                // save data
                QVariant data1 = prevselected->data(1);
                TDMMeasureLayerData lda = data1.value<TDMMeasureLayerData>();

                MeasurePattern &pattern = lda.pattern();

                QJsonDocument doc = pattern.get();
                saveData(doc);

                pattern.set(doc);
                data1.setValue<TDMMeasureLayerData>(lda);
                prevselected->setData(1, data1);
                delete m_currentItem;
                m_currentItem = 0;
                updateAttributeTable(0);
            }
        }
    }
    QTreeView *view = ui->tree_widget;

    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    bool hasCurrent = view->selectionModel()->currentIndex().isValid();

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
                updateAttributeTable(selected);
            }
            else if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                QTableWidget *table = ui->attrib_table;
                table->setRowCount(0);

                TDMMeasureLayerData lda = data1.value<TDMMeasureLayerData>();
                if(lda.fileName().length())
                    data1 = lda.fileName();

                updateAttributeTable(selected);

                m_current = lda.pattern();
                m_currentItem = new TDMMeasureLayerData(lda);

                QJsonDocument doc = lda.pattern().get();
                //qDebug() << data1.toString() << " " <<  doc.object().value("Data").toArray().count() << " " << lda.rows().size();
                loadData(doc, false);

                ui->save_measurement_file_action->setEnabled(true);
            }

            statusBar()->showMessage(tr("%1 - %2").arg(selected->data(0).toString()).arg(data1.toString()));
        }
        else
            updateAttributeTable(0);
    }
    else
    {
        updateAttributeTable(0);
        statusBar()->showMessage("");
    }
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
            lda.group()->setNodeMask(itemChecked && checked ? 0xFFFFFFFF : 0);
            //***TODO + propagate to children ????
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
            ui->display_widget->removeGroup(lda.group());
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
    //*** TODO refactor data if nb rows > 0
    //** + confirmation
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
                //qDebug() << "slot_patternChanged" << lda.pattern().getNbFields();
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
        for(int i=0; i<lda.pattern().getNbFields(); i++)
        {
            QString tt = "(" + lda.pattern().fieldTypeName(i) + ")";
            QTableWidgetItem* headerItem = table->horizontalHeaderItem(i+1);
            if (headerItem)
                headerItem->setToolTip(tt);
        }
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
    bool hasSelection = !table->selectionModel()->selection().isEmpty();

    if (hasSelection && hasCurrent)
    {
        menu->addAction(tr("Delete line"), this, SLOT(slot_deleteAttributeLine()));
    }

    menu->exec(QCursor::pos());
}

void TDMGui::slot_addAttributeLine()
{
    QTableWidget *table = ui->attrib_table;
    QTreeView *view = ui->tree_widget;

    // insert last position
    int rowindex = table->rowCount();
    table->setRowCount(rowindex+1);
    // insert first position
    //int rowindex = 0;
    //table->insertRow(rowindex);

    osgMeasureRow *osgRow = new osgMeasureRow(m_current);
    m_currentItem->addRow(osgRow, rowindex);

    QTableWidgetItem *checkbox = new QTableWidgetItem();
    checkbox->setCheckState(Qt::Checked);
    checkbox->setSizeHint(QSize(20,20));
    table->setItem(rowindex, 0, checkbox);

    // process items in line
    for(int i=1; i<table->columnCount(); i++)
    {
        MeasureType::type type = m_current.fieldType(i-1);
        switch(type)
        {
        case MeasureType::Line:
            // line edit widget
        {
            MeasureTableWidgetItem *pwidget = new MeasureTableWidgetItem();
            MeasureLine *l = new MeasureLine(m_current.fieldName(i-1),osgRow->get(i-1));
            pwidget->setMeasureItem(l);
            table->setItem(rowindex, i, pwidget);
            AttribLineWidget *line = new AttribLineWidget();
            line->setLine(l);
            table->setCellWidget(rowindex,i, line);
            int height = table->rowHeight(rowindex);
            int minheight = line->height() + 2;
            if(minheight > height)
                table->setRowHeight(rowindex,minheight);
        }
            break;

        case MeasureType::Point:
            // point edit widget
        {
            MeasureTableWidgetItem *pwidget = new MeasureTableWidgetItem();
            MeasurePoint *p = new MeasurePoint(m_current.fieldName(i-1),osgRow->get(i-1));
            //p->updateGeode();
            pwidget->setMeasureItem(p);
            table->setItem(rowindex, i, pwidget);
            AttribPointWidget *point = new AttribPointWidget();
            point->setPoint(p, false);
            table->setCellWidget(rowindex,i, point);
            int height = table->rowHeight(rowindex);
            int minheight = point->height() + 2;
            if(minheight > height)
                table->setRowHeight(rowindex,minheight);
        }
            break;

        case MeasureType::Perimeter:
            // perimeter edit widget
        {
            MeasureTableWidgetItem *pwidget = new MeasureTableWidgetItem();
            MeasureArea *a = new MeasureArea(m_current.fieldName(i-1),osgRow->get(i-1));
            pwidget->setMeasureItem(a);
            table->setItem(rowindex, i, pwidget);
            AttribAreaWidget *area = new AttribAreaWidget();
            area->setArea(a);
            //            area->setNbval("");
            //            area->setAreaval("");
            table->setCellWidget(rowindex,i, area);
            int height = table->rowHeight(rowindex);
            int minheight = area->height() + 2;
            if(minheight > height)
                table->setRowHeight(rowindex,minheight);
        }
            break;

        default:
            // string - default editable text line
        {
            MeasureTableWidgetItem *pwidget = new MeasureTableWidgetItem();
            pwidget->setMeasureItem(new MeasureString(m_current.fieldName(i-1)));
            table->setItem(rowindex, i, pwidget);
        }
            break;
        }
    }

    // save in actual selection
    TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                view->selectionModel()->currentIndex());
    if(selected != nullptr)
    {
        QVariant data1 = selected->data(1);
        TDMMeasureLayerData lda;
        if(selected->type() == TdmLayerItem::MeasurementLayer)
        {
            lda = data1.value<TDMMeasureLayerData>();
            lda.rows() = m_currentItem->rows();
            data1.setValue(lda);
            selected->setData(1, data1);
        }
    }

    table->selectRow(rowindex);
}

void TDMGui::slot_deleteAttributeLine()
{
    QTableWidget *table = ui->attrib_table;
    QTreeView *view = ui->tree_widget;

    bool hasCurrent = table->selectionModel()->currentIndex().isValid();
    bool hasSelection = !table->selectionModel()->selection().isEmpty();
    if (hasSelection && hasCurrent)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, tr("Delete Row Confirmation"),
                                                                    tr("Do you want to delete the selected row?"),
                                                                    QMessageBox::Cancel | QMessageBox::Ok,
                                                                    QMessageBox::Cancel);
        if (resBtn != QMessageBox::Ok)
        {
            return;
        }

        // remove in osg
        int row = table->currentRow();
        m_currentItem->deleteRow(row);
        // save in actual selection
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            QVariant data1 = selected->data(1);
            TDMMeasureLayerData lda;
            if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                lda = data1.value<TDMMeasureLayerData>();
                lda.rows() = m_currentItem->rows();
                data1.setValue(lda);
                selected->setData(1, data1);
            }
        }
        // removal of data : done by destructor
        table->removeRow(row);
    }
}

void TDMGui::slot_attribTableDoubleClick(int row, int column)
{
    QTableWidget *table = ui->attrib_table;
    MeasureType::type type = m_current.fieldType(column-1);

    switch(type)
    {
    case MeasureType::Line:
        // line edit widget
    {
        AttribLineWidget *line = (AttribLineWidget *)table->cellWidget(row, column);
        line->clicked();
    }
        break;

    case MeasureType::Point:
        // point edit widget
    {
        AttribPointWidget *point = (AttribPointWidget *)table->cellWidget(row, column);
        point->clicked();
    }
        break;

    case MeasureType::Perimeter:
        // perimeter edit widget
    {
        AttribAreaWidget *area = (AttribAreaWidget *)table->cellWidget(row, column);
        area->clicked();
    }
        break;

    default:
        // string - default editable text line
        //table->editItem(table->item(row, column));
        // need slot_attribTableCellChanged to update data
        break;
    }
}

void TDMGui::slot_attribTableCellChanged(int row, int column)
{
    if(column == 0 && m_currentItem->rows().size() > 0)
    {
        // check state
        QTableWidget *table = ui->attrib_table;
        QTableWidgetItem *it = table->item(row, column);
        osg::ref_ptr<osg::Group> pcur = (m_currentItem->rows()[row])->getGroup();
        if(it->checkState() == Qt::Checked)
        {
            // show line items
            pcur->setNodeMask(0xFFFFFFFF);
        }
        else
        {
            // hide line items
            pcur->setNodeMask(0);
        }

        return;
    }

    MeasureType::type type = m_current.fieldType(column-1);

    switch(type)
    {
    case MeasureType::String:
    {
        // update MeasureString data
        QTableWidget *table = ui->attrib_table;
        MeasureTableWidgetItem *it = (MeasureTableWidgetItem *)table->item(row, column);
        ((MeasureString*)it->measureItem())->setValue(table->item(row,column)->text());
        break;
    }
    default:
        break;
    }
}


void TDMGui::slot_displayToplevelChanged(bool floating)
{
    if(floating)
    {
        // *** TODO : afficher quelque chose
        if(ui->display_widget_dock->isWindow())
        {
            //              ui->display_widget->setParent(ui->display_widget_dock->topLevelWidget());
            ui->display_widget->updateGeometry();
        }
    }
    else
        ui->display_widget->setParent(ui->display_widget_dock);
}

void TDMGui::selectItem(QModelIndex &index)
{
    QTreeView *view = ui->tree_widget;

    view->selectionModel()->clear();
    view->selectionModel()->clearSelection();

    view->selectionModel()->setCurrentIndex(index,QItemSelectionModel::SelectCurrent);
    view->selectionModel()->select(index,QItemSelectionModel::SelectCurrent);
}

void TDMGui::slot_messageStartTool(QString&_msg)
{
    statusBar()->showMessage(_msg);
}

void TDMGui::slot_messageCancelTool(QString&_msg)
{
    statusBar()->showMessage(_msg);
}

void TDMGui::slot_messageEndTool(QString&_msg)
{
    statusBar()->showMessage(_msg);
}
