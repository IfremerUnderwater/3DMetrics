#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QString>
#include <QWidget>

// QFileDialog::getOpenFileName replacement

QString getOpenFileName(QWidget *_parent, QString _title, QString _directory, QString _filter);
QString getSaveFileName(QWidget *_parent, QString _title, QString _directory, QString _filter, QString _defaultFile = "");

#endif // FILEDIALOG_H

