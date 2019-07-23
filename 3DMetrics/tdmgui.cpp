#include <QCloseEvent>
#include <QProcess>
#include <QMessageBox>

#include "tdmgui.h"
#include "ui_tdmgui.h"

#include "TreeView/tdm_layer_item.h"
#include "TreeView/tdm_layer_model.h"

#include "TreeView/tdm_model_layerdata.h"
#include "TreeView/tdm_measurement_layerdata.h"

#include "file_dialog.h"

#include "edit_meas_dialog.h"
#include "decimation_dialog.h"

#include "attrib_point_widget.h"
#include "attrib_line_widget.h"
#include "attrib_area_widget.h"
#include "attrib_categories_widget.h"

#include "meas_table_widget_item.h"
#include "Measurement/measurement_string.h"
#include "Measurement/measurement_point.h"
#include "Measurement/measurement_line.h"
#include "Measurement/measurement_area.h"
#include "Measurement/measurement_category.h"
#include "Measurement/osg_measurement_row.h"

#include "OSGWidget/osg_widget_tool.h"

#include "tool_point_dialog.h"
#include "tool_line_dialog.h"
#include "tool_area_dialog.h"

#include "osg_axes.h"

#include "Measurement/area_computation_visitor.h"
#include "meas_geom_export_dialog.h"

#include "gdal/ogr_spatialref.h"
#include "gdal/ogrsf_frmts.h"

#include "edit_transparency_model.h"

#include <GeographicLib/LocalCartesian.hpp>

TDMGui::TDMGui(QWidget *_parent) :
    QMainWindow(_parent),
    ui(new Ui::TDMGui),
    m_current_item(0),
    m_settings("3DMetrics", "IFREMER"),
    m_undo_shortcut(this),
    m_help_shortcut(this),
    m_addline_shortcut(this),
    m_stereo_shortcut(this),
    m_delete_shortcut(this),
    m_light_shortcut(this)
{
    qRegisterMetaType<MeasPattern>();

    ui->setupUi(this);

    // set icon
    this->setWindowIcon(QIcon(":/icons/ressources/3dm_icon.svg"));
    m_dialog.setWindowIcon(QIcon(":/icons/ressources/3dm_icon.svg"));

    // to add in reverse because toolbar order is right to left
    m_depth_label = new QLabel("", ui->coords_toolbar);
    m_depth_label->setMinimumWidth(120);
    ui->coords_toolbar->addWidget(m_depth_label);

    m_lon_label = new QLabel("", ui->coords_toolbar);
    m_lon_label->setMinimumWidth(120);
    ui->coords_toolbar->addWidget(m_lon_label);

    m_lat_label = new QLabel("", ui->coords_toolbar);
    m_lat_label->setMinimumWidth(120);
    ui->coords_toolbar->addWidget(m_lat_label);

    ui->tree_widget->setModel(TdmLayersModel::instance());
    ui->tree_widget->hideColumn(1);

    QObject::connect(ui->open_3d_model_action, SIGNAL(triggered()), this, SLOT(slot_open3dModel()));
    QObject::connect(ui->open_measurement_file_action, SIGNAL(triggered()), this, SLOT(slot_openMeasurementFile()));
    QObject::connect(ui->save_measurement_file_action, SIGNAL(triggered()), this, SLOT(slot_saveMeasurementFile()));
    QObject::connect(ui->save_measurement_file_as_action, SIGNAL(triggered()), this, SLOT(slot_saveMeasurementFileAs()));
    QObject::connect(ui->import_old_measurement_format_action, SIGNAL(triggered()), this, SLOT(slot_importOldMeasurementFile()));
    QObject::connect(ui->open_project_action, SIGNAL(triggered()), this, SLOT(slot_openProject()));
    QObject::connect(ui->save_project_action, SIGNAL(triggered()), this, SLOT(slot_saveProject()));
    QObject::connect(ui->layers_tree_window_action, SIGNAL(triggered()), this, SLOT(slot_layersTreeWindow()));
    QObject::connect(ui->attrib_table_window_action, SIGNAL(triggered()), this, SLOT(slot_attribTableWindow()));
    QObject::connect(ui->add_axes_action, SIGNAL(triggered()),this, SLOT(slot_axeView()));
    QObject::connect(ui->stereo_action, SIGNAL(triggered()),this, SLOT(slot_toggleStereoView()));
    QObject::connect(ui->light_action, SIGNAL(triggered()),this, SLOT(slot_toggleLight()));
    QObject::connect(ui->quit_action, SIGNAL(triggered()), this, SLOT(close()));

    QObject::connect(ui->about_action, SIGNAL(triggered()), this, SLOT(slot_about()));

    QObject::connect(ui->tree_widget_dock, SIGNAL(visibilityChanged(bool)), this, SLOT(slot_layersTreeWindowVisibilityChanged(bool)));
    QObject::connect(ui->attrib_table_dock, SIGNAL(visibilityChanged(bool)), this, SLOT(slot_attribTableWindowVisibilityChanged(bool)));

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
    QHeaderView *vertical_header = ui->attrib_table->verticalHeader();
    vertical_header->setSectionResizeMode(QHeaderView::ResizeToContents);
    vertical_header->setDefaultSectionSize(100);

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
    ui->open_measurement_file_action->setEnabled(true);
    ui->save_measurement_file_action->setEnabled(false);
    ui->save_measurement_file_as_action->setEnabled(false);
    ui->import_old_measurement_format_action->setEnabled(false);

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

    connect(ui->display_widget, SIGNAL(signal_onMousePress(Qt::MouseButton, int, int)), this, SLOT(slot_mouseClickInOsgWidget(Qt::MouseButton, int,int)));

    // decimation
    connect(ui->decimate_model_action,SIGNAL(triggered(bool)),this,SLOT(slot_showDecimationDialog()));
    connect(&m_decimation_dialog, SIGNAL(accepted()),this,SLOT(slot_decimateSelectedModel()));

    // csv export
    connect(ui->export_data_to_csv_action,SIGNAL(triggered(bool)),this,SLOT(slot_saveAttribTableToASCII()));

    // snapshot
    connect(ui->take_snapshot_action,SIGNAL(triggered(bool)),this,SLOT(slot_saveSnapshot()));

    // settings
    bool ready_to_apply = true;
    if(m_settings.contains("3DMetrics/path3DModel"))
    {
        ready_to_apply = ready_to_apply && true;
        m_path_model3D = m_settings.value("3DMetrics/path3DModel").value<QString>();
    }else{
        m_path_model3D="";
        ready_to_apply = ready_to_apply && false;
    }
    if(m_settings.contains("3DMetrics/pathMeasurement"))
    {
        ready_to_apply = ready_to_apply && true;
        m_path_measurement = m_settings.value("3DMetrics/pathMeasurement").value<QString>();
    }else{
        m_path_measurement="";
        ready_to_apply = ready_to_apply && false;
    }
    if(m_settings.contains("3DMetrics/pathProject"))
    {
        ready_to_apply = ready_to_apply && true;
        m_path_project = m_settings.value("3DMetrics/pathProject").value<QString>();
    }else{
        m_path_project="";
        ready_to_apply = ready_to_apply && false;
    }
    if(m_settings.contains("3DMetrics/pathSnapshot"))
    {
        ready_to_apply = ready_to_apply && true;
        m_path_snapshot = m_settings.value("3DMetrics/pathSnapshot").value<QString>();
    }else{
        m_path_snapshot="";
        ready_to_apply = ready_to_apply && false;
    }
    if(m_settings.contains("3DMetrics/pathOrthoMap"))
    {
        ready_to_apply = ready_to_apply && true;
        m_path_ortho_map = m_settings.value("3DMetrics/pathOrthoMap").value<QString>();
    }else{
        m_path_ortho_map="";
        ready_to_apply = ready_to_apply && false;
    }
    if(m_settings.contains("3DMetrics/pathDepthMap"))
    {
        ready_to_apply = ready_to_apply && true;
        m_path_depth_map = m_settings.value("3DMetrics/pathDepthMap").value<QString>();
    }else{
        m_path_depth_map="";
        ready_to_apply = ready_to_apply && false;
    }

    if(ready_to_apply)
        slot_applySettings();

    // Keys event
    m_undo_shortcut.setKey(Qt::CTRL + Qt::Key_Z);
    connect(&m_undo_shortcut, SIGNAL(activated()),OSGWidgetTool::instance(), SLOT(slot_removeLastPointTool()));

    m_help_shortcut.setKey(Qt::Key_F1);
    connect(&m_help_shortcut, SIGNAL(activated()),this, SLOT(slot_help()));
    connect(ui->action_user_manual, SIGNAL(triggered(bool)),this, SLOT(slot_help()));

    m_addline_shortcut.setKey(Qt::Key_F2);
    connect(&m_addline_shortcut, SIGNAL(activated()),this, SLOT(slot_addLine ()));

    m_stereo_shortcut.setKey(Qt::Key_F3);
    connect(&m_stereo_shortcut, SIGNAL(activated()),this, SLOT(slot_stereoShortcut()));

    m_delete_shortcut.setKey(Qt::Key_Delete);
    connect(&m_delete_shortcut, SIGNAL(activated()),this, SLOT(slot_deleteRow()));

    m_light_shortcut.setKey(Qt::Key_L);
    connect(&m_light_shortcut, SIGNAL(activated()),this, SLOT(slot_lightShorcut()));

    // export measurement to geometry
    connect(&m_meas_geom_export_dialog, SIGNAL(accepted()),this,SLOT(slot_exportMeasToGeom()));

}

TDMGui::~TDMGui()
{
    delete ui;
}

// ask on close
void TDMGui::closeEvent(QCloseEvent *_event)
{
    QMessageBox::StandardButton res_btn = QMessageBox::question( this, tr("Close 3DMetrics"),
                                                                 tr("Are you sure?\n"),
                                                                 QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                 QMessageBox::Yes);
    if (res_btn != QMessageBox::Yes)
    {
        _event->ignore();
    }
    else
    {
        // to avoid SEGV on exit
        OSGWidgetTool::instance()->endTool();

        _event->accept();
    }
}

void TDMGui::slot_open3dModel()
{
    // Problem on Linux Ubuntu : to be replaced
    //
    //    QString model_file = QFileDialog::getOpenFileName(
    //                this,
    //                "Select one 3d Model to open");

    QString filename = getOpenFileName(this,tr("Select a 3d Model to open"),m_path_model3D, tr("3D files (*.kml *.obj *.ply)"));

    // save Path Model 3D
    m_path_model3D = filename;
    slot_applySettings();

    if(filename.length() > 0)
    {
        FileOpenThread *thread_node = new FileOpenThread();
        connect(thread_node,SIGNAL(signal_createNode(osg::Node*,QString,TdmLayerItem*,bool)), this, SLOT(slot_load3DModel(osg::Node*,QString,TdmLayerItem*,bool)));

        thread_node->setFileName(filename);
        thread_node->setTDMLayerItem(TdmLayersModel::instance()->rootItem());
        thread_node->setSelectItem(true);
        thread_node->setOSGWidget(ui->display_widget);
        thread_node->start();

        // allow measurement to be loaded
        //ui->open_measurement_file_action->setEnabled(true);
        ui->import_old_measurement_format_action->setEnabled(true);

        // measurement tools
        ui->line_tool->setEnabled(true);
        ui->surface_tool->setEnabled(true);
        ui->pick_point->setEnabled(true);

    }
    else
    {
        QMessageBox::information(this, tr("Error : 3d Model"), tr("Error : you didn't open a valid 3d model"));
    }
}


void TDMGui::slot_load3DModel(osg::Node* _node ,QString _filename,TdmLayerItem *_parent, bool _select_item)
{
    if(_node == 0)
    {
        QMessageBox::critical(this, tr("Error : model file"), tr("Error : model file is missing"));
        return;
    }
    TDMModelLayerData model_data(_filename, _node);

    TdmLayersModel *model = TdmLayersModel::instance();
    QFileInfo info(_filename);
    QVariant name(info.fileName());
    QVariant data;
    data.setValue(model_data);

    TdmLayerItem *added = model->addLayerItem(TdmLayerItem::ModelLayer, _parent, name, data);
    added->setChecked(true);

    ui->display_widget->addNodeToScene(_node);

    if(_select_item)
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
    bool has_selection = !view->selectionModel()->selection().isEmpty();

    bool has_current = view->selectionModel()->currentIndex().isValid();
    if (has_selection && has_current)
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
    MeasPattern pattern;
    QString dummy(""); // must be empty
    osg::ref_ptr<osg::Group> group = new osg::Group();
    ui->display_widget->addGroup(group);
    TDMMeasurementLayerData model_data(dummy, pattern, group);
    QVariant tool;
    tool.setValue(model_data);
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

void TDMGui::slot_openMeasurementFile()
{
    QString measurement_filename = getOpenFileName(this,tr("Select measurement file to open"), m_path_measurement, tr("Json files (*.json)"));

    // save Path Measurement
    m_path_measurement = measurement_filename;
    slot_applySettings();

    if(measurement_filename.length() > 0)
    {
        // parent to be used
        TdmLayerItem *parent = TdmLayersModel::instance()->rootItem();

        QTreeView *view = ui->tree_widget;
        bool has_selection = !view->selectionModel()->selection().isEmpty();

        bool has_current = view->selectionModel()->currentIndex().isValid();
        if (has_selection && has_current)
        {
            TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                        view->selectionModel()->currentIndex());
            if(selected != nullptr && selected->type() == TdmLayerItem::GroupLayer)
            {
                parent = selected;
            }
        }

        bool res = loadMeasurementFromFile(measurement_filename, parent, true);
        if(!res)
        {
            QMessageBox::critical(this, tr("Error : measurement file"), tr("Error : invalid file"));
            return;
        }

        ui->save_measurement_file_action->setEnabled(true);
        ui->save_measurement_file_as_action->setEnabled(true);
    }
    else
    {
        QMessageBox::critical(this, tr("Error : measurement file"), tr("Error : you didn't open measurement file"));
    }
}

bool TDMGui::loadMeasurementFromFile(QString _filename, TdmLayerItem *_parent, bool _select_item)
{
    TdmLayersModel *model = TdmLayersModel::instance();
    QTreeView *view = ui->tree_widget;

    QFile measurement_filename(_filename);
    measurement_filename.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray ba = measurement_filename.readAll();
    measurement_filename.close();

    // load pattern
    MeasPattern pattern;
    bool res = pattern.loadFromJson(ba);
    if(!res)
        return false;


    // get reference point from OSG widget
    QPointF lat_lon;
    double depth_org;
    ui->display_widget->getGeoOrigin(lat_lon, depth_org);

    QFileInfo file_measurement_info(measurement_filename.fileName());
    QVariant data(file_measurement_info.fileName());

    osg::ref_ptr<osg::Group> group = new osg::Group();
    ui->display_widget->addGroup(group);
    TDMMeasurementLayerData model_data(measurement_filename.fileName(), pattern, group);
    model_data.setFileName(file_measurement_info.filePath());
    QVariant tool;
    tool.setValue(model_data);
    TdmLayerItem *added = model->addLayerItem(TdmLayerItem::MeasurementLayer, _parent, data, tool);
    added->setPrivateData(model_data);
    updateAttributeTable(added);
    added->setChecked(true);

    TDMMeasurementLayerData *local_data = new TDMMeasurementLayerData(model_data);
    m_current_item = local_data;
    // load data
    m_current_pattern = pattern;
    QJsonDocument doc = pattern.get();
    loadAttribTableFromJson(doc, true);

    pattern.set(doc);
    m_current_pattern = pattern;

    // update doc in modelData
    model_data.pattern().set(doc);

    model_data.rows() = local_data->rows();

    added->setPrivateData(model_data);

    saveAttribTableToJson(doc);
    model_data.pattern().set(doc);
    added->setPrivateData(model_data);

    QModelIndex index = model->index(added);
    view->setExpanded(index.parent(),true);

    // select created item
    if(_select_item)
    {
        selectItem(index);
    }

    if(depth_org == INVALID_VALUE)
        ui->display_widget->home();

    return true;
}

void TDMGui::loadAttribTableFromJson(QJsonDocument &_doc, bool _build_osg)
{  
    OSGWidgetTool::instance()->endTool();

    QTableWidget *table = ui->attrib_table;

    Point3D offset;

    // check reference if available
    if(_doc.object().contains("Reference"))
    {
        QJsonObject reference;

        QJsonObject obj = _doc.object();
        reference = obj["Reference"].toObject();

        if(reference.contains("latitude") && reference.contains("longitude") && reference.contains("depth") )
        {
            // in Json
            double lat_ref = reference["latitude"].toDouble();
            double lon_ref = reference["longitude"].toDouble();
            double depth_ref = reference["depth"].toDouble();

            // in OSGWidget
            // add reference point from OSG widget
            QPointF lat_lon;
            double depth_org;
            ui->display_widget->getGeoOrigin(lat_lon, depth_org);
            qDebug() << "Reference lat=" << lat_lon.x() << " lon=" << lat_lon.y() << " depth=" << depth_org;
            // Update OSGWidget if not initialized
            if(depth_org == INVALID_VALUE)
            {
                lat_lon.setX(lat_ref);
                lat_lon.setY(lon_ref);
                depth_org = depth_ref;
                ui->display_widget->setGeoOrigin(lat_lon, depth_org);
            }
            // Warning : latitude is in x, longitude is in y in OSGWidget
            double lat_org = lat_lon.x();
            double lon_org = lat_lon.y();

            // convert ref in GeoOrigin
            GeographicLib::LocalCartesian proj;
            proj.Reset(lat_org, lon_org, depth_org);
            double x_ref;
            double y_ref;
            double z;
            proj.Forward(lat_ref, lon_ref, depth_ref, x_ref, y_ref,z);

            offset.x = x_ref;
            offset.y = y_ref;

            // depth
            offset.z = depth_ref - depth_org;
            qDebug() << "offsetX=" << offset.x << " offsetY=" << offset.y << " offsetZ=" << offset.z;
            obj.remove("Reference");
            _doc.setObject(obj);
        }
        else
        {
            qDebug() << "NO reference";
        }
    }

    QJsonArray array = _doc.object().value("Data").toArray();
    qDebug() << "loadData buildosg=" << _build_osg << "  nbItems=" << array.count()
             << " osgRowscount=" << m_current_item->rows().size();
    for(int i=0; i<array.count(); i++)
    {
        //row
        table->setRowCount(i+1);
        QJsonArray row = array.at(i).toArray();

        osgMeasurementRow *osg_row = 0;
        if(_build_osg)
        {
            osg_row = new osgMeasurementRow(m_current_pattern);
            m_current_item->addRow(osg_row, i);
        }
        else
        {
            osg_row = m_current_item->rows().at(i);
        }

        // checkbox
        QTableWidgetItem *checkbox = new QTableWidgetItem();
        checkbox->setCheckState( osg_row->isVisible() ? Qt::Checked : Qt::Unchecked);
        checkbox->setSizeHint(QSize(20,20));
        table->setItem(i, 0, checkbox);

        // columns
        for(int j=0; j<m_current_pattern.getNbFields(); j++)
        {
            int column_index_plus1 = j+1;
            QJsonObject obj = row.at(j).toObject();

            MeasType::type meas_type = m_current_pattern.fieldType(j);
            switch(meas_type)
            {
            case MeasType::Line:
                // line edit widget
            {
                MeasTableWidgetItem *pwidget = new MeasTableWidgetItem();

                MeasLine *meas_line = new MeasLine(m_current_pattern.fieldName(j),osg_row->get(j));
                meas_line->decode(obj, offset);
                if(_build_osg)
                {
                    meas_line->updateGeode();
                }
                meas_line->computeLength();
                pwidget->setMeasItem(meas_line);
                table->setItem(i, column_index_plus1, pwidget);
                AttribLineWidget *line = new AttribLineWidget();
                line->setLine(meas_line);
                table->setCellWidget(i,column_index_plus1, line);
                int height = table->rowHeight(i);
                int min_height = line->height() + 2;
                if(min_height > height)
                    table->setRowHeight(i,min_height);
            }
                break;

            case MeasType::Point:
                // point edit widget
            {
                MeasTableWidgetItem *pwidget = new MeasTableWidgetItem();
                MeasPoint *meas_point = new MeasPoint(m_current_pattern.fieldName(j),osg_row->get(j));
                meas_point->decode(obj, offset);
                if(_build_osg)
                {
                    meas_point->updateGeode();
                }
                pwidget->setMeasItem(meas_point);
                table->setItem(i, column_index_plus1, pwidget);
                AttribPointWidget *point = new AttribPointWidget();
                point->setPoint(meas_point);
                table->setCellWidget(i,column_index_plus1, point);
                int height = table->rowHeight(i);
                int min_height = point->height() + 2;
                if(min_height > height)
                    table->setRowHeight(i,min_height);
            }
                break;

            case MeasType::Area:
                // area edit widget
            {
                MeasTableWidgetItem *pwidget = new MeasTableWidgetItem();
                MeasArea *meas_area = new MeasArea(m_current_pattern.fieldName(j),osg_row->get(j));
                meas_area->decode(obj, offset);
                if(_build_osg)
                {
                    meas_area->updateGeode();
                }
                meas_area->computeLengthAndArea();
                pwidget->setMeasItem(meas_area);
                table->setItem(i, column_index_plus1, pwidget);
                AttribAreaWidget *area = new AttribAreaWidget();
                area->setArea(meas_area);
                table->setCellWidget(i,column_index_plus1, area);
                int height = table->rowHeight(i);
                int min_height = area->height() + 2;
                if(min_height > height)
                    table->setRowHeight(i,min_height);
            }
                break;

            case MeasType::Category:
                // category edit widget
            {
                MeasTableWidgetItem *pwidget = new MeasTableWidgetItem();
                MeasCategory *meas_category = new MeasCategory(m_current_pattern.fieldName(j));
                meas_category->decode(obj);
                pwidget->setMeasItem(meas_category);
                table->setItem(i, column_index_plus1, pwidget);
                AttribCategoriesWidget *category = new AttribCategoriesWidget();
                category->setCategory(meas_category);
                table->setCellWidget(i,column_index_plus1, category);
                int height = table->rowHeight(i);
                int min_height = category->height() + 2;
                if(min_height > height)
                    table->setRowHeight(i,min_height);
            }
                break;

            default:
                // string - default editable text line
            {
                MeasTableWidgetItem *pwidget = new MeasTableWidgetItem();
                MeasString *meas_string = new MeasString(m_current_pattern.fieldName(j));
                meas_string->decode(obj);
                pwidget->setMeasItem(meas_string);
                pwidget->setText(meas_string->value());
                table->setItem(i, column_index_plus1, pwidget);
            }
                break;
            }
        }
    }
}

void TDMGui::slot_saveMeasurementFile()
{
    OSGWidgetTool::instance()->endTool();

    // check measurement selected
    QTreeView *view = ui->tree_widget;

    bool has_selection = !view->selectionModel()->selection().isEmpty();
    bool has_current = view->selectionModel()->currentIndex().isValid();
    bool ok = false;

    TDMMeasurementLayerData layer_data;

    if (has_selection && has_current) {
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                layer_data = selected->getPrivateData<TDMMeasurementLayerData>();
                ok = true;
            }
        }
    }

    if(!ok)
        return;

    // get filename
    QString measurement_filename = layer_data.fileName();
    // check filename is not empty
    if(measurement_filename.isEmpty()){
        QMessageBox::critical(this, tr("Error : save measurement"), tr("Error : you didn't give a name to the file"));
        return;
    }

    if(!saveMeasurementToFile(measurement_filename, layer_data))
    {
        QMessageBox::critical(this, tr("Error : save measurement file"), tr("Error : cannot open file for saving, check path writing rights"));
        return;
    }
}

void TDMGui::slot_saveMeasurementFileAs()
{
    OSGWidgetTool::instance()->endTool();

    // check measurement selected
    QTreeView *view = ui->tree_widget;

    bool has_selection = !view->selectionModel()->selection().isEmpty();
    bool has_current = view->selectionModel()->currentIndex().isValid();
    bool ok = false;
    QString name_measurement;

    TDMMeasurementLayerData layer_data;

    if (has_selection && has_current) {
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                layer_data = selected->getPrivateData<TDMMeasurementLayerData>();
                ok = true;
                name_measurement = selected->getName();

            }
        }
    }

    if(!ok)
        return;

    // save in file
    QString measurement_filename = getSaveFileName(this, "Save measurement : "+ name_measurement,m_path_measurement,
                                                   "*.json");

    // save Path Measurement
    m_path_measurement = measurement_filename;
    slot_applySettings();

    QFileInfo file_measurement_info(measurement_filename);

    // check filename is not empty
    if(file_measurement_info.fileName().isEmpty()){
        QMessageBox::critical(this, tr("Error : save measurement"), tr("Error : you didn't give a name to the file"));
        return;
    }

    // add suffix if needed
    if (file_measurement_info.suffix() != "json"){
        measurement_filename += ".json";
        file_measurement_info.setFile(measurement_filename);
    }

    if(!saveMeasurementToFile(measurement_filename, layer_data))
    {
        QMessageBox::critical(this, tr("Error : save measurement file"), tr("Error : cannot open file for saving, check path writing rights"));
        return;
    }

    // store file name
    layer_data.setFileName(file_measurement_info.filePath());
    TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                view->selectionModel()->currentIndex());

    selected->setPrivateData<TDMMeasurementLayerData>(layer_data);
}

bool TDMGui::saveMeasurementToFile(QString _filename, TDMMeasurementLayerData &_data)
{
    QFile measurement_file(_filename);
    if(!measurement_file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    // build json object
    QJsonDocument data_json = _data.pattern().get();
    // add data
    saveAttribTableToJson(data_json);

    // add reference point from OSG widget
    QPointF lat_lon;
    double ref_depth;
    ui->display_widget->getGeoOrigin(lat_lon,ref_depth);
    QJsonObject root_obj = data_json.object();
    QJsonObject reference;
    // Warning : latitude is in x, longitude is in y in OSGWidget
    reference.insert("latitude", QJsonValue(lat_lon.x()));
    reference.insert("longitude", QJsonValue(lat_lon.y()));
    reference.insert("depth", QJsonValue(ref_depth));
    root_obj.insert("Reference",reference);
    data_json.setObject(root_obj);

    // write
    QString json_string = data_json.toJson();
    measurement_file.write(json_string.toUtf8());
    measurement_file.close();

    return true;
}

void TDMGui::saveAttribTableToJson(QJsonDocument &_doc)
{
    QTableWidget *table = ui->attrib_table;
    QJsonArray array;

    for(int i=0; i<table->rowCount(); i++)
    {
        // add row
        QJsonArray row;
        for(int j=1; j<table->columnCount(); j++)
        {
            // add field
            MeasTableWidgetItem *pwidget = (MeasTableWidgetItem *)table->item(i,j);
            MeasItem *item = pwidget->measItem();
            QJsonObject obj;
            item->encode(obj);
            row.append(obj);
        }
        array.append(row);
    }

    QJsonObject root_obj = _doc.object();
    root_obj.insert("Data",array);
    _doc.setObject(root_obj);
}

void TDMGui::slot_saveAttribTableToASCII()
{
    // save in file
    QString out_filename = getSaveFileName(this, tr("Save measurement to csv"), "",
                                           "*.csv");
    QFileInfo fileinfo(out_filename);

    // check filename is not empty
    if(fileinfo.fileName().isEmpty())
    {
        QMessageBox::critical(this, tr("Error : save measurement to csv"), tr("Error : you didn't give a name to the file"));
        return;
    }

    // add suffix if needed
    if (fileinfo.suffix() != "csv")
    {
        out_filename += ".csv";
        fileinfo.setFile(out_filename);
    }

    QFile csv_file(out_filename);
    if(!csv_file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, tr("Error : save measurement to csv"), tr("Error : cannot open the file"));
        return;
    }

    // Get Table
    QTableWidget *table = ui->attrib_table;

    //get and write header
    for(int i=1; i<table->columnCount(); i++)
    {
        QString field_string = table->horizontalHeaderItem(i)->text();
        // write field
        if (i<table->columnCount()-1)
            field_string = field_string + ",";

        csv_file.write(field_string.toUtf8());
    }
    // write end of line
    csv_file.write(QString("\n").toUtf8());

    // write fields data
    for(int i=0; i < table->rowCount(); i++)
    {
        for(int j=1; j < table->columnCount(); j++)
        {
            // add field
            MeasTableWidgetItem *pwidget = (MeasTableWidgetItem *)table->item(i,j);
            MeasItem *item = pwidget->measItem();
            QString field_string;
            item->encodeASCII(field_string);

            // write field
            if (j<table->columnCount()-1)
                field_string = field_string + ",";

            csv_file.write(field_string.toUtf8());
        }
        // write end of line
        csv_file.write(QString("\n").toUtf8());
    }

    // close file
    csv_file.close();
}

void TDMGui::slot_newGroup()
{
    TdmLayersModel *model = TdmLayersModel::instance();
    TdmLayerItem *parent = model->rootItem();

    QTreeView *view = ui->tree_widget;

    bool has_selection = !view->selectionModel()->selection().isEmpty();
    bool has_current = view->selectionModel()->currentIndex().isValid();
    if (has_selection && has_current)
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
            TdmLayerItem *prev_selected = TdmLayersModel::instance()->getLayerItem(prev);
            if(prev_selected != nullptr && prev_selected->type() == TdmLayerItem::MeasurementLayer)
            {
                // save layer attrib data before changing current layer
                TDMMeasurementLayerData layer_data = prev_selected->getPrivateData<TDMMeasurementLayerData>();

                QJsonDocument doc = layer_data.pattern().get();
                saveAttribTableToJson(doc);

                layer_data.pattern().set(doc);

                prev_selected->setPrivateData<TDMMeasurementLayerData>(layer_data);

                delete m_current_item;
                m_current_item = 0;
                updateAttributeTable(0);
            }
        }
    }
    QTreeView *view = ui->tree_widget;

    bool has_selection = !view->selectionModel()->selection().isEmpty();
    bool has_current = view->selectionModel()->currentIndex().isValid();

    if (has_selection && has_current) {
        view->closePersistentEditor(view->selectionModel()->currentIndex());
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            ui->save_measurement_file_action->setEnabled(false);
            ui->save_measurement_file_as_action->setEnabled(false);
            QString fileName = selected->getFileName();

            if(selected->type() == TdmLayerItem::ModelLayer)
            {
                updateAttributeTable(selected);
            }
            else if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                QTableWidget *table = ui->attrib_table;
                table->setRowCount(0);

                TDMMeasurementLayerData layer_data = selected->getPrivateData<TDMMeasurementLayerData>();
                //                if(lda.fileName().length())
                //                    data1 = lda.fileName();

                m_current_item = new TDMMeasurementLayerData(layer_data);
                m_current_pattern = m_current_item->pattern();

                updateAttributeTable(selected);

                QJsonDocument doc = layer_data.pattern().get();
                qDebug() << layer_data.fileName() << " " <<  doc.object().value("Data").toArray().count() << " " << layer_data.rows().size();
                loadAttribTableFromJson(doc, false);

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

void TDMGui::manageCheckStateForChildren(TdmLayerItem *_item, bool _checked)
{
    if(_item == nullptr)
        return;

    bool itemChecked = _item->isChecked();

    if(_item->type() == TdmLayerItem::ModelLayer)
    {
        if(_item->hasData<TDMModelLayerData>())
        {
            TDMModelLayerData layer_data = _item->getPrivateData<TDMModelLayerData>();
            layer_data.node()->setNodeMask(itemChecked && _checked ? 0xFFFFFFFF : 0);
        }
    }

    if(_item->type() == TdmLayerItem::MeasurementLayer)
    {
        if(_item->hasData<TDMMeasurementLayerData>())
        {
            TDMMeasurementLayerData layer_data = _item->getPrivateData<TDMMeasurementLayerData>();
            layer_data.group()->setNodeMask(itemChecked && _checked ? 0xFFFFFFFF : 0);
        }
    }

    if(_item->type() == TdmLayerItem::GroupLayer)
    {
        for(int i=0; i<_item->childCount(); i++)
            manageCheckStateForChildren(_item->child(i), _checked && itemChecked);
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

    bool has_current = view->selectionModel()->currentIndex().isValid();
    if(!has_current)
    {
        menu->addAction(tr("Create new group"), this, SLOT(slot_newGroup()));
        QAction *new_measurement =  menu->addAction(tr("Create new measurement"), this, SLOT(slot_newMeasurement()));

        if(ui->open_measurement_file_action->isEnabled())
            new_measurement->setEnabled(true);
        else
            new_measurement->setDisabled(true);

        menu->exec(QCursor::pos());
        return;
    }

    bool has_selection = !view->selectionModel()->selection().isEmpty();
    view->closePersistentEditor(view->selectionModel()->currentIndex());


    if (has_selection && has_current)
    {
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                menu->addAction(tr("Edit measurement"), this, SLOT(slot_editMeasurement()));
                menu->addAction(tr("Export measurement to geometry"), this, SLOT(slot_showExportMeasToGeom()));
                menu->addSeparator();

            }
            if(selected->type() == TdmLayerItem::ModelLayer)
            {
                menu->addAction(tr("Edit transparency"),this,SLOT(slot_editTransparency()));
                menu->addSeparator();
                menu->addAction(tr("Make an orthographic map"),this,SLOT(slot_saveOrthoMap()));
                menu->addAction(tr("Make an depth map"),this,SLOT(slot_saveDepthMap()));
                menu->addAction(tr("Compute total area"),this,SLOT(slot_computeTotalArea()));
                menu->addSeparator();

            }
        }
    }

    menu->addAction(tr("Rename"), this, SLOT(slot_renameTreeItem()));
    menu->addSeparator();
    menu->addAction(tr("Remove item (Del)"), this, SLOT(slot_deleteRow()));
    menu->addAction(tr("Move item to toplevel"), this, SLOT(slot_moveToToplevel()));
    menu->addSeparator();
    menu->addAction(tr("Create new group"), this, SLOT(slot_newGroup()));
    menu->addSeparator();
    menu->addAction(tr("Create new measurement"), this, SLOT(slot_newMeasurement()));
    menu->addSeparator();
    menu->addAction(tr("Unselect"), this, SLOT(slot_unselect()));
    menu->addSeparator();

    menu->exec(QCursor::pos());
}

// recursively delete datas (in column 1 - not shown in treeview)
void TDMGui::deleteTreeItemsData(TdmLayerItem *_item)
{
    if(_item == nullptr)
        return;

    if(_item->type() == TdmLayerItem::ModelLayer)
    {
        // delete node in osgwidget
        TDMModelLayerData layer_data = _item->getPrivateData<TDMModelLayerData>();
        ui->display_widget->removeNodeFromScene(layer_data.node());
    }
    if(_item->type() == TdmLayerItem::MeasurementLayer)
    {
        if(_item->hasData<TDMMeasurementLayerData>())
        {
            TDMMeasurementLayerData layer_data = _item->getPrivateData<TDMMeasurementLayerData>();
            ui->display_widget->removeGroup(layer_data.group());
        }
    }
    if(_item->type() == TdmLayerItem::GroupLayer)
    {
        for(int i=0; i<_item->childCount(); i++)
            deleteTreeItemsData(_item->child(i));
    }
}

void TDMGui::slot_deleteRow()
{
    QTreeView *view = ui->tree_widget;

    bool has_selection = !view->selectionModel()->selection().isEmpty();
    bool has_current = view->selectionModel()->currentIndex().isValid();

    if (has_selection && has_current)
    {
        QModelIndex index = view->selectionModel()->currentIndex();
        QAbstractItemModel *model = view->model();
        TdmLayerItem *item = (static_cast<TdmLayersModel*>(model))->getLayerItem(index);

        QString msg = tr("Do you want to remove %1:\n%2").arg(item->typeName()).arg(item->getName());
        QMessageBox::StandardButton res_btn = QMessageBox::question( this, tr("Row removal Confirmation"),
                                                                     msg,
                                                                     QMessageBox::Cancel | QMessageBox::Ok,
                                                                     QMessageBox::Cancel);
        if (res_btn != QMessageBox::Ok)
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

    bool has_selection = !view->selectionModel()->selection().isEmpty();
    bool has_current = view->selectionModel()->currentIndex().isValid();

    if (has_selection && has_current)
    {
        view->closePersistentEditor(view->selectionModel()->currentIndex());

        QModelIndex index = view->selectionModel()->currentIndex();
        view->edit(index);
    }
}

void TDMGui::slot_moveToToplevel()
{
    QTreeView *view = ui->tree_widget;

    bool has_selection = !view->selectionModel()->selection().isEmpty();
    bool has_current = view->selectionModel()->currentIndex().isValid();

    if (has_selection && has_current)
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

    bool has_selection = !view->selectionModel()->selection().isEmpty();
    bool has_current = view->selectionModel()->currentIndex().isValid();

    if (has_selection && has_current)
    {
        view->closePersistentEditor(view->selectionModel()->currentIndex());
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                QString title_dialog = selected->getName();
                // Show dialog
                EditMeasDialog *edit_meas_dialog = new EditMeasDialog(this);

                TDMMeasurementLayerData layer_data = selected->getPrivateData<TDMMeasurementLayerData>();
                qDebug() << "init dialog" << layer_data.pattern().getNbFields();
                edit_meas_dialog->setPattern(layer_data.pattern());
                edit_meas_dialog->setWindowTitle(title_dialog);
                edit_meas_dialog->setModal(true);

                QObject::connect(edit_meas_dialog,SIGNAL(signal_apply(MeasPattern)),
                                 this,SLOT(slot_patternChanged(MeasPattern)));

                edit_meas_dialog->show();
            }
        }
    }
}

void TDMGui::slot_patternChanged(MeasPattern _pattern)
{    
    OSGWidgetTool::instance()->endTool();

    //** + confirmation
    QMessageBox::StandardButton res_btn = QMessageBox::question( this, tr("Pattern changed Confirmation"),
                                                                 tr("Do you want change the measurement pattern?\nLoss of data can occur"),
                                                                 QMessageBox::Yes | QMessageBox::No,
                                                                 QMessageBox::No);
    if (res_btn != QMessageBox::Yes)
    {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    QTableWidget *table = ui->attrib_table;

    QJsonDocument new_doc;

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

            for(int j=0; j<_pattern.getNbFields(); j++)
            {
                // does field exist in old pattern ?
                bool found = false;
                for(int k=0; k< m_current_pattern.getNbFields(); k++)
                {
                    if(_pattern.fieldName(k) == m_current_pattern.fieldName(k)
                            &&
                            _pattern.fieldType(k) ==  m_current_pattern.fieldType(k))
                    {
                        // copy field
                        found = true;
                        MeasTableWidgetItem *pwidget = (MeasTableWidgetItem *)table->item(i, k+1);
                        MeasItem *item = pwidget->measItem();
                        QJsonObject obj;
                        item->encode(obj);
                        row.append(obj);

                        break;
                    }
                }

                if(!found)
                {
                    // add empty
                    MeasType::type type = _pattern.fieldType(j);

                    switch(type)
                    {
                    case MeasType::Line:
                        // line edit widget
                    {
                        osg::ref_ptr<osg::Geode> geode;
                        MeasLine *meas_line = new MeasLine(_pattern.fieldName(j), geode);
                        QJsonObject line_json;
                        meas_line->encode(line_json);
                        row.append(line_json);
                        delete meas_line;
                    }
                        break;

                    case MeasType::Point:
                        // point edit widget
                    {
                        osg::ref_ptr<osg::Geode> geode;
                        MeasPoint *meas_point = new MeasPoint(_pattern.fieldName(j), geode);
                        QJsonObject point_json;
                        meas_point->encode(point_json);
                        row.append(point_json);
                        delete meas_point;
                    }
                        break;

                    case MeasType::Area:
                        // area edit widget
                    {
                        osg::ref_ptr<osg::Geode> geode;
                        MeasArea *meas_area = new MeasArea(_pattern.fieldName(j), geode);
                        QJsonObject area_json;
                        meas_area->encode(area_json);
                        row.append(area_json);
                        delete meas_area;
                    }
                        break;

                    case MeasType::Category:
                        // category edit widget
                    {
                        MeasCategory *meas_category = new MeasCategory(_pattern.fieldName(j));
                        QJsonObject category_json;
                        meas_category->encode(category_json);
                        row.append(category_json);
                        delete meas_category;
                    }
                        break;

                    default:
                        // string - default editable text line
                    {
                        MeasString *meas_string = new MeasString(_pattern.fieldName(j));
                        QJsonObject string_json;
                        meas_string->encode(string_json);
                        row.append(string_json);
                        delete meas_string;
                    }
                        break;
                    }

                }

            }

            array.append(row);
        }

        // store data
        QJsonObject root_obj = new_doc.object();
        root_obj.insert("Data",array);
        new_doc.setObject(root_obj);
    }

    for(int i=m_current_item->rows().size()-1; i>=0; i--)
        m_current_item->deleteRow(i);
    delete m_current_item;
    m_current_item = 0;
    updateAttributeTable(0);

    // put in TDMMeasurementLayerData

    QTreeView *view = ui->tree_widget;

    bool has_selection = !view->selectionModel()->selection().isEmpty();
    bool has_current = view->selectionModel()->currentIndex().isValid();

    if (has_selection && has_current)
    {
        view->closePersistentEditor(view->selectionModel()->currentIndex());
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                TDMMeasurementLayerData layer_data = selected->getPrivateData<TDMMeasurementLayerData>();
                layer_data.pattern().clear();
                for(int j=0; j < _pattern.getNbFields(); j++)
                    layer_data.pattern().addField(_pattern.fieldName(j), _pattern.fieldType(j));
                selected->setPrivateData<TDMMeasurementLayerData>(layer_data);
                //qDebug() << "slot_patternChanged" << lda.pattern().getNbFields();
                updateAttributeTable(selected);

                // delete group
                layer_data.group()->removeChildren(0,layer_data.group()->getNumChildren());

                // update pattern
                m_current_pattern = _pattern;
                m_current_item = new TDMMeasurementLayerData(layer_data);

                // load data
                loadAttribTableFromJson(new_doc, true);
            }
        }
    }

    QApplication::restoreOverrideCursor();
}

void TDMGui::updateAttributeTable(TdmLayerItem *_item)
{
    QTableWidget *table = ui->attrib_table;
    if(_item != nullptr && _item->type() == TdmLayerItem::MeasurementLayer)
    {
        TDMMeasurementLayerData layer_data = _item->getPrivateData<TDMMeasurementLayerData>();
        int nb_fields = layer_data.pattern().getNbFields();

        table->setColumnCount(nb_fields+1);
        QStringList headers;
        headers << ""; //tr("[+]");

        for(int i=0; i<layer_data.pattern().getNbFields(); i++)
        {
            QString head = layer_data.pattern().fieldName(i); // + "\n(" + lda.pattern().fieldTypeName(i) + ")";
            headers << head;
        }
        table->setHorizontalHeaderLabels(headers);
        for(int i=0; i<layer_data.pattern().getNbFields(); i++)
        {
            QString tt = "(" + layer_data.pattern().fieldTypeName(i) + ")";
            QTableWidgetItem* header_item = table->horizontalHeaderItem(i+1);
            if (header_item)
                header_item->setToolTip(tt);

            // column width
            switch(layer_data.pattern().fieldType(i))
            {
            case MeasType::Area:
                table->setColumnWidth(i+1,150);

                break;
            case MeasType::Line:
                table->setColumnWidth(i+1,150);

                break;
            case MeasType::Point:
                table->setColumnWidth(i+1,160);

                break;
            case MeasType::String:
                table->setColumnWidth(i+1,125);

                break;
            case MeasType::Category:
                table->setColumnWidth(i+1,125);

                break;
            default:
                break;
            }
        }
        table->verticalHeader()->setVisible(true);
        table->setRowCount(0);
        m_current_pattern = layer_data.pattern();
    }
    else
    {
        table->setColumnCount(1);
        QStringList headers;
        headers << ""; //tr("[+]");
        table->setHorizontalHeaderLabels(headers);

        table->setRowCount(0);
        m_current_pattern.clear();
    }

    // 1rst column : check
    table->setColumnWidth(0,30);
    QTableWidgetItem* header_item = table->horizontalHeaderItem(0);
    if (header_item)
        header_item->setToolTip("Visibility");
}

void TDMGui::slot_attribTableContextMenu(const QPoint &)
{
    QMenu *menu = new QMenu;
    QTableWidget *table = ui->attrib_table;

    QTreeView *view = ui->tree_widget;

    bool has_current_tree = view->selectionModel()->currentIndex().isValid();
    bool has_selection_tree = !view->selectionModel()->selection().isEmpty();
    view->closePersistentEditor(view->selectionModel()->currentIndex());

    if(has_selection_tree && has_current_tree)
    {
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            if(selected->type() == TdmLayerItem::MeasurementLayer)
            {

                TDMMeasurementLayerData layer_data = selected->getPrivateData<TDMMeasurementLayerData>();
                if(layer_data.pattern().getNbFields() !=0 ) menu->addAction(tr("Add line (F1)"), this, SLOT(slot_addAttributeLine()));
            }
        }
    }


    bool has_current_attrib = table->selectionModel()->currentIndex().isValid();
    bool has_selection_attrib = !table->selectionModel()->selection().isEmpty();

    if (has_selection_attrib && has_current_attrib)
    {
        menu->addAction(tr("Remove line"), this, SLOT(slot_deleteAttributeLine()));
    }

    menu->exec(QCursor::pos());
}

void TDMGui::slot_addAttributeLine()
{   
    OSGWidgetTool::instance()->endTool();

    QTableWidget *table = ui->attrib_table;
    QTreeView *view = ui->tree_widget;

    // insert last position
    int row_index = table->rowCount();
    table->setRowCount(row_index+1);
    // insert first position
    //int rowindex = 0;
    //table->insertRow(rowindex);

    osgMeasurementRow *osg_row = new osgMeasurementRow(m_current_pattern);
    m_current_item->addRow(osg_row, row_index);

    QTableWidgetItem *checkbox = new QTableWidgetItem();
    checkbox->setCheckState(Qt::Checked);
    checkbox->setSizeHint(QSize(20,20));
    table->setItem(row_index, 0, checkbox);

    // process items in line
    for(int i=1; i<table->columnCount(); i++)
    {
        MeasType::type meas_type = m_current_pattern.fieldType(i-1);
        switch(meas_type)
        {
        case MeasType::Line:
            // line edit widget
        {
            MeasTableWidgetItem *pwidget = new MeasTableWidgetItem();
            MeasLine *meas_line = new MeasLine(m_current_pattern.fieldName(i-1),osg_row->get(i-1));
            pwidget->setMeasItem(meas_line);
            table->setItem(row_index, i, pwidget);
            AttribLineWidget *line = new AttribLineWidget();
            line->setLine(meas_line);
            table->setCellWidget(row_index,i, line);
            int height = table->rowHeight(row_index);
            int min_height = line->height() + 2;
            if(min_height > height)
                table->setRowHeight(row_index,min_height);
        }
            break;

        case MeasType::Point:
            // point edit widget
        {
            MeasTableWidgetItem *pwidget = new MeasTableWidgetItem();
            MeasPoint *meas_point = new MeasPoint(m_current_pattern.fieldName(i-1),osg_row->get(i-1));
            //point->updateGeode();
            pwidget->setMeasItem(meas_point);
            table->setItem(row_index, i, pwidget);
            AttribPointWidget *point = new AttribPointWidget();
            point->setPoint(meas_point, false);
            table->setCellWidget(row_index,i, point);
            int height = table->rowHeight(row_index);
            int min_height = point->height() + 2;
            if(min_height > height)
                table->setRowHeight(row_index,min_height);
        }
            break;

        case MeasType::Area:
            // area edit widget
        {
            MeasTableWidgetItem *pwidget = new MeasTableWidgetItem();
            MeasArea *meas_area = new MeasArea(m_current_pattern.fieldName(i-1),osg_row->get(i-1));
            pwidget->setMeasItem(meas_area);
            table->setItem(row_index, i, pwidget);
            AttribAreaWidget *area = new AttribAreaWidget();
            area->setArea(meas_area);
            //            area->setNbval("");
            //            area->setAreaval("");
            table->setCellWidget(row_index,i, area);
            int height = table->rowHeight(row_index);
            int min_height = area->height() + 2;
            if(min_height > height)
                table->setRowHeight(row_index,min_height);
        }
            break;

        case MeasType::Category:
            // category edit widget
        {
            MeasTableWidgetItem *pwidget = new MeasTableWidgetItem();
            MeasCategory *meas_category = new MeasCategory(m_current_pattern.fieldName(i-1));
            pwidget->setMeasItem(meas_category);
            table->setItem(row_index, i, pwidget);
            AttribCategoriesWidget *category = new AttribCategoriesWidget();
            category->setCategory(meas_category); category->initItem();
            table->setCellWidget(row_index,i, category);
            int height = table->rowHeight(row_index);
            int min_height = category->height() + 2;
            if(min_height > height)
                table->setRowHeight(row_index,min_height);
        }
            break;


        default:
            // string - default editable text line
        {
            MeasTableWidgetItem *pwidget = new MeasTableWidgetItem();
            pwidget->setMeasItem(new MeasString(m_current_pattern.fieldName(i-1)));
            table->setItem(row_index, i, pwidget);
        }
            break;
        }
    }

    // save in current selection
    TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                view->selectionModel()->currentIndex());
    if(selected != nullptr)
    {

        if(selected->type() == TdmLayerItem::MeasurementLayer)
        {
            TDMMeasurementLayerData layer_data = selected->getPrivateData<TDMMeasurementLayerData>();
            layer_data.rows() = m_current_item->rows();
            // add in json doc
            MeasPattern pattern = layer_data.pattern();
            QJsonDocument doc = pattern.get();
            saveAttribTableToJson(doc);
            pattern.set(doc);
            layer_data.pattern() = pattern;
            selected->setPrivateData<TDMMeasurementLayerData>(layer_data);
        }
    }

    table->selectRow(row_index);
}

void TDMGui::slot_deleteAttributeLine()
{
    OSGWidgetTool::instance()->endTool();

    QTableWidget *table = ui->attrib_table;
    QTreeView *view = ui->tree_widget;

    bool has_current = table->selectionModel()->currentIndex().isValid();
    bool has_selection = !table->selectionModel()->selection().isEmpty();
    if (has_selection && has_current)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, tr("Row removal Confirmation"),
                                                                    tr("Do you want to remove the selected row?"),
                                                                    QMessageBox::Cancel | QMessageBox::Ok,
                                                                    QMessageBox::Cancel);
        if (resBtn != QMessageBox::Ok)
        {
            return;
        }

        // remove in osg
        int row = table->currentRow();
        m_current_item->deleteRow(row);
        // save in actual selection
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            if(selected->type() == TdmLayerItem::MeasurementLayer)
            {
                TDMMeasurementLayerData layer_data = selected->getPrivateData<TDMMeasurementLayerData>();
                layer_data.rows() = m_current_item->rows();
                // remove in json doc
                MeasPattern pattern = layer_data.pattern();
                QJsonDocument doc = pattern.get();
                QJsonObject root_obj = doc.object();
                QJsonArray array = root_obj["Data"].toArray();
                array.removeAt(row);
                root_obj.insert("Data",array);
                doc.setObject(root_obj);
                pattern.set(doc);
                saveAttribTableToJson(doc);
                layer_data.pattern() = pattern;
                selected->setPrivateData<TDMMeasurementLayerData>(layer_data);
            }
        }
        // removal of data : done by destructor
        table->removeRow(row);
    }
}

void TDMGui::slot_attribTableDoubleClick(int _row, int _column)
{
    QTableWidget *table = ui->attrib_table;
    MeasType::type meas_type = m_current_pattern.fieldType(_column-1);

    switch(meas_type)
    {
    case MeasType::Line:
        // line edit widget
    {
        OSGWidgetTool::instance()->slot_cancelTool();

        AttribLineWidget *line = (AttribLineWidget *)table->cellWidget(_row, _column);
        line->clicked();
    }
        break;

    case MeasType::Point:
        // point edit widget
    {
        OSGWidgetTool::instance()->slot_cancelTool();

        AttribPointWidget *point = (AttribPointWidget *)table->cellWidget(_row, _column);
        point->clicked();
    }
        break;

    case MeasType::Area:
        // area edit widget
    {
        OSGWidgetTool::instance()->slot_cancelTool();

        AttribAreaWidget *area = (AttribAreaWidget *)table->cellWidget(_row, _column);
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

void TDMGui::slot_attribTableCellChanged(int _row, int _column)
{
    if(_column == 0 && m_current_item->rows().size() > 0)
    {
        // check state
        QTableWidget *table = ui->attrib_table;
        QTableWidgetItem *table_widget_item = table->item(_row, _column);
        osg::ref_ptr<osg::Group> current_widget = (m_current_item->rows()[_row])->getGroup();
        if(table_widget_item->checkState() == Qt::Checked)
        {
            // show line items
            current_widget->setNodeMask(0xFFFFFFFF);
            (m_current_item->rows()[_row])->setVisible(true);
        }
        else
        {
            // hide line items
            current_widget->setNodeMask(0);
            (m_current_item->rows()[_row])->setVisible(false);
        }

        return;
    }

    MeasType::type meas_type = m_current_pattern.fieldType(_column-1);

    switch(meas_type)
    {
    case MeasType::String:
    {
        // update MeasString data
        QTableWidget *table = ui->attrib_table;
        MeasTableWidgetItem *meas_table_widget_item = (MeasTableWidgetItem *)table->item(_row, _column);
        ((MeasString*)meas_table_widget_item->measItem())->setValue(table->item(_row,_column)->text());
        break;
    }
    default:
        break;
    }
}

void TDMGui::selectItem(QModelIndex &_index)
{
    QTreeView *view = ui->tree_widget;

    view->selectionModel()->clear();
    view->selectionModel()->clearSelection();

    view->selectionModel()->setCurrentIndex(_index,QItemSelectionModel::SelectCurrent);
    view->selectionModel()->select(_index,QItemSelectionModel::SelectCurrent);
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
    ToolLineDialog *line_dialog = new ToolLineDialog(this);
    QPoint point = QCursor::pos();
    line_dialog->move(point.x()+20, point.y()+20);
    line_dialog->show();
    line_dialog->raise();
    line_dialog->activateWindow();
}

void TDMGui::slot_tempPointTool()
{
    ToolPointDialog *point_dialog = new ToolPointDialog(this);
    QPoint point = QCursor::pos();
    point_dialog->move(point.x()+20, point.y()+20);
    point_dialog->show();
    point_dialog->raise();
    point_dialog->activateWindow();
}

void TDMGui::slot_tempAreaTool()
{
    ToolAreaDialog *area_dialog = new ToolAreaDialog(this);
    QPoint point = QCursor::pos();
    area_dialog->move(point.x()+20, point.y()+20);
    area_dialog->show();
    area_dialog->raise();
    area_dialog->activateWindow();
}

void TDMGui::slot_importOldMeasurementFile()
{
    // open file
    QString old_meas_fileName = getOpenFileName(this,tr("Select old measurement file to open"), m_path_measurement, tr("Json files (*.json)"));

    // save Path Measurement
    m_path_measurement = old_meas_fileName;
    slot_applySettings();

    if(old_meas_fileName.length() > 0)
    {
        // read json
        QFile old_meas_file(old_meas_fileName);
        old_meas_file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray ba = old_meas_file.readAll();
        old_meas_file.close();
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

        QFileInfo old_meas_info(old_meas_file.fileName());
        QVariant data(old_meas_info.baseName());
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
        MeasPattern pattern_point;
        pattern_point.addField("Name", MeasType::type::String);
        pattern_point.addField("Category", MeasType::type::String);
        pattern_point.addField("Temp", MeasType::type::String);
        pattern_point.addField("Point", MeasType::type::Point);
        pattern_point.addField("Comment", MeasType::type::String);

        QString point_name(old_meas_info.baseName() + "-points");
        QVariant point_data(point_name);
        osg::ref_ptr<osg::Group> group_point = new osg::Group();
        ui->display_widget->addGroup(group_point);
        TDMMeasurementLayerData model_point_data("", pattern_point, group_point);
        QVariant tool_point;
        tool_point.setValue(model_point_data);
        TdmLayerItem *added_point = model->addLayerItem(TdmLayerItem::MeasurementLayer, added, point_data, tool_point);
        added_point->setChecked(true);

        // Load data points
        QJsonArray points = obj["interest_points"].toArray();

        if(!points.isEmpty())
        {
            m_current_pattern = pattern_point;

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

                Point3D point;

                if(points_vector.size() > 0 ) // only 1 point
                {
                    QJsonArray xyz_json = points_vector.at(0).toArray();

                    point.x = xyz_json.at(0).toDouble();
                    point.y = xyz_json.at(1).toDouble();
                    point.z = xyz_json.at(2).toDouble();
                }

                // add line
                osgMeasurementRow *osg_row = new osgMeasurementRow(m_current_pattern);
                model_point_data.addRow(osg_row,i);

                //0 : "Name", MeasType::type::String);
                MeasString name_meas_string("Name");
                name_meas_string.setValue(meas_name);
                QJsonObject name_json;
                name_meas_string.encode(name_json);
                row.append(name_json);

                //1 : "Category", MeasType::type::String);
                MeasString cat_meas_string("Category");
                cat_meas_string.setValue(meas_category);
                QJsonObject category_json;
                cat_meas_string.encode(category_json);
                row.append(category_json);

                //2 : "Temp", MeasType::type::String);
                MeasString temp_meas_string("Temp");
                temp_meas_string.setValue(meas_temp);
                QJsonObject temp_json;
                temp_meas_string.encode(temp_json);
                row.append(temp_json);

                //3 : "Point", MeasType::type::Point);
                osg::ref_ptr<osg::Geode> geode = osg_row->get(3);
                MeasPoint point_meas_point("Point", geode);
                point_meas_point.setP(point);
                point_meas_point.updateGeode();
                QJsonObject point_json;
                point_meas_point.encode(point_json);
                row.append(point_json);

                //4 : "Comment", MeasType::type::String);
                MeasString comment_meas_string("Comment");
                comment_meas_string.setValue(meas_comment);
                QJsonObject comment_json;
                comment_meas_string.encode(comment_json);
                row.append(comment_json);

                array.append(row);
            }

            QJsonDocument doc = pattern_point.get();
            QJsonObject root_obj = doc.object();
            root_obj.insert("Data",array);
            doc.setObject(root_obj);
            model_point_data.pattern().set(doc);

            added_point->setPrivateData<TDMMeasurementLayerData>(model_point_data);

            updateAttributeTable(0);

            m_current_item = new TDMMeasurementLayerData(model_point_data);
            loadAttribTableFromJson(doc, false);

            delete m_current_item;
            m_current_item = 0;
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
        MeasPattern pattern_line;
        pattern_line.addField("Name", MeasType::type::String);
        pattern_line.addField("Category", MeasType::type::String);
        pattern_line.addField("Temp", MeasType::type::String);
        pattern_line.addField("Line", MeasType::type::Line);
        pattern_line.addField("Comment", MeasType::type::String);

        QString line_name(old_meas_info.baseName() + "-lines");
        QVariant line_data(line_name);
        osg::ref_ptr<osg::Group> group_line = new osg::Group();
        ui->display_widget->addGroup(group_line);
        TDMMeasurementLayerData model_line_data("", pattern_line, group_line);
        QVariant tool_line;
        tool_line.setValue(model_line_data);
        TdmLayerItem *added_line = model->addLayerItem(TdmLayerItem::MeasurementLayer, added, line_data, tool_line);
        added_line->setChecked(true);

        // Load data lines
        QJsonArray lines = obj["line_measurements"].toArray();

        if(!lines.isEmpty())
        {
            m_current_pattern = pattern_line;

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

                QVector<Point3D> points_vector;

                for (int j=0; j<lines_vector.size(); j++)
                {
                    QJsonArray xyz_json = lines_vector.at(j).toArray();

                    Point3D point;
                    point.x = xyz_json.at(0).toDouble();
                    point.y = xyz_json.at(1).toDouble();
                    point.z = xyz_json.at(2).toDouble();
                    points_vector.append(point);
                }

                // add line
                osgMeasurementRow *osg_row = new osgMeasurementRow(m_current_pattern);
                model_line_data.addRow(osg_row,i);

                //"Name", MeasType::type::String);
                MeasString name_meas_string("Name");
                name_meas_string.setValue(meas_name);
                QJsonObject name_json;
                name_meas_string.encode(name_json);
                row.append(name_json);

                //"Category", MeasType::type::String);
                MeasString cat_meas_string("Category");
                cat_meas_string.setValue(meas_category);
                QJsonObject category_json;
                cat_meas_string.encode(category_json);
                row.append(category_json);

                //"Temp", MeasType::type::String);
                MeasString temp_meas_string("Temp");
                temp_meas_string.setValue(meas_temp);
                QJsonObject temp_json;
                temp_meas_string.encode(temp_json);
                row.append(temp_json);

                //"Line", MeasType::type::Line);
                osg::ref_ptr<osg::Geode> geode = osg_row->get(3);
                MeasLine line_meas_line("Line", geode);
                line_meas_line.getArray() = points_vector;
                line_meas_line.computeLength();
                line_meas_line.updateGeode();
                QJsonObject line_json;
                line_meas_line.encode(line_json);
                row.append(line_json);

                //"Comment", MeasType::type::String);
                MeasString comment_meas_string("Comment");
                comment_meas_string.setValue(meas_comment);
                QJsonObject comment_json;
                comment_meas_string.encode(comment_json);
                row.append(comment_json);

                array.append(row);
            }

            QJsonDocument doc = pattern_line.get();
            QJsonObject root_obj = doc.object();
            root_obj.insert("Data",array);
            doc.setObject(root_obj);
            model_line_data.pattern().set(doc);

            added_line->setPrivateData<TDMMeasurementLayerData>(model_line_data);

            updateAttributeTable(0);

            m_current_item = new TDMMeasurementLayerData(model_line_data);
            loadAttribTableFromJson(doc, false);

            delete m_current_item;
            m_current_item = 0;
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
        MeasPattern pattern_area;
        pattern_area.addField("Name", MeasType::type::String);
        pattern_area.addField("Category", MeasType::type::String);
        pattern_area.addField("Temp", MeasType::type::String);
        pattern_area.addField("Area", MeasType::type::Area);
        pattern_area.addField("Comment", MeasType::type::String);

        QString area_name(old_meas_info.baseName() + "-areas");
        QVariant area_data(area_name);
        osg::ref_ptr<osg::Group> group_area = new osg::Group();
        ui->display_widget->addGroup(group_area);
        TDMMeasurementLayerData model_area_data("", pattern_area, group_area);
        QVariant tool_area;
        tool_area.setValue(model_area_data);
        TdmLayerItem *added_area = model->addLayerItem(TdmLayerItem::MeasurementLayer, added, area_data, tool_area);
        added_area->setChecked(true);

        // Load data area
        QJsonArray areas = obj["area_measurements"].toArray();

        if(!lines.isEmpty())
        {
            m_current_pattern = pattern_area;

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

                QVector<Point3D> points_vector;

                for (int j=0; j<areas_vector.size(); j++)
                {
                    QJsonArray xyz_json = areas_vector.at(j).toArray();

                    Point3D point;
                    point.x = xyz_json.at(0).toDouble();
                    point.y = xyz_json.at(1).toDouble();
                    point.z = xyz_json.at(2).toDouble();
                    points_vector.append(point);
                }
                // remove last point (because last == first in JSon)
                if(areas_vector.size() > 1
                        && points_vector[0].x == points_vector[areas_vector.size()-1].x
                        && points_vector[0].y == points_vector[areas_vector.size()-1].y
                        && points_vector[0].z == points_vector[areas_vector.size()-1].z )
                {
                    points_vector.removeLast();
                }

                // add line
                osgMeasurementRow *osg_row = new osgMeasurementRow(m_current_pattern);
                model_area_data.addRow(osg_row,i);

                //"Name", MeasType::type::String);
                MeasString name_meas_string("Name");
                name_meas_string.setValue(meas_name);
                QJsonObject name_json;
                name_meas_string.encode(name_json);
                row.append(name_json);

                //"Category", MeasType::type::String);
                MeasString cat_meas_string("Category");
                cat_meas_string.setValue(meas_category);
                QJsonObject category_json;
                cat_meas_string.encode(category_json);
                row.append(category_json);

                //"Temp", MeasType::type::String);
                MeasString temp_meas_string("Temp");
                temp_meas_string.setValue(meas_temp);
                QJsonObject temp_json;
                temp_meas_string.encode(temp_json);
                row.append(temp_json);

                //"Area", MeasType::type::Area);
                osg::ref_ptr<osg::Geode> geode = osg_row->get(3);
                MeasArea area_meas_area("Area", geode);
                area_meas_area.getArray() = points_vector;
                area_meas_area.updateGeode();
                area_meas_area.computeLengthAndArea();
                QJsonObject area_json;
                area_meas_area.encode(area_json);
                row.append(area_json);

                //"Comment", MeasType::type::String);
                MeasString comment_meas_string("Comment");
                comment_meas_string.setValue(meas_comment);
                QJsonObject comment_json;
                comment_meas_string.encode(comment_json);
                row.append(comment_json);

                array.append(row);
            }

            QJsonDocument doc = pattern_area.get();
            QJsonObject root_obj = doc.object();
            root_obj.insert("Data",array);
            doc.setObject(root_obj);
            model_area_data.pattern().set(doc);

            added_area->setPrivateData<TDMMeasurementLayerData>(model_area_data);

            updateAttributeTable(0);

            m_current_item = new TDMMeasurementLayerData(model_area_data);
            loadAttribTableFromJson(doc, false);

            delete m_current_item;
            m_current_item = 0;
            updateAttributeTable(0);
        }

        // expand
        QTreeView *view = ui->tree_widget;
        QModelIndex index = model->index(added);
        view->setExpanded(index,true);

        // select created item
        selectItem(index);

        updateAttributeTable(0);
        QItemSelection item_selection;
        slot_selectionChanged(item_selection, item_selection);

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
        QMessageBox::StandardButton res_btn = QMessageBox::question( this, tr("Save project file"),
                                                                     tr("Do you want to save current project?"),
                                                                     QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                     QMessageBox::Yes);
        if (res_btn == QMessageBox::Yes)
        {
            slot_saveProject();
        }
        if(res_btn == QMessageBox::Cancel)
            return;
    }

    // delete all data
    deleteTreeItemsData(root);
    TdmLayersModel *model = TdmLayersModel::instance();
    if(model->hasChildren())
    {
        model->removeRows(0, model->rowCount());
    }

    // disallow measurement to be loaded
    //ui->open_measurement_file_action->setEnabled(false);
    ui->import_old_measurement_format_action->setEnabled(false);

    // disallow measurement tools
    ui->line_tool->setEnabled(false);
    ui->surface_tool->setEnabled(false);
    ui->pick_point->setEnabled(false);

    // ask file name
    QString project_filename = getOpenFileName(this,tr("Select project to open"),m_path_project, tr("3DMetrics project (*.tdm)"));

    // save pathProject
    m_path_project = project_filename;
    slot_applySettings();

    if(project_filename.length() > 0)
    {
        // open project
        QFile project_file(project_filename);
        if(!project_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::critical(this, tr("Error : project file"), tr("Error : reading file"));
            return;
        }
        QByteArray ba = project_file.readAll();
        project_file.close();

        QJsonDocument doc = QJsonDocument::fromJson(ba);

        QApplication::setOverrideCursor(Qt::WaitCursor);
        // create project structure
        m_project_filename = project_filename;

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
        QString filename = _obj["File"].toString();
        QFileInfo project_path(m_project_filename);
        QDir dir(project_path.absoluteDir());
        QString file_path = dir.absoluteFilePath(filename);

        // load 3D model
        FileOpenThread *thread_node = new FileOpenThread();
        connect(thread_node,SIGNAL(signal_createNode(osg::Node*,QString,TdmLayerItem*,bool)), this, SLOT(slot_load3DModel(osg::Node*,QString,TdmLayerItem*,bool)));

        thread_node->setFileName(file_path);
        thread_node->setTDMLayerItem(_parent ? _parent : root);
        thread_node->setSelectItem(false);
        thread_node->setOSGWidget(ui->display_widget);
        thread_node->start();

        // allow measurement to be loaded
        //ui->open_measurement_file_action->setEnabled(true);
        ui->import_old_measurement_format_action->setEnabled(true);

        // measurement tools
        ui->line_tool->setEnabled(true);
        ui->surface_tool->setEnabled(true);
        ui->pick_point->setEnabled(true);
    }

    if(_obj.contains("Measurement"))
    {
        QString filename = _obj["File"].toString();
        QFileInfo project_path(m_project_filename);
        QDir dir(project_path.absoluteDir());
        QString file_path = dir.absoluteFilePath(filename);

        // loadMeasurement
        loadMeasurementFromFile(file_path, _parent ? _parent : root, false);
    }
}

bool TDMGui::checkAndSaveMeasurements(TdmLayerItem *_item)
{
    if(_item == nullptr)
        return false;

    TdmLayersModel *model = TdmLayersModel::instance();

    if(_item->type() == TdmLayerItem::MeasurementLayer)
    {
        if(_item->hasData<TDMMeasurementLayerData>())
        {
            QModelIndex index = model->index(_item);
            selectItem(index);
            if(_item->getFileName().isEmpty())
            {
                slot_saveMeasurementFileAs();

                return !_item->getFileName().isEmpty();
            }
            else
            {
                TDMMeasurementLayerData layer_data = _item->getPrivateData<TDMMeasurementLayerData>();
                return saveMeasurementToFile(layer_data.fileName(),layer_data);
            }
        }
    }

    bool res = true;
    if(_item->type() == TdmLayerItem::GroupLayer)
    {
        for(int i=0; i<_item->childCount(); i++)
        {
            bool reschildren = checkAndSaveMeasurements(_item->child(i));
            res = res && reschildren;
        }
    }

    return res;
}

QJsonObject TDMGui::saveTreeStructure(TdmLayerItem *_item)
{
    QJsonObject obj;
    if(_item->type() == TdmLayerItem::GroupLayer)
    {
        QJsonArray array;

        for(int i=0; i<_item->childCount(); i++)
        {
            array.append(saveTreeStructure(_item->child(i)));
        }
        obj.insert("Group",_item->getName());
        obj.insert("Children",array);
    }
    else
    {
        //save relative file name
        QFileInfo file_info(m_project_filename);
        QDir dir = file_info.absoluteDir();
        QString rel_filename = dir.relativeFilePath(_item->getFileName());

        if(_item->type() == TdmLayerItem::MeasurementLayer)
        {
            obj.insert("Measurement", _item->getName());
            obj.insert("File", rel_filename);
        }
        if(_item->type() == TdmLayerItem::ModelLayer)
        {
            obj.insert("Model3D", _item->getName());
            obj.insert("File", rel_filename);
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
        QMessageBox::StandardButton res_btn = QMessageBox::question( this, tr("Save project file"),
                                                                     tr("Saving f_nodemeasurement is mandatory before\nProceed?"),
                                                                     QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                     QMessageBox::Yes);
        if (res_btn != QMessageBox::Yes)
        {
            QMessageBox::information(this,tr("Save project file"),tr("Project not saved"));
            return;
        }
    }

    // check all measurement have filenames
    // ask to save missing
    // save all measurements
    bool status = checkAndSaveMeasurements(TdmLayersModel::instance()->rootItem());
    if(!status)
    {
        QMessageBox::information(this, tr("Save project file"), tr("Not all measurements are saved"));
        return;
    }

    // save in file
    QString project_filename = getSaveFileName(this, tr("Save project"), "",
                                               "*.tdm");
    QFileInfo project_file_info(project_filename);

    // check filename is not empty
    if(project_file_info.fileName().isEmpty()){
        QMessageBox::critical(this, tr("Error : save project"), tr("Error : you didn't give a name to the file"));
        return;
    }

    // add suffix if needed
    if (project_file_info.suffix() != "tdm"){
        project_filename += ".tdm";
        project_file_info.setFile(project_filename);
    }

    // save project structure
    QFile project_file(project_filename);
    if(!project_file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, tr("Error : save measurement file"), tr("Error : cannot open file for saving, check path writing rights"));
        return;
    }

    m_project_filename = project_file_info.absoluteFilePath();

    // build json object
    QJsonDocument json;
    QJsonObject obj = saveTreeStructure(TdmLayersModel::instance()->rootItem());
    json.setObject(obj);

    // write
    QString json_string = json.toJson();
    project_file.write(json_string.toUtf8());
    project_file.close();
}

void TDMGui::slot_layersTreeWindow()
{
    if(ui->layers_tree_window_action->isChecked())
    {
        ui->tree_widget_dock->show();
    }
    else
    {
        ui->tree_widget_dock->hide();
    }
}

void TDMGui::slot_attribTableWindow()
{
    if(ui->attrib_table_window_action->isChecked())
    {
        ui->attrib_table_dock->show();
    }
    else
    {
        ui->attrib_table_dock->hide();
    }
}

void TDMGui::slot_layersTreeWindowVisibilityChanged(bool value)
{
    ui->layers_tree_window_action->setChecked(value);
}

void TDMGui::slot_attribTableWindowVisibilityChanged(bool value)
{
    ui->attrib_table_window_action->setChecked(value);
}

void TDMGui::slot_about()
{
    m_dialog.show();
}

void TDMGui::slot_mouseClickInOsgWidget(Qt::MouseButton _button, int _x, int _y)
{
    // clic
    bool exists = false;
    osg::Vec3d vect;
    ui->display_widget->getIntersectionPoint(_x, _y, vect, exists);
    if(exists)
    {
        double lat, lon, depth;

        // transform to lat/lon
        QPointF ref_lat_lon; double ref_depth;
        ui->display_widget->getGeoOrigin(ref_lat_lon, ref_depth);
        if(ref_depth == INVALID_VALUE)
        {
            m_lat_label->setText("");
            m_lon_label->setText("");
            m_depth_label->setText("");
            return;
        }
        ui->display_widget->xyzToLatLonDepth(vect[0], vect[1], vect[2], lat, lon, depth);

        m_lat_label->setText("lat: "+QString::number(fabs(lat),'f',7) + (lat >= 0 ? "N" : "S"));
        m_lon_label->setText("lon: "+QString::number(fabs(lon),'f',7) + (lon >= 0 ? "E" : "W"));
        m_depth_label->setText("depth: "+QString::number(depth,'f',1) + "m");
    }
    else
    {
        m_lat_label->setText("");
        m_lon_label->setText("");
        m_depth_label->setText("");
    }
}

void TDMGui::slot_showDecimationDialog()
{
    m_decimation_dialog.show();
}

void TDMGui::slot_decimateSelectedModel()
{
    QString file_to_be_decimated = m_decimation_dialog.getModelPath();

    if (!file_to_be_decimated.isEmpty())
    {
        QFileInfo model_file(file_to_be_decimated);

        double decimation_factor = m_decimation_dialog.getDecimationFactor();

        QProcess decimation_process;
        decimation_process.setWorkingDirectory(model_file.absoluteDir().absolutePath());

        QString command_line=QString("osgconv.exe -O noRotation --simplify %1 %2 %3")
                .arg(decimation_factor,3,'f',3)
                .arg(model_file.fileName())
                .arg(model_file.baseName()+"_dec"+QString::number((int)(decimation_factor*100))+"."+model_file.completeSuffix());

        decimation_process.start(command_line);

        QProgressDialog decimation_progress(QString("Decimating model"), "Abort decimation", 0, 100, this);
        decimation_progress.setWindowModality(Qt::WindowModal);

        // Monitor process
        int k=0,l=0;
        while(decimation_process.waitForReadyRead(-1)){

            QString output = decimation_process.readAllStandardOutput() + decimation_process.readAllStandardError();

            if (decimation_progress.wasCanceled())
                decimation_process.kill();

            if (k%200==0){
                decimation_progress.setValue(l%99);
                l++;
            }

            QApplication::processEvents();
            k++;
            qDebug() << output;
        }

        decimation_progress.setValue(100);

        QMessageBox::information(this,"Success","Decimation of the model ended");

    }

}

void TDMGui::slot_saveSnapshot()
{
    // retrieve the generateOrthoMap
    QImage image_capture = ui->display_widget->grabFramebuffer();


    QString snapshot_name = getSaveFileName(this, tr("Save snapshot"), m_path_snapshot,
                                            tr("Images (*.png)"));

    m_path_snapshot = snapshot_name;
    slot_applySettings();

    QFileInfo snapshot_file_info(snapshot_name);

    // check filename is not empty
    if(snapshot_file_info.fileName().isEmpty()){
        QMessageBox::critical(this, tr("Error : save snapshot"), tr("Error : you didn't give a name to the file"));
        return;
    }

    // add suffix if needed
    if (snapshot_file_info.suffix() != "png"){
        snapshot_name += ".png";
        snapshot_file_info.setFile(snapshot_name);
    }

    // save the capture
    bool save_check = image_capture.save(snapshot_name,"png");

    // check if the image has been saved
    if(save_check)
    {
        QMessageBox::information(this,"Success","Your capture is saved");
    }
    else
    {
        QMessageBox::critical(this, tr("Error : save snapshot"), tr("Error : your capture is not saved"));

    }
}

void TDMGui::slot_applySettings()
{
    m_settings.setValue("3DMetrics/path3DModel", m_path_model3D);
    m_settings.setValue("3DMetrics/pathMeasurement", m_path_measurement);
    m_settings.setValue("3DMetrics/pathProject", m_path_project);
    m_settings.setValue("3DMetrics/pathSnapshot", m_path_snapshot);
    m_settings.setValue("3DMetrics/pathOrthoMap", m_path_ortho_map);
    m_settings.setValue("3DMetrics/pathDepthMap", m_path_depth_map);

}

void TDMGui::slot_axeView()
{
    if(ui->add_axes_action->isChecked())
    {
        //scale
        bool ok;
        double scale = QInputDialog::getDouble(this,tr("Scale") , tr("Enter the scale size in meter ?"), 0.0, 0, 99999,4, &ok);
        if( !ok ) return;
        QMessageBox::information(this,tr("Information"), tr("Double click where you want to put your axe"));
        m_axe.setScale(scale);
        m_axe.start();
        m_axe.show();
    }
    else
    {
        m_axe.removeAxe();
    }
}

void TDMGui::slot_computeTotalArea()
{

    QTreeView *view = ui->tree_widget;

    bool has_selection = !view->selectionModel()->selection().isEmpty();
    bool has_current = view->selectionModel()->currentIndex().isValid();

    if (has_selection && has_current)
    {
        // get the 3D model selected
        QModelIndex index = view->selectionModel()->currentIndex();
        QAbstractItemModel *model = view->model();
        TdmLayerItem *item = (static_cast<TdmLayersModel*>(model))->getLayerItem(index);
        TDMModelLayerData layer_data = item->getPrivateData<TDMModelLayerData>();

        osg::Node* const node = (layer_data.node().get());

        // compute the surface of the 3D model selected through his node
        AreaComputationVisitor total_area;
        node->accept(total_area);
        double total_area_double = total_area.getArea();
        QString total_area_string = QString::number(total_area_double,'f',2);
        QStringList filename_split = layer_data.fileName().split("/");
        QString name3D_mode = filename_split.at(filename_split.length()-1);
        QMessageBox::information(this,tr("total surface area"), tr("The total surface area of ")+ name3D_mode+tr(" is ")+total_area_string + " m²");
    }
}

void TDMGui::slot_saveOrthoMap()
{
    QTreeView *view = ui->tree_widget;

    bool has_selection = !view->selectionModel()->selection().isEmpty();
    bool has_current = view->selectionModel()->currentIndex().isValid();

    QString name_file_orhto2D = getSaveFileName(this, tr("Save orthographic map"), m_path_ortho_map,
                                                tr("Images (*.tif)"));

    m_path_ortho_map = name_file_orhto2D;
    slot_applySettings();

    QFileInfo ortho2D_file_info(name_file_orhto2D);

    // check filename is not empty
    if(ortho2D_file_info.fileName().isEmpty()){
        QMessageBox::critical(this, tr("Error : save orthographic map"), tr("Error : you didn't give a name to the file"));
        return;
    }

    // add suffix if needed
    if (ortho2D_file_info.suffix() != ".tif"){
        name_file_orhto2D = name_file_orhto2D.remove(".tif");
        ortho2D_file_info.setFile(name_file_orhto2D);
    }

    if (has_selection && has_current)
    {
        // get the 3D model selected
        QModelIndex index = view->selectionModel()->currentIndex();
        QAbstractItemModel *model = view->model();
        TdmLayerItem *item = (static_cast<TdmLayersModel*>(model))->getLayerItem(index);
        TDMModelLayerData layer_data = item->getPrivateData<TDMModelLayerData>();

        osg::Node* const node = (layer_data.node().get());


        // SCREEN

        // Collect the number of pixel that the user want
        bool ok;
        double pixels = QInputDialog::getDouble(this,tr("Pixels") , tr("Enter the pixel size in meter ?"), 0.1 , 0, 99999,4, &ok);
        if( !ok ) return;

        bool save_image = ui->display_widget->generateGeoTiff(node,name_file_orhto2D,pixels,OSGWidget::OrthoMap);
        if (save_image) QMessageBox::information(this,"Done","Your orthographic image have been generated");
        else
        {
            QMessageBox::critical(this, tr("Error : depth map file"), tr("Error : your depth image couldn't be generated"));
            return;
        }
    }
}

void TDMGui::slot_saveDepthMap()
{
    QTreeView *view = ui->tree_widget;

    bool has_selection = !view->selectionModel()->selection().isEmpty();
    bool has_current = view->selectionModel()->currentIndex().isValid();

    QString name_file_depth = getSaveFileName(this, tr("Save depth map"),m_path_depth_map,
                                              tr("Images (*.tif)"));
    m_path_depth_map = name_file_depth;
    slot_applySettings();

    QFileInfo depth_file_info(name_file_depth);

    // check filename is not empty
    if(depth_file_info.fileName().isEmpty()){
        QMessageBox::critical(this, tr("Error : save depth map"), tr("Error : you didn't give a name to the file"));
        return;
    }

    // add suffix if needed
    if (depth_file_info.suffix() != ".tif"){
        name_file_depth = name_file_depth.remove(".tif");
        depth_file_info.setFile(name_file_depth);
    }

    if (has_selection && has_current)
    {
        // get the 3D model selected
        QModelIndex index = view->selectionModel()->currentIndex();
        QAbstractItemModel *model = view->model();
        TdmLayerItem *item = (static_cast<TdmLayersModel*>(model))->getLayerItem(index);
        TDMModelLayerData layer_data = item->getPrivateData<TDMModelLayerData>();

        osg::Node* const node = (layer_data.node().get());

        // SCREEN

        // Collect the number of pixel that the user want
        bool ok;
        double pixels = QInputDialog::getDouble(this,tr("Pixels") , tr("Enter the pixel size in meter ?"), 0.1, 0, 99999,4, &ok);
        if( !ok ) return;

        bool save_image = ui->display_widget->generateGeoTiff(node,name_file_depth,pixels, OSGWidget::DepthMap);
        if (save_image) QMessageBox::information(this,"Done","Your depth image have been generated");
        else
        {
            QMessageBox::critical(this, tr("Error : depth map file"), tr("Error : your depth image couldn't be generated"));
            return;
        }
    }
}

void TDMGui::slot_help()
{
    QString userManualFileName = "help/3DMetricsStartGuide.pdf";

    QFileInfo userManualFile(userManualFileName);

    if (!userManualFile.exists()) {
        QMessageBox::warning(this, tr("User manual"), tr("User manual file '%1' does not exist")
                             .arg(userManualFile.absoluteFilePath()));
        return;
    }

    QUrl url = QUrl::fromLocalFile(userManualFile.absoluteFilePath());
    QDesktopServices::openUrl(url);
}

void TDMGui::slot_addLine()
{
    QTreeView *view = ui->tree_widget;

    bool has_current_tree = view->selectionModel()->currentIndex().isValid();
    bool has_selection_tree = !view->selectionModel()->selection().isEmpty();
    view->closePersistentEditor(view->selectionModel()->currentIndex());

    if(has_selection_tree && has_current_tree)
    {
        TdmLayerItem *selected = TdmLayersModel::instance()->getLayerItem(
                    view->selectionModel()->currentIndex());
        if(selected != nullptr)
        {
            if(selected->type() == TdmLayerItem::MeasurementLayer)
            {

                TDMMeasurementLayerData layer_data = selected->getPrivateData<TDMMeasurementLayerData>();
                if(layer_data.pattern().getNbFields() !=0 )  slot_addAttributeLine();
            }
        }
    }
}

void TDMGui::slot_stereoShortcut()
{
    if(ui->stereo_action->isChecked())
    {
        ui->stereo_action->setChecked(false);
        slot_toggleStereoView();
    }
    else
    {
        ui->stereo_action->setChecked(true);
        slot_toggleStereoView();
    }
}

void TDMGui::slot_toggleStereoView()
{
    if(ui->stereo_action->isChecked())
    {
        ui->display_widget->enableStereo(true);
    }
    else
    {
        ui->display_widget->enableStereo(false);
    }
}

void TDMGui::slot_lightShorcut()
{
    if(ui->light_action->isChecked())
    {
        ui->light_action->setChecked(false);
        slot_toggleLight();
    }
    else
    {
        ui->light_action->setChecked(true);
        slot_toggleLight();
    }
}

void TDMGui::slot_toggleLight()
{
    if(ui->light_action->isChecked())
    {
        ui->display_widget->enableLight(false);
    }
    else
    {
        ui->display_widget->enableLight(true);
    }
}

void TDMGui::slot_showExportMeasToGeom()
{
    m_meas_geom_export_dialog.show();
}

void TDMGui::slot_exportMeasToGeom()
{
    QString fileName = m_meas_geom_export_dialog.getFilename();
    QFile path(fileName);
    QFileInfo path_info(path.fileName());
    if( m_meas_geom_export_dialog.getExportType() == MeasGeomExportDialog::export_type::ASCII)
    {
        // add suffix if needed
        if (path_info.suffix() != "csv")
        {
            fileName += ".csv";
            path_info.setFile(fileName);
        }

        QFile csv_file(fileName);
        if(!csv_file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this, tr("Error : save measurement to csv"), tr("Error : cannot open the file"));
            return;
        }
        if( m_meas_geom_export_dialog.getLatLonSelected() == true)
        {
            // Get Table
            QTableWidget *table = ui->attrib_table;

            //get and write header
            /*for(int i=1; i<table->columnCount(); i++)
            {
                QString field_string = table->horizontalHeaderItem(i)->text();
                // write field
                if (i<table->columnCount()-1)
                    field_string = field_string + ",";

                csv_file.write(field_string.toUtf8());
            }*/
            // write end of line
            csv_file.write(QString("\n").toUtf8());

            // write fields data
            for(int i=0; i < table->rowCount(); i++)
            {
                for(int j=1; j < table->columnCount(); j++)
                {
                    // add field
                    MeasTableWidgetItem *pwidget = (MeasTableWidgetItem *)table->item(i,j);
                    MeasItem *item = pwidget->measItem();
                    QString field_string;
                    item->encodeASCIILatLon(field_string);

                    if( item->type() == "Point" && m_meas_geom_export_dialog.getPointSelected() == true )
                    {
                        // write field
                        if (j<table->columnCount()-1)
                            field_string = field_string + ",";

                        csv_file.write(field_string.toUtf8());
                    }
                    if( item->type() == "Line" && m_meas_geom_export_dialog.getLineSelected() == true )
                    {
                        // write field
                        if (j<table->columnCount()-1)
                            field_string = field_string + ",";

                        csv_file.write(field_string.toUtf8());
                    }
                    if( item->type() == "Area" && m_meas_geom_export_dialog.getAreaSelected() == true )
                    {
                        // write field
                        if (j<table->columnCount()-1)
                            field_string = field_string + ",";

                        csv_file.write(field_string.toUtf8());
                    }
                }
                // write end of line
                csv_file.write(QString("\n").toUtf8());
            }
            if( m_meas_geom_export_dialog.getXYZSelected() == true)
            {
                // Get Table
                QTableWidget *table = ui->attrib_table;

                //get and write header
                /*for(int i=1; i<table->columnCount(); i++)
                {
                    QString field_string = table->horizontalHeaderItem(i)->text();
                    // write field
                    if (i<table->columnCount()-1)
                        field_string = field_string + ",";

                    csv_file.write(field_string.toUtf8());
                }*/
                // write end of line
                csv_file.write(QString("\n").toUtf8());

                // write fields data
                for(int i=0; i < table->rowCount(); i++)
                {
                    for(int j=1; j < table->columnCount(); j++)
                    {
                        // add field
                        MeasTableWidgetItem *pwidget = (MeasTableWidgetItem *)table->item(i,j);
                        MeasItem *item = pwidget->measItem();
                        QString field_string;
                        item->encodeASCII(field_string);

                        if( item->type() == "Point" && m_meas_geom_export_dialog.getPointSelected() == true )
                        {
                            // write field
                            if (j<table->columnCount()-1)
                                field_string = field_string + ",";

                            csv_file.write(field_string.toUtf8());
                        }
                        if( item->type() == "Line" && m_meas_geom_export_dialog.getLineSelected() == true )
                        {
                            // write field
                            if (j<table->columnCount()-1)
                                field_string = field_string + ",";

                            csv_file.write(field_string.toUtf8());
                        }
                        if( item->type() == "Area" && m_meas_geom_export_dialog.getAreaSelected() == true )
                        {
                            // write field
                            if (j<table->columnCount()-1)
                                field_string = field_string + ",";

                            csv_file.write(field_string.toUtf8());
                        }
                    }
                    // write end of line
                    csv_file.write(QString("\n").toUtf8());
                }
            }
        }

        // close file
        csv_file.close();

    }
    if( m_meas_geom_export_dialog.getExportType() == MeasGeomExportDialog::export_type::ShapeFile)
    {
        QTableWidget *table = ui->attrib_table;
        const char *psz_driver_name = "ESRI Shapefile";
        OGRSFDriver *driver;
        OGRRegisterAll();
        driver =OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(psz_driver_name);
        OGRDataSource *data_source;
        data_source = driver->CreateDataSource( path_info.absolutePath().toStdString().c_str(), NULL );

        if( data_source == NULL )
        {
            printf( "Creation of output file failed.\n" );
        }

        OGRSpatialReference oSRS;
        QPointF lat_lon;
        double depth_org;
        ui->display_widget->getGeoOrigin(lat_lon, depth_org);
        oSRS.SetTM(lat_lon.x(),lat_lon.y(),0.9996,0,0);
        oSRS.SetWellKnownGeogCS( "WGS84" );

        bool point_created = false;
        bool line_created = false;
        bool polygon_created = false;

        OGRFieldDefn oField( "Points", OFTString );
        oField.SetWidth(32);

        OGRLayer *po_layer_points;
        OGRLayer *po_layer_lines;
        OGRLayer *po_layer_area;
        for(int i=0; i<table->rowCount(); i++)
        {
            for(int j=1; j<table->columnCount(); j++)
            {
                MeasTableWidgetItem *widget = (MeasTableWidgetItem *)table->item(i,j);
                MeasItem *item = widget->measItem();

                if( point_created == false && item->type() == "Point" && m_meas_geom_export_dialog.getPointSelected() == true )
                {
                    QString fileName_points = path_info.fileName()+"_point";
                    po_layer_points = data_source->CreateLayer( fileName_points.toStdString().c_str() , &oSRS, wkbPoint, NULL );

                    if( po_layer_points->CreateField( &oField ) != OGRERR_NONE )
                    {
                        printf( "Creating Name field failed.\n" );
                    }
                    point_created = true;
                }
                if( line_created == false && item->type() == "Line" && m_meas_geom_export_dialog.getLineSelected() == true )
                {
                    QString fileName_lines = path_info.fileName()+"_line";
                    po_layer_lines = data_source->CreateLayer( fileName_lines.toStdString().c_str(), &oSRS, wkbLineString, NULL );

                    if( po_layer_lines->CreateField( &oField ) != OGRERR_NONE )
                    {
                        printf( "Creating Name field failed.\n" );
                    }
                    line_created = true;
                }
                if( polygon_created == false && item->type() == "Area" && m_meas_geom_export_dialog.getAreaSelected() == true )
                {
                    QString fileName_polygons = path_info.fileName()+"_polygon";
                    po_layer_area = data_source->CreateLayer( fileName_polygons.toStdString().c_str() , &oSRS, wkbPolygon, NULL );

                    if( po_layer_area->CreateField( &oField ) != OGRERR_NONE )
                    {
                        printf( "Creating Name field failed.\n" );
                    }
                    polygon_created = true;
                }
            }
        }

        char name[33] ;
        for(int i=0; i<table->rowCount(); i++)
        {
            for(int j=1; j<table->columnCount(); j++)
            {
                MeasTableWidgetItem *widget = (MeasTableWidgetItem *)table->item(i,j);
                MeasItem *item = widget->measItem();

                QString point;
                item->encodeShapefile(point);

                if( item->type() == "Point" && point_created == true )
                {
                    QStringList point_split = point.split("/");
                    OGRFeature *feature;
                    feature = OGRFeature::CreateFeature( po_layer_points->GetLayerDefn() );
                    feature->SetField( "Point", name );
                    OGRPoint point;
                    point.setX( point_split.at(0).toDouble() );
                    point.setY( point_split.at(1).toDouble() );
                    feature->SetGeometry( &point );
                    if( po_layer_points->CreateFeature( feature ) != OGRERR_NONE )
                    {
                        printf( "Failed to create feature in shapefile.\n" );
                    }
                    OGRFeature::DestroyFeature( feature );
                }
                if( item->type() == "Line" && line_created == true )
                {
                    QStringList point_split = point.split("/");
                    OGRFeature *feature;
                    feature = OGRFeature::CreateFeature( po_layer_lines->GetLayerDefn() );
                    feature->SetField( "Line", name );
                    OGRLineString line_OGR ;
                    for( int k = 0; k < point_split.size()-1; k = k+2 )
                    {
                        line_OGR.addPoint(point_split.at(k).toDouble(),point_split.at(k+1).toDouble());
                    }
                    feature->SetGeometry( &line_OGR );
                    if( po_layer_lines->CreateFeature( feature ) != OGRERR_NONE )
                    {
                        printf( "Failed to create feature in shapefile.\n" );
                    }
                    OGRFeature::DestroyFeature( feature );
                }
                if( item->type() == "Area" && polygon_created == true )
                {
                    QStringList point_split = point.split("/");
                    OGRFeature *feature;
                    feature = OGRFeature::CreateFeature( po_layer_area->GetLayerDefn() );
                    feature->SetField( "Polygon", name );
                    OGRPolygon polygon_OGR;
                    OGRLinearRing line_point ;
                    for( int k = 0; k < point_split.size()-1; k = k+2 )
                    {
                        line_point.addPoint(point_split.at(k).toDouble(),point_split.at(k+1).toDouble());
                    }
                    polygon_OGR.addRing(&line_point);
                    feature->SetGeometry( &polygon_OGR );
                    if( po_layer_area->CreateFeature( feature ) != OGRERR_NONE )
                    {
                        printf( "Failed to create feature in shapefile.\n" );
                    }
                    OGRFeature::DestroyFeature( feature );
                }
            }
        }
        OGRDataSource::DestroyDataSource( data_source );
    }
}

void TDMGui::slot_editTransparency()
{
    QObject::connect(&m_edit_trans_model, SIGNAL(signal_onChangedTransparencyValue(int)), this, SLOT(slot_Transparency(int)));
    m_edit_trans_model.show();

}

void TDMGui::slot_Transparency(int _transparency_value_transparency_value)
{
    QTreeView *view = ui->tree_widget;

    bool has_selection = !view->selectionModel()->selection().isEmpty();
    bool has_current = view->selectionModel()->currentIndex().isValid();

    if (has_selection && has_current)
    {
        // get the 3D model selected
        QModelIndex index = view->selectionModel()->currentIndex();
        QAbstractItemModel *model = view->model();
        TdmLayerItem *item = (static_cast<TdmLayersModel*>(model))->getLayerItem(index);
        TDMModelLayerData layer_data = item->getPrivateData<TDMModelLayerData>();

        osg::Node* const node = (layer_data.node().get());
        ui->display_widget->slot_onTransparencyChange(_transparency_value_transparency_value, node);
    }
}
