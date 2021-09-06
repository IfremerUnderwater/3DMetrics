#include "style_utils.h"

namespace style_tools {

QString StyleUtils::substitutePlaceHolders(QString _source, QMap<QString, QString> _properties, QString _place_holder_pattern)
{
    QString resolved_ph_pattern = _place_holder_pattern.arg(PROPERTY_NAME_PATTERN);
    QRegExp ph_rex(resolved_ph_pattern);

    QString current(_source);

//    qDebug() << "Source string with placeholders:\n" << source;

    int index = ph_rex.indexIn(current);

    while(index >= 0) {
        QString phKey = ph_rex.cap(1);

        if (_properties.contains(phKey)) {
            //qDebug() << QString("Subsituting placeholder %1").arg(phKey);
            QString value = _properties.value(phKey);
            int ph_size = ph_rex.matchedLength();
            current.replace(index, ph_size, value);
        } else {
            qWarning() << QString("Unknown placeholder %1").arg(phKey);
        }

        index = ph_rex.indexIn(current, index + 1); // index + 1 to avoid looping on unknown placeholders
    }

    return current;
}

QMap<QString, QString> StyleUtils::readPropertiesFile(QString _prop_file_path) {
  QFile prop_file(_prop_file_path);
  QMap<QString, QString> properties;

  if (!prop_file.exists()) {
    qCritical()
        << QString("The file '%1' does not exist, could not load properties")
               .arg(_prop_file_path);
    return properties;
  }

  if (!prop_file.open(QIODevice::ReadOnly)) {
    qCritical()
        << QString(
               "I/O error while opening file '%1', could not load properties")
               .arg(_prop_file_path);
    return properties;
  }

  QString resolved_prop_def_pattern =
      QString(PROPERTY_DEFINITION_PATTERN).arg(PROPERTY_NAME_PATTERN);
  QRegExp prop_def_rex(resolved_prop_def_pattern);

  QTextStream in(&prop_file);
  while (!in.atEnd()) {
    QString line = in.readLine().trimmed();

    if (line.isEmpty()) {
      continue;
    }

    if (line.startsWith('#')) {
      // comment
      continue;
    }

    if (!prop_def_rex.exactMatch(line)) {
      qWarning() << QString(
                        "Line does not match property definition pattern and "
                        "will be ignored :\n%1")
                        .arg(line);
      continue;
    }

    QStringList parts = line.split("=");

    if (parts.size() < 2) {
      qCritical() << QString("Line could not be parsed :\n%1").arg(line);
      continue;
    }

    QString key = parts.at(0);
    QString value = parts.at(1);

    if (properties.contains(key)) {
      qWarning() << QString(
                        "Property '%1' defined more than once, only first "
                        "value will be kept.")
                        .arg(key);
      continue;
    }

    properties.insert(key, value);
  }

  prop_file.close();

  return properties;
}

} // namespace system_tools
