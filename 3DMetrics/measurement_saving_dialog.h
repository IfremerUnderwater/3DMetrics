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

    void setMeasFields(QString _measurement_formatted, QString _measurement_type);

    void cleanMeasFormValues();

    QString getMeasName();
    QString getMeasTemp();
    QString getMeasComment();
    QString getMeasCategory();

protected:
    void keyPressEvent(QKeyEvent * _e);

public slots:
    void sl_cancelSaving();
    void sl_acceptSaving();

signals:
    void si_measFormCanceled();
    void si_measFormAccepted();

private:
    Ui::MeasurementSavingDialog *ui;

    QVector<QString> m_category_names;
    QString m_last_meas_name;

};

#endif // MEASUREMENT_SAVING_DIALOG_H
