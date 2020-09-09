#ifndef MODEL_LOADINGMODE_DIALOG_H
#define MODEL_LOADINGMODE_DIALOG_H

#include <QDialog>
#include "OSGWidget/loading_mode.h"

namespace Ui {
class ModelLoadingModeDialog;
}

class ModelLoadingModeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModelLoadingModeDialog(QWidget *parent = 0);
    ~ModelLoadingModeDialog();

    LoadingMode mode() const { return m_mode; }
    void setMode(LoadingMode _mode) { m_mode = _mode; }

    bool saveCompoundLOD() const { return m_saveCompoundLOD; }
    int nXTiles() const { return m_nXTiles; }
    int nYTiles() const { return m_nYTiles; }

public slots:
    void radioToggled();
    void ok();

private:
    Ui::ModelLoadingModeDialog *ui;

    LoadingMode m_mode;
    bool m_saveCompoundLOD;
    int m_nXTiles;
    int m_nYTiles;
};

#endif // MODEL_LOADINGMODE_DIALOG_H
