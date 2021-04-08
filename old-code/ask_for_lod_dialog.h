#ifndef ASK_FOR_LOD_DIALOG_H
#define ASK_FOR_LOD_DIALOG_H

#include <QDialog>

namespace Ui {
class AskForLODDialog;
}

class AskForLODDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AskForLODDialog(QWidget *parent = 0);
    ~AskForLODDialog();

    // setters
    void enableUseLOD(const bool _enable);

    void setUseLOD(const bool _use);
    void setBuildLOD(const bool _build);
    void setSaveCompLOD(const bool _save);

    // getters
    bool useLOD() const { return m_useLOD; }
    bool buildLOD() const {return m_setBuildLOD; }
    bool saveCompLOD() const { return m_saveCompLOD; }

public slots:
    void ok();
    void cancel();
    void saveCompChecked();

private:
    Ui::AskForLODDialog *ui;

    bool m_useLOD;
    bool m_setBuildLOD;
    bool m_saveCompLOD;
};

#endif // ASK_FOR_LOD_DIALOG_H
