#ifndef MEASURE_PICKER_DIALOG_H
#define MEASURE_PICKER_DIALOG_H

#include <QDialog>
#include <osg/Node>

namespace Ui {
class MeasurePickerDialog;
}

class MeasurePickerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MeasurePickerDialog(QWidget *parent = 0);
    ~MeasurePickerDialog();

protected:
    void mouseDoubleClickEvent( QMouseEvent * _e );
    void closeEvent(QCloseEvent *);
    void reject();

public slots:
    void start();
    void slot_toolEnded();
    void slot_toolClicked(int, int);
    void slot_toolCanceled();

signals:
    void signal_toolStarted(QString &_info);
    void signal_toolEnded(QString &_info);
    void signal_nodeClicked(osg::Node *_node);

private:
    Ui::MeasurePickerDialog *ui;
};

#endif // MEASURE_PICKER_DIALOG_H
