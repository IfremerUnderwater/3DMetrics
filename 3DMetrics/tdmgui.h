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

    void deleteTreeItemsData(TdmLayerItem *item);

    void manageCheckStateForChildren(TdmLayerItem *item, bool checked);

public slots:
    void slot_open3dModel();
    void slot_openMeasureFile();

    // general tools
    void slot_focussingTool();

    // measurement tools

    // measurment pattern dialog
    void slot_patternChanged(MeasurePattern pattern);

    // from TreeView
    void slot_selectionChanged();
    void slot_checkChanged(TdmLayerItem*);
    void slot_itemDropped(TdmLayerItem*);
    void slot_contextMenu(const QPoint &);

    // TreeView context menu
    void slot_renameTreeItem();
    void slot_deleteRow();
    void slot_newGroup();
    void slot_newMeasurement();
    void slot_moveToToplevel();
    void slot_unselect();
    void slot_editMeasurement();
};

#endif // TDMGUI_H
