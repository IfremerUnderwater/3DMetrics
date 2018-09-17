#ifndef TDMGUI_H
#define TDMGUI_H

#include <QMainWindow>

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


    // from TreeView
    void slot_selectionChanged();
    void slot_checkChanged(TdmLayerItem*);
    void slot_itemDropped(TdmLayerItem*);
    void slot_contextMenu(const QPoint &);

    // TreeView context menu
    void slot_deleteRow();
    void slot_newGroup();
    void slot_moveToToplevel();
    void slot_unselect();
};

#endif // TDMGUI_H
