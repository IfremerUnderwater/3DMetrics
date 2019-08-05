#ifndef EDIT_TRANSPARENCY_MODEL_H
#define EDIT_TRANSPARENCY_MODEL_H

#include <QDialog>

namespace Ui {
class EditTransparencyModel;
}

class EditTransparencyModel : public QDialog
{
    Q_OBJECT

public:
    explicit EditTransparencyModel(QWidget *parent = 0);
    ~EditTransparencyModel();

    double getTransparencyValue() { return m_transparency_value; }
    void setTransparency(double _transparency_value);

signals:
    void signal_onChangedTransparencyValue(int _transparency_value);

private:
    Ui::EditTransparencyModel *ui;
    double m_transparency_value;

private slots:
    void slot_changeTransparencyValue(int);
};

#endif // EDIT_TRANSPARENCY_MODEL_H
