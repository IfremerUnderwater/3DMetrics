#ifndef TOOL_POINT_DIALOG_H
#define TOOL_POINT_DIALOG_H

#include <QDialog>
#include <osg/Geode>

namespace Ui {
class ToolPointDialog;
}

class MeasPoint;

class ToolPointDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolPointDialog(QWidget *_parent = 0);
    ~ToolPointDialog();

protected:
    void mouseDoubleClickEvent( QMouseEvent * _e );
    void closeEvent(QCloseEvent *);
    void reject();

public slots:
    void start();

private:
    Ui::ToolPointDialog *ui;

    MeasPoint *m_meas_point;
    osg::ref_ptr<osg::Geode> m_geode;
};

#endif // TOOL_POINT_DIALOG_H
