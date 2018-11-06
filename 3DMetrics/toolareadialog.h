#ifndef TOOLAREADIALOG_H
#define TOOLAREADIALOG_H

#include <QDialog>
#include <osg/Geode>

namespace Ui {
class ToolAreaDialog;
}

class MeasureArea;

class ToolAreaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolAreaDialog(QWidget *parent = 0);
    ~ToolAreaDialog();

protected:
    void mouseDoubleClickEvent( QMouseEvent * e );
    void closeEvent(QCloseEvent *);
    void reject();

public slots:
    void start();

private:
    Ui::ToolAreaDialog *ui;

    MeasureArea *m_measurearea;
    osg::ref_ptr<osg::Geode> m_geode;
};

#endif // TOOLAREADIALOG_H
