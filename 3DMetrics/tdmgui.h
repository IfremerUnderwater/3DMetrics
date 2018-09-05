#ifndef TDMGUI_H
#define TDMGUI_H

#include <QMainWindow>

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
};

#endif // TDMGUI_H
