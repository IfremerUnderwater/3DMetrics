#ifndef TRIANGULATION_THRESHOLD_DIALOG_H
#define TRIANGULATION_THRESHOLD_DIALOG_H

#include <QDialog>
class QDoubleValidator;

namespace Ui {
class TriangulationThresholdDialog;
}

class TriangulationThresholdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TriangulationThresholdDialog(QWidget *parent = nullptr);
    ~TriangulationThresholdDialog();

    double threshold() const;
    void setThreshold(double threshold);

    double dXthreshold() const;
    void setDXthreshold(double dXthreshold);

    double dYthreshold() const;
    void setDYthreshold(double dYthreshold);

    double dZthreshold() const;
    void setDZthreshold(double dZthreshold);

    virtual void accept();
private:
    Ui::TriangulationThresholdDialog *ui;

    double m_threshold;
    double m_dXthreshold;
    double m_dYthreshold;
    double m_dZthreshold;
    QDoubleValidator *m_doubleValidator;
};

#endif // TRIANGULATION_THRESHOLD_DIALOG_H
