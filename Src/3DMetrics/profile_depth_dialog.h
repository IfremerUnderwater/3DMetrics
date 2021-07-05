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
    void sl_exportProfileAsCsv();

private:
    Ui::ProfileDepthDialog *ui;

    MeasLine *m_measLine;
    QVector<QPointF> m_fpts;
    QVector<QPointF> m_mpts;
};

#endif // PROFILE_DEPTH_DIALOG_H
