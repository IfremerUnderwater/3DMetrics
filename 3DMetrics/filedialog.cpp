#include "filedialog.h"
#include <QFileDialog>

QString getOpenFileName(QWidget *_parent, QString _title, QString _directory, QString _filter)
{
    QFileDialog fileDialog(_parent,_title, _directory, _filter);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFiles);

    fileDialog.setOption(QFileDialog::DontUseCustomDirectoryIcons, true);

#ifndef __MINGW32__
    // Ubuntu Linux doesn't show the dialog properly without this option
    fileDialog.setOption(QFileDialog::DontUseNativeDialog,true);
#endif

    if (QDialog::Accepted != fileDialog.exec())
        return QString();

    QStringList files = fileDialog.selectedFiles();
    if(files.count() > 0)
    {
        return files.first();
    }

    return QString();
}

QString getSaveFileName(QWidget *_parent, QString _title, QString _directory, QString _filter, QString _defaultFile)
{
    QFileDialog fileDialog(_parent,_title, _directory, _filter);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    if(!_defaultFile.isEmpty())
    {
        QFileInfo finfo(_defaultFile);
        QString dir = finfo.absolutePath();
        fileDialog.setDirectory(dir);
        fileDialog.selectFile(finfo.fileName());
    }
    fileDialog.setOption(QFileDialog::DontUseCustomDirectoryIcons, true);

#ifndef __MINGW32__
    // Ubuntu Linux doesn't show the dialog properly without this option
    fileDialog.setOption(QFileDialog::DontUseNativeDialog,true);
#endif

    if (QDialog::Accepted != fileDialog.exec())
        return QString();

    QStringList files = fileDialog.selectedFiles();
    if(files.count() > 0)
    {
        return files.first();
    }

    return QString();
}
