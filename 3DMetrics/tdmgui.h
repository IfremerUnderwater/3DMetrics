#ifndef TDMGUI_H
#define TDMGUI_H

#include <QMainWindow>

#include "Measure/measurepattern.h"

class TdmLayerItem;
class QCloseEvent;

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

    // attribute table
    void updateAttributeTable(TdmLayerItem *item);
    MeasurePattern m_current;

public slots:

    void slot_open3dModel();
    void slot_openMeasureFile();
    void slot_saveMeasureFile();

    // general tools
    void slot_focussingTool();

    // measurement tools

    // measurment pattern dialog
    void slot_patternChanged(MeasurePattern pattern);

    // from TreeView
    void slot_selectionChanged();
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
    //
    void slot_addAttributeLine();
    void slot_deleteAttributeLine();

};

#endif // TDMGUI_H
