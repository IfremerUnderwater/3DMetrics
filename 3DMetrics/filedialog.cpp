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
