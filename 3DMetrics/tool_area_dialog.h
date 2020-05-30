#ifndef TOOL_AREA_DIALOG_H
#define TOOL_AREA_DIALOG_H

#include <QDialog>
#include <osg/Geode>

namespace Ui {
class ToolAreaDialog;
}

class MeasArea;

class ToolAreaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolAreaDialog(QWidget *_parent = 0);
    ~ToolAreaDialog();

protected:
    void mouseDoubleClickEvent( QMouseEvent * _e );
    void closeEvent(QCloseEvent *);
    void reject();

public slots:
    void start();
    void slot_toolEnded(QString&);  
    void removelast();
    void slot_nbPointsChanged();

private:
    Ui::ToolAreaDialog *ui;

    MeasArea *m_meas_area;
    osg::ref_ptr<osg::Geode> m_geode;
};

#endif // TOOL_AREA_DIALOG_H
