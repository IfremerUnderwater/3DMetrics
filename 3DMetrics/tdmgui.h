#ifndef TDMGUI_H
#define TDMGUI_H

#include <QMainWindow>
#include <QLabel>
#include <QSettings>
#include <QShortcut>


#include "Measurement/measurement_pattern.h"
#include "decimation_dialog.h"
#include "osg_axes.h"

class TdmLayerItem;
class QCloseEvent;
class QItemSelection;
class TDMMeasurementLayerData;

namespace Ui {
class TDMGui;
}

class TDMGui : public QMainWindow
{
    Q_OBJECT

public:
    explicit TDMGui(QWidget *parent = 0);
    ~TDMGui();

    // ask on close
    void closeEvent(QCloseEvent *event);

private:
    Ui::TDMGui *ui;

    // Treeview
    void deleteTreeItemsData(TdmLayerItem *_item);
    void manageCheckStateForChildren(TdmLayerItem *_item, bool _checked);

    // helper functions
    void loadAttribTableFromJson(QJsonDocument &_doc, bool _buildOsg);
    void saveAttribTableToJson(QJsonDocument &_doc);

    void selectItem(QModelIndex &_index);

    // attribute table
    void updateAttributeTable(TdmLayerItem *_item);
    MeasurePattern m_current;
    // for current vector of rows
    TDMMeasurementLayerData *m_currentItem;

    // working helpers
    void load3DModel(QString _filename, TdmLayerItem *_parent, bool _selectItem);
    bool loadMeasurementFromFile(QString _filename, TdmLayerItem *_parent, bool _selectItem);
    bool saveMeasurementToFile(QString _filename, TDMMeasurementLayerData &_data);

    bool checkAndSaveMeasures(TdmLayerItem *_item);
    QJsonObject saveTreeStructure(TdmLayerItem *_item);
    void buildProjectTree(QJsonObject _obj, TdmLayerItem *_parent);

    QString m_projectFileName;

    QLabel *m_latLabel;
    QLabel *m_lonLabel;
    QLabel *m_depthLabel;

    DecimationDialog m_decimation_dialog;

    // Settings variable
    QSettings m_settings;
    QString m_pathModel3D;
    QString m_pathMeasurement;
    QString m_pathProject;

    QShortcut *m_ctrlZ;

    OSGAxes m_axe;
public slots:

    void slot_open3dModel();

    void slot_openMeasureFile();
    void slot_saveMeasureFile();
    void slot_saveMeasureFileAs();

    void slot_saveAttribTableToASCII();

    void slot_openProject();
    void slot_saveProject();

    void slot_layersTreeWindow();
    void slot_attribTableWindow();

    void slot_layersTreeWindowVisibilityChanged(bool);
    void slot_attribTableWindowVisibilityChanged(bool);

    void slot_importOldMeasureFile();

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
    void slot_patternChanged(MeasurePattern _pattern);

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

    // Attributes Table widget
    void slot_attribTableContextMenu(const QPoint &);
    void slot_attribTableDoubleClick(int row, int column);
    void slot_attribTableCellChanged(int row, int column);

    // attribute table context menu
    void slot_addAttributeLine();
    void slot_deleteAttributeLine();

    // mouse move in osg widget
    void slot_mouseMoveInOsgWidget(int x, int y);

    // decimation dialog & action
    void slot_showDecimationDialog();
    void slot_decimateSelectedModel();

    // Snapshot
    void slot_saveSnapshot();

    // Settings
    void slot_applySettings();

    // Axes
    void slot_axeWindows();

private slots:

};

#endif // TDMGUI_H
