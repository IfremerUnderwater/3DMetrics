#ifndef CHOOSE_LOADINGMODE_DIALOG_H
#define CHOOSE_LOADINGMODE_DIALOG_H

#include <QDialog>
#include "OSGWidget/loading_mode.h"

namespace Ui {
class ChooseLoadingModeDialog;
}

class ChooseLoadingModeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseLoadingModeDialog(QWidget *parent = 0);
    ~ChooseLoadingModeDialog();

    LoadingMode mode() const { return m_mode; }
    void setMode(LoadingMode _mode);

public slots:
    void radioToggled();
    void ok();

private:
    Ui::ChooseLoadingModeDialog *ui;
    LoadingMode m_mode;
};

#endif // CHOOSE_LOADINGMODE_DIALOG_H
