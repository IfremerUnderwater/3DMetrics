#ifndef TOOL_LINE_DIALOG_H
#define TOOL_LINE_DIALOG_H

#include <QDialog>
#include <osg/Geode>

namespace Ui {
class ToolLineDialog;
}

class MeasLine;

class ToolLineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolLineDialog(QWidget *_parent = 0);
    ~ToolLineDialog();

protected:
    void mouseDoubleClickEvent( QMouseEvent * _e );
    void closeEvent(QCloseEvent *);
    void reject();

public slots:
    void start();
    void removelast();
    void profile();
    void profile_z();
    void slot_toolEnded(QString&);
    void slot_nbPointsChanged();

private:
    Ui::ToolLineDialog *ui;

    MeasLine *m_meas_line;
    osg::ref_ptr<osg::Geode> m_geode;
};

#endif // TOOL_LINE_DIALOG_H
