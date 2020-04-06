#ifndef PROFILE_DEPTH_DIALOG_H
#define PROFILE_DEPTH_DIALOG_H

#include <QDialog>

class MeasLine;

namespace Ui {
class ProfileDepthDialog;
}

class ProfileDepthDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProfileDepthDialog(QWidget *parent = 0);
    ~ProfileDepthDialog();

    void setMeasLine(MeasLine *_line, bool _topview = false);

public slots:
    void onCoordChanged(float _d, float _z);
    void onWidgetLeaved();

private:
    Ui::ProfileDepthDialog *ui;

    MeasLine *m_measLine;
};

#endif // PROFILE_DEPTH_DIALOG_H
