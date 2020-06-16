#include "qtable_arrowkey_detector.h"
#include <QEvent>
#include <QKeyEvent>

bool QTableArrowKeyDetector::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        switch(keyEvent->key())
        {
        case Qt::Key_Up:
        case Qt::Key_Down:
            bool res = QObject::eventFilter(obj, event);
            emit signal_KeyUpDown();
            return res;
        }
    }
    return QObject::eventFilter(obj, event);
}
