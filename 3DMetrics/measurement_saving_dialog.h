#ifndef MEASUREMENT_SAVING_DIALOG_H
#define MEASUREMENT_SAVING_DIALOG_H
#include <QDialog>
#include "OSGWidget/OSGWidget.h"
#include "tool_handler.h"

namespace Ui {
class MeasurementSavingDialog;
}

class MeasurementSavingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MeasurementSavingDialog(QWidget *parent = 0);
    ~MeasurementSavingDialog();

    enum Category{
        BASALTE,
        SUBSTRAT_BRUN_ROUGE,
        SUBSTRAT_BRUN_AVEC_FILAMENTS_BACTERIENS,
        ANHYDRITE,
        COUVERTURE_MICROBIENNE,
        MOULIERES_AVEC_MOULES_DE_GRANDE_TAILLE_AVEC_FILAMENTS_BACTERIENS,
        MOULIERES_AVEC_MOULES_DE_GRANDE_TAILLE_SANS_FILAMENTS_BACTERIENS,
        MOULIERES_AVEC_MOULES_DE_TAILLES_MOYENNES_AVEC_FILAMENTS_BACTERIENS,
        MOULIERES_AVEC_MOULES_DE_TAILLES_MOYENNES_SANS_FILAMENTS_BACTERIENS,
        PETITES_MOULES_EPARSES_AVEC_FILAMENTS_BACTERIENS,
        PETITES_MOULES_EPARSES_SANS_FILAMENTS_BACTERIENS,
        CREVETTES,
        GASTEROPODES
    };

    void setDistanceSurfaceMeasValueAndType(double _measurement, ToolState _measurement_type, int _measurement_index);
    void setInterestPointMeasValueAndType(QString _coordinates, ToolState _measurement_type, int _measurement_index);



public slots:
    void slot_addMeasFormValues();
    void sl_clearMeasForm();
    void sl_clearPreviousMeasFormValues();

signals:
    void sig_getMeasFormValues(QString _measur_name, ToolState _measur_type, QString _category, QString _temperature, QString _measur_result, int _measur_counter, QString comments);

    void si_distanceMeasurementFormCanceled();
    void si_surfaceMeasurementFormCanceled();
    void si_interestPointMeasurementFormCanceled();


private:
    Ui::MeasurementSavingDialog *ui;
    int m_measur_counter;
    QMap<int,QString> m_category_names;

    int m_measurement_index;

};

#endif // MEASUREMENT_SAVING_DIALOG_H
