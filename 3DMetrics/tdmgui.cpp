#include <QCloseEvent>

#include "tdmgui.h"
#include "ui_tdmgui.h"

#include "TreeView/tdm_layer_item.h"
#include "TreeView/tdm_layer_model.h"

#include "TreeView/tdm_model_layerdata.h"
#include "TreeView/tdm_measurement_layerdata.h"

#include "filedialog.h"

#include "edit_measure_dialog.h"

#include "attribpointwidget.h"
#include "attriblinewidget.h"
#include "attribareawidget.h"

#include "measuretablewidgetitem.h"
#include "Measurement/measurement_string.h"
#include "Measurement/measurement_point.h"
#include "Measurement/measurement_line.h"
#include "Measurement/measurement_area.h"
#include "Measurement/osg_measurement_row.h"

#include "OSGWidget/osg_widget_tool.h"

#include "toolpointdialog.h"
#include "toollinedialog.h"
#include "toolareadialog.h"

TDMGui::TDMGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TDMGui),
    m_currentItem(0)
{
    qRegisterMetaType<MeasurePattern>();

    ui->setupUi(this);

    ui->tree_widget->setModel(TdmLayersModel::instance());
    ui->tree_widget->hideColumn(1);

    QObject::connect(ui->open_3d_model_action, SIGNAL(triggered()), this, SLOT(slot_open3dModel()));
    QObject::connect(ui->open_measurement_file_action, SIGNAL(triggered()), this, SLOT(slot_openMeasureFile()));
    QObject::connect(ui->save_measurement_file_action, SIGNAL(triggered()), this, SLOT(slot_saveMeasureFile()));
    QObject::connect(ui->save_measurement_file_as_action, SIGNAL(triggered()), this, SLOT(slot_saveMeasureFileAs()));
    QObject::connect(ui->import_old_measure_format_action, SIGNAL(triggered()), this, SLOT(slot_importOldMeasureFile()));
    QObject::connect(ui->open_project_action, SIGNAL(triggered()), this, SLOT(slot_openProject()));
    QObject::connect(ui->save_project_action, SIGNAL(triggered()), this, SLOT(slot_saveProject()));

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
    verticalHeader->setDefaultSectionSize(100);

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
    ui->open_measurement_file_action->setEnabled(false);
    ui->save_measurement_file_action->setEnabled(false);
    ui->save_measurement_file_as_action->setEnabled(false);
    ui->import_old_measure_format_action->setEnabled(false);

    updateAttributeTable(0);

    // tools
    OSGWidgetTool::initialize(ui->display_widget);
    connect(ui->display_widget, SIGNAL(signal_startTool(QString&)), this, SLOT(slot_messageStartTool(QString&)));
    connect(ui->display_widget, SIGNAL(signal_cancelTool(QString&)), this, SLOT(slot_messageCancelTool(QString&)));
    connect(ui->display_widget, SIGNAL(signal_endTool(QString&)), this, SLOT(slot_messageEndTool(QString&)));
    connect(ui->cancel_measurement, SIGNAL(triggered()), OSGWidgetTool::instance(), SLOT(slot_cancelTool()));
    // temporary tools
    connect(ui->line_tool, SIGNAL(triggered()), this, SLOT(slot_tempLineTool()));
    connect(ui->surface_tool, SIGNAL(triggered()), this, SLOT(slot_tempAreaTool()));
    connect(ui->pick_point, SIGNAL(triggered()), this,  SLOT(slot_tempPointTool()));

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
        // to avoid SEGV on exit
        OSGWidgetTool::instance()->endTool();

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

        load3DModel(fileName, TdmLayersModel::instance()->rootItem(), true);

        // allow measurement to be loaded
        ui->open_measurement_file_action->setEnabled(true);
        ui->import_old_measure_format_action->setEnabled(true);

        // measurement tools
        ui->line_tool->setEnabled(true);
        ui->surface_tool->setEnabled(true);
        ui->pick_point->setEnabled(true);

        QApplication::restoreOverrideCursor();
    }
    else
    {
        QMessageBox::information(this, tr("Error : 3d Model"), tr("Error : you didn't open a 3d model"));
    }
}

void TDMGui::load3DModel(QString _filename, TdmLayerItem *_parent, bool _selectItem)
{
    osg::ref_ptr<osg::Node> node = ui->display_widget->createNodeFromFile(_filename.toStdString());

    TDMModelLayerData modelData(_filename, node);

    TdmLayersModel *model = TdmLayersModel::instance();
    QFileInfo info(_filename);
    QVariant name(info.fileName());
    QVariant data;
    data.setValue(modelData);

    TdmLayerItem *added = model->addLayerItem(TdmLayerItem::ModelLayer, _parent, name, data);
    added->setChecked(true);

    ui->display_widget->addNodeToScene(node);

    if(_selectItem)
    {
        QModelIndex index = model->index(added);
        selectItem(index);
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
    MeasurePattern pattern;
    QString dummy(""); // must be empty
    osg::ref_ptr<osg::Group> group = new osg::Group();
    ui->display_widget->addGroup(group);
    TDMMeasurementLayerData modelData(dummy, pattern, group);
    QVariant tool;
    tool.setValue(modelData);
    TdmLayerItem *added = model->addLayerItem(TdmLayerItem::MeasurementLayer, parent, data, tool);
    added->setChecked(true);

    QModelIndex index = model->index(added);
    // select created item
    selectItem(index);
    view->setExpanded(index.parent(),true);
    view->edit(index);

    ui->save_measurement_file_action->setEnabled(false);
    ui->save_measurement_file_as_action->setEnabled(true);

    updateAttributeTable(0);
    QItemSelection is;
    slot_selectionChanged(is, is);
}

void TDMGui::slot_openMeasureFile()
{
    QString fileName = getOpenFileName(this,tr("Select measure file to open"), "", tr("Json files (*.json)"));
    if(fileName.length() > 0)
    {
        // parent to be used
        TdmLayerItem *parent = TdmLayersModel::instance()->rootItem();

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

        bool res = loadMeasure(fileName, parent, true);
        if(!res)
        {
            QMessageBox::critical(this, tr("Error : measure file"), tr("Error : invalid file"));
            return;
        }

        ui->save_measurement_file_action->setEnabled(true);
        ui->save_measurement_file_as_action->setEnabled(true);
    }
    else
    {
        QMessageBox::critical(this, tr("Error : measure file"), tr("Error : you didn't open measure file"));
    }
}

bool TDMGui::loadMeasure(QString _filename, TdmLayerItem *_parent, bool _selectItem)
{
    TdmLayersModel *model = TdmLayersModel::instance();
    QTreeView *view = ui->tree_widget;

    QFile f(_filename);
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray ba = f.readAll();
    f.close();

    // load pattern
    MeasurePattern pattern;
    bool res = pattern.loadFromJson(ba);
    if(!res)
        return false;

    QFileInfo fi(f.fileName());
    QVariant data(fi.fileName());

    osg::ref_ptr<osg::Group> group = new osg::Group();
    ui->display_widget->addGroup(group);
    TDMMeasurementLayerData modelData(f.fileName(), pattern, group);
    modelData.setFileName(fi.filePath());
    QVariant tool;
    tool.setValue(modelData);
    TdmLayerItem *added = model->addLayerItem(TdmLayerItem::MeasurementLayer, _parent, data, tool);
    added->setChecked(true);

    added->setPrivateData(modelData);
    updateAttributeTable(added);

    TDMMeasurementLayerData *localData = new TDMMeasurementLayerData(modelData);
    m_currentItem = localData;
    // load data
    QJsonDocument doc = pattern.get();
    m_current = pattern;
    loadData(doc, true);
    modelData.rows() = localData->rows();

    added->setPrivateData(modelData);

    QModelIndex index = model->index(added);
    view->setExpanded(index.parent(),true);

    // select created item
    if(_selectItem)
    {
        selectItem(index);
    }
    return true;
}

void TDMGui::loadData(QJsonDocument &_doc, bool _buildOsg)
{  
    OSGWidgetTool::instance()->endTool();

    QTableWidget *table = ui->attrib_table;

    QJsonArray array = _doc.object().value("Data").toArray();
    qDebug() << "loadData buildosg=" << _buildOsg << "  nbItems=" << array.count()
             << " osgRowscount=" << m_currentItem->rows().size();
    for(int rowindex=0; rowindex<array.count(); rowindex++)
    {
        //row
        table->setRowCount(rowindex+1);
        QJsonArray row = array.at(rowindex).toArray();

        osgMeasurementRow *osgRow = 0;
        if(_buildOsg)
        {
            osgRow = new osgMeasurementRow(m_current);
            m_currentItem->addRow(osgRow, rowindex);
        }
        else
            osgRow = m_currentItem->rows().at(rowindex);

        // checkbox
        QTableWidgetItem *checkbox = new QTableWidgetItem();
        checkbox->setCheckState( osgRow->isVisible() ? Qt::Checked : Qt::Unchecked);
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
                if(_buildOsg)
                {
                    l->updateGeode();
                }
                l->computeLength();
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

            case MeasureType::Area:
                // perimeter edit widget
            {
                MeasureTableWidgetItem *pwidget = new MeasureTableWidgetItem();
                MeasureArea *a = new MeasureArea(m_current.fieldName(c),osgRow->get(c));
                a->decode(obj);
                if(_buildOsg)
                {
                    a->updateGeode();
                }
                a->computeLengthAndArea();
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
    OSGWidgetTool::instance()->endTool();

    // check measure selected
    QTreeView *view = ui->tree_widget;

    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    bool hasCurrent = view->selectionModel()->currentIndex().isValid();
    bool ok = false;

    TDMMeasurementLayerData lda;

    if (hasSelection && hasCurrent) {
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                lda = selected->getPrivateData<TDMMeasurementLayerData>();
                ok = true;
            }
        }
    }

    if(!ok)
        return;

    // get filename
    QString name = lda.fileName();
    // check filename is not empty
    if(name.isEmpty()){
        QMessageBox::critical(this, tr("Error : save measurement"), tr("Error : you didn't give a name to the file"));
        return;
    }

    if(!saveMeasure(name, lda))
    {
        QMessageBox::critical(this, tr("Error : save measurement file"), tr("Error : cannot open file for saving, check path writing rights"));
        return;
    }
}

void TDMGui::slot_saveMeasureFileAs()
{
    OSGWidgetTool::instance()->endTool();

    // check measure selected
    QTreeView *view = ui->tree_widget;

    bool hasSelection = !view->selectionModel()->selection().isEmpty();
    bool hasCurrent = view->selectionModel()->currentIndex().isValid();
    bool ok = false;

    TDMMeasurementLayerData lda;

    if (hasSelection && hasCurrent) {
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                lda = selected->getPrivateData<TDMMeasurementLayerData>();
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

    if(!saveMeasure(name, lda))
    {
        QMessageBox::critical(this, tr("Error : save measurement file"), tr("Error : cannot open file for saving, check path writing rights"));
        return;
    }

    // store file name
    lda.setFileName(fileinfo.filePath());
    TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                view->selectionModel()->currentIndex());

    selected->setPrivateData<TDMMeasurementLayerData>(lda);
}

bool TDMGui::saveMeasure(QString _filename, TDMMeasurementLayerData &_data)
{
    QFile file(_filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    // build json object
    QJsonDocument json = _data.pattern().get();
    // add data
    saveData(json);
    // add reference point from OSG widget
    QPointF latlon;
    double refdepth;
    ui->display_widget->getGeoOrigin(latlon,refdepth);
    QJsonObject rootobj = json.object();
    QJsonObject reference;
    // Warning : latitude is in x, longitude is in y in OSGWidget
    reference.insert("latitude", QJsonValue(latlon.x()));
    reference.insert("longitude", QJsonValue(latlon.y()));
    reference.insert("depth", QJsonValue(refdepth));
    rootobj.insert("Reference",reference);
    json.setObject(rootobj);

    // write
    QString json_string = json.toJson();
    file.write(json_string.toUtf8());
    file.close();

    return true;
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
    ui->save_measurement_file_as_action->setEnabled(false);
}

void TDMGui::slot_selectionChanged(const QItemSelection& /*_sel*/, const QItemSelection& _desel)
{   
    OSGWidgetTool::instance()->endTool();

    if(_desel.length() > 0)
    {
        if(!_desel.first().isEmpty() && _desel.first().isValid())
        {
            QModelIndex prev = _desel.first().indexes().first();
            TdmLayerItem *prevselected = TdmLayersModel::instance()->getLayerItem(prev);
            if(prevselected != nullptr && prevselected->type() == TdmLayerItem::MeasurementLayer)
            {
                // save data
                //                QVariant data1 = prevselected->data(1);
                //                TDMMeasurementLayerData lda = data1.value<TDMMeasurementLayerData>();
                TDMMeasurementLayerData lda = prevselected->getPrivateData<TDMMeasurementLayerData>();
                //MeasurePattern &pattern = lda.pattern();

                QJsonDocument doc = lda.pattern().get();
                saveData(doc);

                lda.pattern().set(doc);
                prevselected->setPrivateData<TDMMeasurementLayerData>(lda);
                //                data1.setValue<TDMMeasurementLayerData>(lda);
                //                prevselected->setData(1, data1);
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
            ui->save_measurement_file_as_action->setEnabled(false);
            //QVariant data1 = selected->data(1);
            QString fileName = selected->getFileName();

            if(selected->type() == TdmLayerItem::ModelLayer)
            {
                //                TDMModelLayerData lda = data1.value<TDMModelLayerData>();
                //                if(lda.fileName().length())
                //                    data1 = lda.fileName();
                updateAttributeTable(selected);
            }
            else if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                QTableWidget *table = ui->attrib_table;
                table->setRowCount(0);

                //                QVariant data1 = selected->data(1);
                //                TDMMeasurementLayerData lda = data1.value<TDMMeasurementLayerData>();
                TDMMeasurementLayerData lda = selected->getPrivateData<TDMMeasurementLayerData>();
                //                if(lda.fileName().length())
                //                    data1 = lda.fileName();

                updateAttributeTable(selected);


                m_currentItem = new TDMMeasurementLayerData(lda);
                m_current = m_currentItem->pattern();

                QJsonDocument doc = lda.pattern().get();
                qDebug() << lda.fileName() << " " <<  doc.object().value("Data").toArray().count() << " " << lda.rows().size();
                loadData(doc, false);

                ui->save_measurement_file_as_action->setEnabled(true);
                ui->save_measurement_file_action->setEnabled(!fileName.isEmpty());
            }

            statusBar()->showMessage(tr("%1 - %2").arg(selected->getName()).arg(fileName));
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
        //QVariant data1 = item->data(1);
        if(item->hasData<TDMModelLayerData>())
        {
            TDMModelLayerData lda = item->getPrivateData<TDMModelLayerData>();
            lda.node()->setNodeMask(itemChecked && checked ? 0xFFFFFFFF : 0);
        }
    }

    if(item->type() == TdmLayerItem::MeasurementLayer)
    {
        //QVariant data1 = item->data(1);
        if(item->hasData<TDMMeasurementLayerData>())
        {
            TDMMeasurementLayerData lda = item->getPrivateData<TDMMeasurementLayerData>();
            lda.group()->setNodeMask(itemChecked && checked ? 0xFFFFFFFF : 0);
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
        QAction *newMeasure =  menu->addAction(tr("Create new measurement"), this, SLOT(slot_newMeasurement()));

        if(ui->open_measurement_file_action->isEnabled())
            newMeasure->setEnabled(true);
        else
            newMeasure->setDisabled(true);

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
        //QVariant data1 = item->data(1);
        TDMModelLayerData lda = item->getPrivateData<TDMModelLayerData>();
        ui->display_widget->removeNodeFromScene(lda.node());
    }
    if(item->type() == TdmLayerItem::MeasurementLayer)
    {
        //QVariant data1 = item->data(1);
        if(item->hasData<TDMMeasurementLayerData>())
        {
            // TODO : test
            TDMMeasurementLayerData lda = item->getPrivateData<TDMMeasurementLayerData>();
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

        QString msg = tr("Do you want to delete %1:\n%2").arg(item->typeName()).arg(item->getName());
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
    ui->save_measurement_file_as_action->setEnabled(false);

    updateAttributeTable(0);

    QApplication::instance()->processEvents();
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
                QString name = selected->getName();
                // Show dialog
                edit_measure_dialog *dlg = new edit_measure_dialog(this);

                TDMMeasurementLayerData lda = selected->getPrivateData<TDMMeasurementLayerData>();
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

void TDMGui::slot_patternChanged(MeasurePattern _pattern)
{    
    OSGWidgetTool::instance()->endTool();

    //** + confirmation
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, tr("Pattern changed Confirmation"),
                                                                tr("Do you want change the measurement pattern?\nLoss of data can occur"),
                                                                QMessageBox::Yes | QMessageBox::No,
                                                                QMessageBox::No);
    if (resBtn != QMessageBox::Yes)
    {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    QTableWidget *table = ui->attrib_table;

    QJsonDocument newdoc;

    if(table->rowCount() > 0)
    {
        QJsonArray array;

        for(int i=0; i<table->rowCount(); i++)
        {
            // add row
            QJsonArray row;

            // delete nonexistent fields in new pattern (nothing to do)

            // keep fields with same name AND same type
            // order can change

            // add new fields

            for(int c=0; c<_pattern.getNbFields(); c++)
            {
                // does field exist in old pattern ?
                bool found = false;
                for(int o=0; o< m_current.getNbFields(); o++)
                {
                    if(_pattern.fieldName(c) == m_current.fieldName(o)
                            &&
                            _pattern.fieldType(c) ==  m_current.fieldType(o))
                    {
                        // copy field
                        found = true;
                        MeasureTableWidgetItem *pwidget = (MeasureTableWidgetItem *)table->item(i, o+1);
                        MeasureItem *it = pwidget->measureItem();
                        QJsonObject no;
                        it->encode(no);
                        row.append(no);

                        break;
                    }
                }

                if(!found)
                {
                    // add empty
                    MeasureType::type type = _pattern.fieldType(c);

                    switch(type)
                    {
                    case MeasureType::Line:
                        // line edit widget
                    {
                        osg::ref_ptr<osg::Geode> geode;
                        MeasureLine *l = new MeasureLine(_pattern.fieldName(c), geode);
                        QJsonObject no;
                        l->encode(no);
                        row.append(no);
                        delete l;
                    }
                        break;

                    case MeasureType::Point:
                        // point edit widget
                    {
                        osg::ref_ptr<osg::Geode> geode;
                        MeasurePoint *p = new MeasurePoint(_pattern.fieldName(c), geode);
                        QJsonObject no;
                        p->encode(no);
                        row.append(no);
                        delete p;
                    }
                        break;

                    case MeasureType::Area:
                        // perimeter edit widget
                    {
                        osg::ref_ptr<osg::Geode> geode;
                        MeasureArea *a = new MeasureArea(_pattern.fieldName(c), geode);
                        QJsonObject no;
                        a->encode(no);
                        row.append(no);
                        delete a;
                    }
                        break;

                    default:
                        // string - default editable text line
                    {
                        MeasureString *s = new MeasureString(_pattern.fieldName(c));
                        QJsonObject no;
                        s->encode(no);
                        row.append(no);
                        delete s;
                    }
                        break;
                    }

                }

            }

            array.append(row);
        }

        // store data
        QJsonObject rootobj = newdoc.object();
        rootobj.insert("Data",array);
        newdoc.setObject(rootobj);
    }

    for(int i=m_currentItem->rows().size()-1; i>=0; i--)
        m_currentItem->deleteRow(i);
    delete m_currentItem;
    m_currentItem = 0;
    updateAttributeTable(0);

    // put in TDMMeasurementLayerData

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
                TDMMeasurementLayerData lda = selected->getPrivateData<TDMMeasurementLayerData>();
                lda.pattern().clear();
                for(int i=0; i<_pattern.getNbFields(); i++)
                    lda.pattern().addField(_pattern.fieldName(i), _pattern.fieldType(i));
                selected->setPrivateData<TDMMeasurementLayerData>(lda);
                //qDebug() << "slot_patternChanged" << lda.pattern().getNbFields();
                updateAttributeTable(selected);

                // delete group
                lda.group()->removeChildren(0,lda.group()->getNumChildren());

                // update pattern
                m_current = _pattern;
                m_currentItem = new TDMMeasurementLayerData(lda);

                // load data
                loadData(newdoc, true);
            }
        }
    }

    QApplication::restoreOverrideCursor();
}

void TDMGui::updateAttributeTable(TdmLayerItem *item)
{
    QTableWidget *table = ui->attrib_table;
    if(item != nullptr && item->type() == TdmLayerItem::MeasurementLayer)
    {
        TDMMeasurementLayerData lda = item->getPrivateData<TDMMeasurementLayerData>();
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
    OSGWidgetTool::instance()->endTool();

    QTableWidget *table = ui->attrib_table;
    QTreeView *view = ui->tree_widget;

    // insert last position
    int rowindex = table->rowCount();
    table->setRowCount(rowindex+1);
    // insert first position
    //int rowindex = 0;
    //table->insertRow(rowindex);

    osgMeasurementRow *osgRow = new osgMeasurementRow(m_current);
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

        case MeasureType::Area:
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

        if(selected->type() == TdmLayerItem::MeasurementLayer)
        {
            TDMMeasurementLayerData lda = selected->getPrivateData<TDMMeasurementLayerData>();
            lda.rows() = m_currentItem->rows();
            selected->setPrivateData<TDMMeasurementLayerData>(lda);
        }
    }

    table->selectRow(rowindex);
}

void TDMGui::slot_deleteAttributeLine()
{
    OSGWidgetTool::instance()->endTool();

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
            if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                TDMMeasurementLayerData lda = selected->getPrivateData<TDMMeasurementLayerData>();
                lda.rows() = m_currentItem->rows();
                selected->setPrivateData<TDMMeasurementLayerData>(lda);
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
        OSGWidgetTool::instance()->slot_cancelTool();

        AttribLineWidget *line = (AttribLineWidget *)table->cellWidget(row, column);
        line->clicked();
    }
        break;

    case MeasureType::Point:
        // point edit widget
    {
        OSGWidgetTool::instance()->slot_cancelTool();

        AttribPointWidget *point = (AttribPointWidget *)table->cellWidget(row, column);
        point->clicked();
    }
        break;

    case MeasureType::Area:
        // perimeter edit widget
    {
        OSGWidgetTool::instance()->slot_cancelTool();

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
            (m_currentItem->rows()[row])->setVisible(true);
        }
        else
        {
            // hide line items
            pcur->setNodeMask(0);
            (m_currentItem->rows()[row])->setVisible(false);
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



void TDMGui::selectItem(QModelIndex &index)
{
    QTreeView *view = ui->tree_widget;

    view->selectionModel()->clear();
    view->selectionModel()->clearSelection();

    view->selectionModel()->setCurrentIndex(index,QItemSelectionModel::SelectCurrent);
    view->selectionModel()->select(index,QItemSelectionModel::SelectCurrent);
    QApplication::instance()->processEvents();
}

void TDMGui::slot_messageStartTool(QString&_msg)
{
    ui->cancel_measurement->setEnabled(true);
    statusBar()->showMessage(_msg);
}

void TDMGui::slot_messageCancelTool(QString&_msg)
{
    statusBar()->showMessage(_msg);
}

void TDMGui::slot_messageEndTool(QString&_msg)
{
    ui->cancel_measurement->setEnabled(false);
    statusBar()->showMessage(_msg);
}

void TDMGui::slot_tempLineTool()
{
    ToolLineDialog *dlg = new ToolLineDialog(this);
    QPoint p = QCursor::pos();
    dlg->move(p.x()+20, p.y()+20);
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
}

void TDMGui::slot_tempPointTool()
{
    ToolPointDialog *dlg = new ToolPointDialog(this);
    QPoint p = QCursor::pos();
    dlg->move(p.x()+20, p.y()+20);
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
}

void TDMGui::slot_tempAreaTool()
{
    ToolAreaDialog *dlg = new ToolAreaDialog(this);
    QPoint p = QCursor::pos();
    dlg->move(p.x()+20, p.y()+20);
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
}

void TDMGui::slot_importOldMeasureFile()
{
    // open file
    QString fileName = getOpenFileName(this,tr("Select old measure file to open"), "", tr("Json files (*.json)"));
    if(fileName.length() > 0)
    {
        // read json
        QFile f(fileName);
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray ba = f.readAll();
        f.close();
        QJsonDocument doc = QJsonDocument::fromJson(ba);

        // checks
        if(doc.isNull())
        {
            QMessageBox::information(this, tr("Reading measurement file"), tr("Failed to create Json"));
            return;
        }

        if(!doc.isObject())
        {
            QMessageBox::information(this, tr("Reading measurement file"), tr("Not containing Json object"));
            return;
        }

        QJsonObject obj=doc.object();

        if(obj.isEmpty())
        {
            QMessageBox::information(this, tr("Reading measurement file"), tr("Json object is empty"));
            return;
        }

        QApplication::setOverrideCursor(Qt::WaitCursor);

        // create group (name = json name)
        // create line
        TdmLayersModel *model = TdmLayersModel::instance();
        TdmLayerItem *parent = model->rootItem();

        QFileInfo fi(f.fileName());
        QVariant data(fi.baseName());
        QVariant dummy("group");
        TdmLayerItem *added = model->addLayerItem(TdmLayerItem::GroupLayer, parent, data, dummy);
        added->setChecked(true);

        slot_unselect();

        // original table in old 3DMetrics:
        // Name
        // Type (Line, Point, Area)
        // Category
        // Temperature
        // Result (point:x,y,z, Line:length, Area:surface)
        // Comment

        // add POINTS
        // fields input :
        //      category : String
        //      comment : String
        //      name : String
        //      temp : String
        //  points (array of 1 point)
        MeasurePattern patternPoint;
        patternPoint.addField("Name", MeasureType::type::String);
        patternPoint.addField("Category", MeasureType::type::String);
        patternPoint.addField("Temp", MeasureType::type::String);
        patternPoint.addField("Point", MeasureType::type::Point);
        patternPoint.addField("Comment", MeasureType::type::String);

        QString pointName(fi.baseName() + "-points");
        QVariant pointData(pointName);
        osg::ref_ptr<osg::Group> grouppoint = new osg::Group();
        ui->display_widget->addGroup(grouppoint);
        TDMMeasurementLayerData modelPointData("", patternPoint, grouppoint);
        QVariant toolPoint;
        toolPoint.setValue(modelPointData);
        TdmLayerItem *addedpoint = model->addLayerItem(TdmLayerItem::MeasurementLayer, added, pointData, toolPoint);
        addedpoint->setChecked(true);

        // Load data points
        QJsonArray points = obj["interest_points"].toArray();

        if(!points.isEmpty())
        {
            m_current = patternPoint;

            QJsonArray array;

            for (int i=0; i<points.size(); i++)
            {
                QJsonArray row;

                QJsonObject points_object = points.at(i).toObject();

                QString meas_name = points_object["name"].toString();
                QJsonArray points_vector = points_object["points"].toArray();
                QString meas_comment = points_object["comment"].toString();
                QString meas_temp = points_object["temp"].toString();
                QString meas_category = points_object["category"].toString();

                Point3D p;

                if(points_vector.size() > 0 ) // only 1 point
                {
                    QJsonArray xyz_json = points_vector.at(0).toArray();

                    p.x = xyz_json.at(0).toDouble();
                    p.y = xyz_json.at(1).toDouble();
                    p.z = xyz_json.at(2).toDouble();
                }

                // add line
                osgMeasurementRow *osgRow = new osgMeasurementRow(m_current);
                modelPointData.addRow(osgRow,i);

                //0 : "Name", MeasureType::type::String);
                MeasureString min("Name");
                min.setValue(meas_name);
                QJsonObject on;
                min.encode(on);
                row.append(on);

                //1 : "Category", MeasureType::type::String);
                MeasureString mic("Category");
                mic.setValue(meas_category);
                QJsonObject oc;
                mic.encode(oc);
                row.append(oc);

                //2 : "Temp", MeasureType::type::String);
                MeasureString mit("Temp");
                mit.setValue(meas_temp);
                QJsonObject ot;
                mit.encode(ot);
                row.append(ot);

                //3 : "Point", MeasureType::type::Point);
                osg::ref_ptr<osg::Geode> geode = osgRow->get(3);
                MeasurePoint mip("Point", geode);
                mip.setP(p);
                mip.updateGeode();
                QJsonObject op;
                mip.encode(op);
                row.append(op);

                //4 : "Comment", MeasureType::type::String);
                MeasureString mio("Comment");
                mio.setValue(meas_comment);
                QJsonObject oo;
                mio.encode(oo);
                row.append(oo);

                array.append(row);
            }

            QJsonDocument doc = patternPoint.get();
            QJsonObject rootobj = doc.object();
            rootobj.insert("Data",array);
            doc.setObject(rootobj);
            modelPointData.pattern().set(doc);

            addedpoint->setPrivateData<TDMMeasurementLayerData>(modelPointData);

            updateAttributeTable(0);

            m_currentItem = new TDMMeasurementLayerData(modelPointData);
            loadData(doc, false);

            delete m_currentItem;
            m_currentItem = 0;
            updateAttributeTable(0);
        }

        // add LINES
        // fields input :
        //      category : String
        //      comment : String
        //      length : double (ignored -> recalculated)
        //      name : String
        //      temp : String
        //  points (array)
        MeasurePattern patternLine;
        patternLine.addField("Name", MeasureType::type::String);
        patternLine.addField("Category", MeasureType::type::String);
        patternLine.addField("Temp", MeasureType::type::String);
        patternLine.addField("Line", MeasureType::type::Line);
        patternLine.addField("Comment", MeasureType::type::String);

        QString lineName(fi.baseName() + "-lines");
        QVariant lineData(lineName);
        osg::ref_ptr<osg::Group> groupline = new osg::Group();
        ui->display_widget->addGroup(groupline);
        TDMMeasurementLayerData modelLineData("", patternLine, groupline);
        QVariant toolLine;
        toolLine.setValue(modelLineData);
        TdmLayerItem *addedline = model->addLayerItem(TdmLayerItem::MeasurementLayer, added, lineData, toolLine);
        addedline->setChecked(true);

        // Load data lines
        QJsonArray lines = obj["line_measurements"].toArray();

        if(!lines.isEmpty())
        {
            m_current = patternLine;

            QJsonArray array;

            for (int i=0; i<lines.size(); i++)
            {
                QJsonArray row;

                QJsonObject line_object = lines.at(i).toObject();

                QString meas_name = line_object["name"].toString();
                QJsonArray lines_vector = line_object["points"].toArray();
                QString meas_comment = line_object["comment"].toString();
                QString meas_temp = line_object["temp"].toString();
                QString meas_category = line_object["category"].toString();

                QVector<Point3D> pts;

                for (int j=0; j<lines_vector.size(); j++)
                {
                    QJsonArray xyz_json = lines_vector.at(j).toArray();

                    Point3D p;
                    p.x = xyz_json.at(0).toDouble();
                    p.y = xyz_json.at(1).toDouble();
                    p.z = xyz_json.at(2).toDouble();
                    pts.append(p);
                }

                // add line
                osgMeasurementRow *osgRow = new osgMeasurementRow(m_current);
                modelLineData.addRow(osgRow,i);

                //"Name", MeasureType::type::String);
                MeasureString min("Name");
                min.setValue(meas_name);
                QJsonObject on;
                min.encode(on);
                row.append(on);

                //"Category", MeasureType::type::String);
                MeasureString mic("Category");
                mic.setValue(meas_category);
                QJsonObject oc;
                mic.encode(oc);
                row.append(oc);

                //"Temp", MeasureType::type::String);
                MeasureString mit("Temp");
                mit.setValue(meas_temp);
                QJsonObject ot;
                mit.encode(ot);
                row.append(ot);

                //"Line", MeasureType::type::Line);
                osg::ref_ptr<osg::Geode> geode = osgRow->get(3);
                MeasureLine mil("Line", geode);
                mil.getArray() = pts;
                mil.computeLength();
                mil.updateGeode();
                QJsonObject op;
                mil.encode(op);
                row.append(op);

                //"Comment", MeasureType::type::String);
                MeasureString mio("Comment");
                mio.setValue(meas_comment);
                QJsonObject oo;
                mio.encode(oo);
                row.append(oo);

                array.append(row);
            }

            QJsonDocument doc = patternLine.get();
            QJsonObject rootobj = doc.object();
            rootobj.insert("Data",array);
            doc.setObject(rootobj);
            modelLineData.pattern().set(doc);

            addedline->setPrivateData<TDMMeasurementLayerData>(modelLineData);

            updateAttributeTable(0);

            m_currentItem = new TDMMeasurementLayerData(modelLineData);
            loadData(doc, false);

            delete m_currentItem;
            m_currentItem = 0;
            updateAttributeTable(0);
        }

        // add AREAS
        // fields input :
        //      area : double (ignored : recalculated)
        //      category : String
        //      comment : String
        //      name : String
        //      temp : String
        //  points (warning : origin duplicates first and last point)
        MeasurePattern patternArea;
        patternArea.addField("Name", MeasureType::type::String);
        patternArea.addField("Category", MeasureType::type::String);
        patternArea.addField("Temp", MeasureType::type::String);
        patternArea.addField("Area", MeasureType::type::Area);
        patternArea.addField("Comment", MeasureType::type::String);

        QString areaName(fi.baseName() + "-areas");
        QVariant areaData(areaName);
        osg::ref_ptr<osg::Group> grouparea = new osg::Group();
        ui->display_widget->addGroup(grouparea);
        TDMMeasurementLayerData modelAreaData("", patternArea, grouparea);
        QVariant toolArea;
        toolArea.setValue(modelAreaData);
        TdmLayerItem *addedarea = model->addLayerItem(TdmLayerItem::MeasurementLayer, added, areaData, toolArea);
        addedarea->setChecked(true);

        // Load data area
        QJsonArray areas = obj["area_measurements"].toArray();

        if(!lines.isEmpty())
        {
            m_current = patternArea;

            QJsonArray array;

            for (int i=0; i<areas.size(); i++)
            {
                QJsonArray row;

                QJsonObject area_object = areas.at(i).toObject();

                QString meas_name = area_object["name"].toString();
                QJsonArray areas_vector = area_object["points"].toArray();
                QString meas_comment = area_object["comment"].toString();
                QString meas_temp = area_object["temp"].toString();
                QString meas_category = area_object["category"].toString();

                QVector<Point3D> pts;

                for (int j=0; j<areas_vector.size(); j++)
                {
                    QJsonArray xyz_json = areas_vector.at(j).toArray();

                    Point3D p;
                    p.x = xyz_json.at(0).toDouble();
                    p.y = xyz_json.at(1).toDouble();
                    p.z = xyz_json.at(2).toDouble();
                    pts.append(p);
                }
                // remove last point (because last == first in JSon)
                if(areas_vector.size() > 1
                        && pts[0].x == pts[areas_vector.size()-1].x
                        && pts[0].y == pts[areas_vector.size()-1].y
                        && pts[0].z == pts[areas_vector.size()-1].z )
                {
                    pts.removeLast();
                }

                // add line
                osgMeasurementRow *osgRow = new osgMeasurementRow(m_current);
                modelAreaData.addRow(osgRow,i);

                //"Name", MeasureType::type::String);
                MeasureString min("Name");
                min.setValue(meas_name);
                QJsonObject on;
                min.encode(on);
                row.append(on);

                //"Category", MeasureType::type::String);
                MeasureString mic("Category");
                mic.setValue(meas_category);
                QJsonObject oc;
                mic.encode(oc);
                row.append(oc);

                //"Temp", MeasureType::type::String);
                MeasureString mit("Temp");
                mit.setValue(meas_temp);
                QJsonObject ot;
                mit.encode(ot);
                row.append(ot);

                //"Area", MeasureType::type::Area);
                osg::ref_ptr<osg::Geode> geode = osgRow->get(3);
                MeasureArea mia("Area", geode);
                mia.getArray() = pts;
                mia.updateGeode();
                mia.computeLengthAndArea();
                QJsonObject op;
                mia.encode(op);
                row.append(op);

                //"Comment", MeasureType::type::String);
                MeasureString mio("Comment");
                mio.setValue(meas_comment);
                QJsonObject oo;
                mio.encode(oo);
                row.append(oo);

                array.append(row);
            }

            QJsonDocument doc = patternArea.get();
            QJsonObject rootobj = doc.object();
            rootobj.insert("Data",array);
            doc.setObject(rootobj);
            modelAreaData.pattern().set(doc);

            addedarea->setPrivateData<TDMMeasurementLayerData>(modelAreaData);

            updateAttributeTable(0);

            m_currentItem = new TDMMeasurementLayerData(modelAreaData);
            loadData(doc, false);

            delete m_currentItem;
            m_currentItem = 0;
            updateAttributeTable(0);
        }

        // expand
        QTreeView *view = ui->tree_widget;
        QModelIndex index = model->index(added);
        view->setExpanded(index,true);

        // select created item
        selectItem(index);

        updateAttributeTable(0);
        QItemSelection is;
        slot_selectionChanged(is, is);

        QApplication::restoreOverrideCursor();
    }
}

void TDMGui::slot_openProject()
{
    OSGWidgetTool::instance()->endTool();

    TdmLayerItem *root = TdmLayersModel::instance()->rootItem();

    // ask to save project (if not empty)
    if(root->childCount() > 0)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, tr("Save project file"),
                                                                    tr("Do you want to save current project?"),
                                                                    QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes)
        {
            slot_saveProject();
        }
        if(resBtn == QMessageBox::Cancel)
            return;
    }

    // delete all data
    deleteTreeItemsData(root);
    TdmLayersModel *model = TdmLayersModel::instance();
    if(model->hasChildren())
    {
        model->removeRows(0, model->rowCount());
    }

    // allow measurement to be loaded
    ui->open_measurement_file_action->setEnabled(false);
    ui->import_old_measure_format_action->setEnabled(false);

    // ask file name
    QString fileName = getOpenFileName(this,tr("Select project to open"), "", tr("Json files (*.json)"));
    if(fileName.length() > 0)
    {
        // open project
        QFile f(fileName);
        if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::critical(this, tr("Error : project file"), tr("Error : reading file"));
            return;
        }
        QByteArray ba = f.readAll();
        f.close();

        QJsonDocument doc = QJsonDocument::fromJson(ba);

        QApplication::setOverrideCursor(Qt::WaitCursor);
        // create project structure
        m_projectFileName = fileName;

        buildProjectTree(doc.object(), nullptr);

        QApplication::restoreOverrideCursor();

        slot_unselect();
    }
    else
    {
        QMessageBox::critical(this, tr("Error : project file"), tr("Error : you didn't open a project"));
        return;
    }
}

void TDMGui::buildProjectTree(QJsonObject _obj, TdmLayerItem *_parent)
{
    TdmLayersModel *model = TdmLayersModel::instance();
    TdmLayerItem *root = TdmLayersModel::instance()->rootItem();
    if(_obj.contains("Group"))
    {
        TdmLayerItem *added = root;
        // do not create root node
        if(_parent != nullptr)
        {
            QString group = _obj["Group"].toString();
            QVariant data(group);
            QVariant dummy("group");
            added = model->addLayerItem(TdmLayerItem::GroupLayer, _parent, data, dummy);
            added->setChecked(true);
        }
        // process children
        QJsonArray array = _obj["Children"].toArray();
        for(int i=0; i<array.count(); i++)
        {
            QJsonObject obj = array.at(i).toObject();
            buildProjectTree(obj,added);
        }
    }

    if(_obj.contains("Model3D"))
    {
        QString fileName = _obj["File"].toString();
        QFileInfo projectPath(m_projectFileName);
        QDir dir(projectPath.absoluteDir());
        QString filePath = dir.absoluteFilePath(fileName);

        // load 3D model
        load3DModel(filePath, _parent ? _parent : root, false);

        // allow measurement to be loaded
        ui->open_measurement_file_action->setEnabled(true);
        ui->import_old_measure_format_action->setEnabled(true);
    }

    if(_obj.contains("Measure"))
    {
        QString fileName = _obj["File"].toString();
        QFileInfo projectPath(m_projectFileName);
        QDir dir(projectPath.absoluteDir());
        QString filePath = dir.absoluteFilePath(fileName);

        // loadMeasure
        loadMeasure(filePath, _parent ? _parent : root, false);
    }
}

bool TDMGui::checkAndSaveMeasures(TdmLayerItem *item)
{
    if(item == nullptr)
        return false;

    TdmLayersModel *model = TdmLayersModel::instance();

    if(item->type() == TdmLayerItem::MeasurementLayer)
    {
        if(item->hasData<TDMMeasurementLayerData>())
        {
            QModelIndex index = model->index(item);
            selectItem(index);

            //TDMMeasurementLayerData lda = item->getPrivateData<TDMMeasurementLayerData>();
            if(item->getFileName().isEmpty())
            {
                slot_saveMeasureFileAs();

                //                data1 = item->data(1);
                //                return !data1.value<TDMMeasurementLayerData>().fileName().isEmpty();
                return !item->getFileName().isEmpty();
            }
            else
            {
                TDMMeasurementLayerData lda = item->getPrivateData<TDMMeasurementLayerData>();
                return saveMeasure(lda.fileName(),lda);
            }
        }
    }

    bool res = true;
    if(item->type() == TdmLayerItem::GroupLayer)
    {
        for(int i=0; i<item->childCount(); i++)
        {
            bool reschildren = checkAndSaveMeasures(item->child(i));
            res = res && reschildren;
        }
    }

    return res;
}

QJsonObject TDMGui::saveTreeStructure(TdmLayerItem *item)
{
    QJsonObject obj;
    if(item->type() == TdmLayerItem::GroupLayer)
    {
        QJsonArray array;

        for(int i=0; i<item->childCount(); i++)
        {
            array.append(saveTreeStructure(item->child(i)));
        }
        obj.insert("Group",item->getName());
        obj.insert("Children",array);
    }
    else
    {
        //save relative file name
        QFileInfo fileInfo(m_projectFileName);
        QDir dir = fileInfo.absoluteDir();
        QString relFileName = dir.relativeFilePath(item->getFileName());

        if(item->type() == TdmLayerItem::MeasurementLayer)
        {
            obj.insert("Measure", item->getName());
            obj.insert("File", relFileName);
        }
        if(item->type() == TdmLayerItem::ModelLayer)
        {
            obj.insert("Model3D", item->getName());
            obj.insert("File", relFileName);
        }
    }

    return obj;
}

void TDMGui::slot_saveProject()
{
    OSGWidgetTool::instance()->endTool();

    TdmLayerItem *root = TdmLayersModel::instance()->rootItem();

    // ask to save project (if not empty)
    if(root->childCount() > 0)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, tr("Save project file"),
                                                                    tr("Saving measure mandatory before\nProceed?"),
                                                                    QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn != QMessageBox::Yes)
        {
            QMessageBox::information(this,tr("Save project file"),tr("Project not saved"));
            return;
        }
    }

    // check all measurement have filenames
    // ask to save missing
    // save all measurements
    bool status = checkAndSaveMeasures(TdmLayersModel::instance()->rootItem());
    if(!status)
    {
        QMessageBox::information(this, tr("Save project file"), tr("Not all measurment are saved"));
        return;
    }

    // save in file
    QString name = getSaveFileName(this, tr("Save project"), "",
                                   "*.json");
    QFileInfo fileinfo(name);

    // check filename is not empty
    if(fileinfo.fileName().isEmpty()){
        QMessageBox::critical(this, tr("Error : save project"), tr("Error : you didn't give a name to the file"));
        return;
    }

    // add suffix if needed
    if (fileinfo.suffix() != "json"){
        name += ".json";
        fileinfo.setFile(name);
    }

    // save project structure
    QFile file(name);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, tr("Error : save measurement file"), tr("Error : cannot open file for saving, check path writing rights"));
        return;
    }

    m_projectFileName = fileinfo.absoluteFilePath();

    // build json object
    QJsonDocument json;
    QJsonObject obj = saveTreeStructure(TdmLayersModel::instance()->rootItem());
    json.setObject(obj);

    // write
    QString json_string = json.toJson();
    file.write(json_string.toUtf8());
    file.close();

}
