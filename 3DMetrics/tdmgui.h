#ifndef TDMGUI_H
#define TDMGUI_H

#include <QMainWindow>

#include "Measurement/measurement_pattern.h"

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
    void deleteTreeItemsData(TdmLayerItem *item);
    void manageCheckStateForChildren(TdmLayerItem *item, bool checked);

    // helper functions
    void loadData(QJsonDocument &_doc, bool _buildOsg);
    void saveData(QJsonDocument &_doc);

    void selectItem(QModelIndex &index);

    // attribute table
    void updateAttributeTable(TdmLayerItem *item);
    MeasurePattern m_current;
    // for current vector of rows
    TDMMeasurementLayerData *m_currentItem;

    // working helpers
    void load3DModel(QString _filename, TdmLayerItem *_parent, bool _selectItem);
    bool loadMeasure(QString _filename, TdmLayerItem *_parent, bool _selectItem);
    bool saveMeasure(QString _filename, TDMMeasurementLayerData &_data);

    bool checkAndSaveMeasures(TdmLayerItem *item);
    QJsonObject saveTreeStructure(TdmLayerItem *item);
    void buildProjectTree(QJsonObject _obj, TdmLayerItem *_parent);

    QString m_projectFileName;

public slots:

    void slot_open3dModel();

    void slot_openMeasureFile();
    void slot_saveMeasureFile();
    void slot_saveMeasureFileAs();

    void slot_openProject();
    void slot_saveProject();

    void slot_layersTreeWindow();
    void slot_attribTableWindow();

    void slot_layersTreeWindowVisibilityChanged(bool);
    void slot_attribTableWindowVisibilityChanged(bool);

    void slot_importOldMeasureFile();

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

};

#endif // TDMGUI_H
