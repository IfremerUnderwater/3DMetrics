#include "file_dialog.h"
#include <QFileDialog>

QString getOpenFileName(QWidget *_parent, QString _title, QString _directory, QString _filter)
{
    QFileDialog file_dialog(_parent,_title, _directory, _filter);
    file_dialog.setAcceptMode(QFileDialog::AcceptOpen);
    file_dialog.setFileMode(QFileDialog::ExistingFiles);

    file_dialog.setOption(QFileDialog::DontUseCustomDirectoryIcons, true);

#ifndef __MINGW32__
    // Ubuntu Linux doesn't show the dialog properly without this option
    file_dialog.setOption(QFileDialog::DontUseNativeDialog,true);
#endif

    if (QDialog::Accepted != file_dialog.exec())
        return QString();

    QStringList files = file_dialog.selectedFiles();
    if(files.count() > 0)
    {
        return files.first();
    }

    return QString();
}

QString getSaveFileName(QWidget *_parent, QString _title, QString _directory, QString _filter, QString _default_file)
{
    QFileDialog file_dialog(_parent,_title, _directory, _filter);
    file_dialog.setAcceptMode(QFileDialog::AcceptSave);
    file_dialog.setFileMode(QFileDialog::AnyFile);
    if(!_default_file.isEmpty())
    {
        QFileInfo finfo(_default_file);
        QString dir = finfo.absolutePath();
        file_dialog.setDirectory(dir);
        file_dialog.selectFile(finfo.fileName());
    }
    file_dialog.setOption(QFileDialog::DontUseCustomDirectoryIcons, true);

#ifndef __MINGW32__
    // Ubuntu Linux doesn't show the dialog properly without this option
    file_dialog.setOption(QFileDialog::DontUseNativeDialog,true);
#endif

    if (QDialog::Accepted != file_dialog.exec())
        return QString();

    QStringList files = file_dialog.selectedFiles();
    if(files.count() > 0)
    {
        return files.first();
    }

    return QString();
}
