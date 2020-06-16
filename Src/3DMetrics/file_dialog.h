#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <QString>
#include <QWidget>

// QFileDialog::getOpenFileName replacement

QString getOpenFileName(QWidget *_parent, QString _title, QString _directory, QString _filter);
QString getSaveFileName(QWidget *_parent, QString _title, QString _directory, QString _filter, QString _default_file = "");

#endif // FILE_DIALOG_H

