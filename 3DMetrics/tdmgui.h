#ifndef TDMGUI_H
#define TDMGUI_H

#include <QMainWindow>
#include <QLabel>
#include <QSettings>
#include <QShortcut>
#include <QMultiMap>
#include "about_dialog.h"

#include "Measurement/measurement_pattern.h"
#include "decimation_dialog.h"
#include "osg_axes.h"
#include "file_open_thread.h"
#include "meas_geom_export_dialog.h"
//#include "edit_transparency_model.h"

class TdmLayerItem;
class QCloseEvent;
class QItemSelection;
class TDMMeasurementLayerData;
class TDMModelLayerData;
class AreaComputationVisitor;
class BoxVisitor;

namespace Ui {
class TDMGui;
}

class TDMGui : public QMainWindow
{
    Q_OBJECT

public:
    explicit TDMGui(QWidget *_parent = 0);
    ~TDMGui();

    // ask on close
    void closeEvent(QCloseEvent *_event);

private:
    Ui::TDMGui *ui;
    AboutDialog m_dialog;

    // Map of opened transparency dialogs
    QMultiMap<TdmLayerItem*, QWidget*> toolWindowsMap;

    // Treeview
    void deleteTreeItemsData(TdmLayerItem *_item);
    void manageCheckStateForChildren(TdmLayerItem *_item, bool _checked);

    // helper functions
    void loadAttribTableFromJson(QJsonDocument &_doc, bool _build_osg);
    void saveAttribTableToJson(QJsonDocument &_doc);

    void selectItem(QModelIndex &_index);

    // attribute table
    void updateAttributeTable(TdmLayerItem *_item);
    MeasPattern m_current_pattern;
    // for current vector of rows
    TDMMeasurementLayerData *m_current_item;

    // working helpers
    bool loadMeasurementFromFile(QString _filename, TdmLayerItem *_parent, bool _select_item);
    bool saveMeasurementToFile(QString _filename, TDMMeasurementLayerData &_data);

    bool checkAndSaveMeasurements(TdmLayerItem *_item);
    QJsonObject saveTreeStructure(TdmLayerItem *_item);
    void buildProjectTree(QJsonObject _obj, TdmLayerItem *_parent);

    QString m_project_filename;

    QLabel *m_lat_label;
    QLabel *m_lon_label;
    QLabel *m_alt_label;

    DecimationDialog m_decimation_dialog;

    MeasGeomExportDialog m_meas_geom_export_dialog;

    // Settings variable
    QSettings m_settings;
    QString m_path_model3D;
    QString m_path_measurement;
    QString m_path_project;
    QString m_path_snapshot;
    QString m_path_ortho_map;
    QString m_path_alt_map;

    OSGAxes m_axe;

    // shortcuts
    QShortcut m_undo_shortcut;
    QShortcut m_help_shortcut;
    QShortcut m_addline_shortcut;
    QShortcut m_stereo_shortcut;
    QShortcut m_delete_shortcut;
    QShortcut m_light_shortcut;

public slots:

    void slot_open3dModel();
    void slot_load3DModel(osg::Node*, QString _filename,TdmLayerItem *_parent, bool _select_item);

    void slot_openMeasurementFile();
    void slot_saveMeasurementFile();
    void slot_saveMeasurementFileAs();

    // Export
    void slot_saveAttribTableToASCII();
    void slot_showExportMeasToGeom();
    void slot_exportMeasToGeom();

    void slot_openProject();
    void slot_saveProject();

    // Menu "View"
    void slot_layersTreeWindow();
    void slot_attribTableWindow();
    void slot_axeView();
    void slot_stereoShortcut();
    void slot_toggleStereoView();
    void slot_lightShorcut();
    void slot_toggleLight();

    void slot_layersTreeWindowVisibilityChanged(bool);
    void slot_attribTableWindowVisibilityChanged(bool);

    void slot_importOldMeasurementFile();

    void slot_about();

    // general tools
    void slot_focussingTool();

    // measurement tools
    void slot_messageStartTool(QString&_msg);
    void slot_messageCancelTool(QString&_msg);
    void slot_messageEndTool(QString&_msg);

    // temporary tools
    void slot_tempLineTool();
    void slot_tempPointTool();
    void slot_tempAreaTool();

    // measurment pattern dialog
    void slot_patternChanged(MeasPattern _pattern);

    // from TreeView
    void slot_selectionChanged(const QItemSelection &,
                               const QItemSelection &);
    void slot_checkChanged(TdmLayerItem*);
    void slot_itemDropped(TdmLayerItem*);
    void slot_treeViewContextMenu(const QPoint &);

    // TreeView context menu
    void slot_renameTreeItem();
    void slot_deleteRow();
    void slot_newGroup();
    void slot_newMeasurement();
    void slot_moveToToplevel();
    void slot_unselect();
    void slot_editMeasurement();
    void slot_computeTotalArea();
    void slot_editTransparency();
    void slot_editModelOffset();

    // Attributes Table widget
    void slot_attribTableContextMenu(const QPoint &);
    void slot_attribTableDoubleClick(int _row, int _column);
    void slot_attribTableCellChanged(int _row, int _column);

    // attribute table context menu
    void slot_addAttributeLine();
    void slot_deleteAttributeLine();

    // mouse move in osg widget
    void slot_mouseClickInOsgWidget(Qt::MouseButton _button, int _x, int _y);

    // decimation dialog & action
    void slot_showDecimationDialog();
    void slot_decimateSelectedModel();

    // Snapshot
    void slot_saveSnapshot();

    // generateOrthoMap 2D
    void slot_saveOrthoMap();

    // generateAltMap 2D
    void slot_saveAltMap();

    // Settings
    void slot_applySettings();

    // Keys event
    void slot_help();
    void slot_addLine();

private slots:

};

#endif // TDMGUI_H
