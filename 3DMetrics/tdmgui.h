#ifndef TDMGUI_H
#define TDMGUI_H

#include <QMainWindow>

#include "Measure/measurepattern.h"

class TdmLayerItem;
class QCloseEvent;
class QItemSelection;
class TDMMeasureLayerData;

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
    TDMMeasureLayerData *m_currentItem;

public slots:

    void slot_open3dModel();
    void slot_openMeasureFile();
    void slot_saveMeasureFile();

    // general tools
    void slot_focussingTool();

    // measurement tools
    void slot_messageStartTool(QString&_msg);
    void slot_messageCancelTool(QString&_msg);
    void slot_messageEndTool(QString&_msg);

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

    void slot_displayToplevelChanged(bool);
};

#endif // TDMGUI_H
