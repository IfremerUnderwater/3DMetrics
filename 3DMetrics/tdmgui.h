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

public slots:
    void slot_open3dModel();
    void slot_openMeasureFile();
    void slot_newGroup();

    // from TreeView
    void slot_selectionChanged();
    void slot_checkChanged(TdmLayerItem*);
};

#endif // TDMGUI_H
