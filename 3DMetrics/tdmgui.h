#ifndef TDMGUI_H
#define TDMGUI_H

#include <QMainWindow>

class TdmLayerItem;

namespace Ui {
class TDMGui;
}

class TDMGui : public QMainWindow
{
    Q_OBJECT

public:
    explicit TDMGui(QWidget *parent = 0);
    ~TDMGui();

private:
    Ui::TDMGui *ui;

    void deleteTreeItemsData(TdmLayerItem *item);

public slots:
    void slot_open3dModel();
    void slot_openMeasureFile();



    // from TreeView
    void slot_selectionChanged();
    void slot_checkChanged(TdmLayerItem*);
    void slot_contextMenu(const QPoint &);
    // TreeView context menu
    void slot_deleteRow();
    void slot_newGroup();
    void slot_moveToToplevel();
    void slot_unselect();
};

#endif // TDMGUI_H
