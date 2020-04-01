#ifndef Z_SCALE_DIALOG_H
#define Z_SCALE_DIALOG_H

#include <QDialog>

namespace Ui {
class ZScaleDialog;
}

class ZScaleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ZScaleDialog(QWidget *parent = 0);
    ~ZScaleDialog();

    void setZScale(double _zscale);

public slots:
    void slot_apply();
    void slot_changeZScale(int);

private:
    Ui::ZScaleDialog *ui;

    double m_zscale;
};

#endif // Z_SCALE_DIALOG_H
