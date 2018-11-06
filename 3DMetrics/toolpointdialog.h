#ifndef TOOLPOINTDIALOG_H
#define TOOLPOINTDIALOG_H

#include <QDialog>
#include <osg/Geode>

namespace Ui {
class ToolPointDialog;
}

class MeasurePoint;

class ToolPointDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolPointDialog(QWidget *parent = 0);
    ~ToolPointDialog();

protected:
    void mouseDoubleClickEvent( QMouseEvent * e );
    void closeEvent(QCloseEvent *);
    void reject();

public slots:
    void start();

private:
    Ui::ToolPointDialog *ui;

    MeasurePoint *m_measurepoint;
    osg::ref_ptr<osg::Geode> m_geode;
};

#endif // TOOLPOINTDIALOG_H
