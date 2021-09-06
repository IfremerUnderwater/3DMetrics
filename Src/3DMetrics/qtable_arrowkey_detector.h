#ifndef QTABLEARROWKEYDETECTOR_H
#define QTABLEARROWKEYDETECTOR_H

#include <QObject>

class QTableArrowKeyDetector : public QObject
{
    Q_OBJECT
public:
    explicit QTableArrowKeyDetector() : QObject() {}
    virtual ~QTableArrowKeyDetector() {}

public slots:

signals:
    void signal_KeyUpDown();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);
};


#endif // QTABLEARROWKEYDETECTOR_H
