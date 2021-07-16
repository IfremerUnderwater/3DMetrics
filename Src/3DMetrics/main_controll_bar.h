#ifndef TDM_MAIN_CONTROLL_BAR_H_
#define TDM_MAIN_CONTROLL_BAR_H_

#include <QFrame>
#include <QMouseEvent>
#include <QmenuBar>

namespace Ui {
class MainControllBar;
}


class MainControllBar : public QFrame
{
    Q_OBJECT

public:
    explicit MainControllBar(QWidget *_parent = 0);
    ~MainControllBar();

    // surcharge des événements
    void mousePressEvent  (QMouseEvent *_event);
    void mouseReleaseEvent(QMouseEvent *_event);
    void mouseMoveEvent   (QMouseEvent *_event);
	
	Ui::MainControllBar *m_ui;

protected:
    void changeEvent(QEvent *_event); // overriding event handler for dynamic translation

private:
    QPoint m_initial_position;


signals:
    void si_moveWindow(const QPoint &_new_pos);
};

#endif // TDM_MAIN_CONTROLL_BAR_H_
