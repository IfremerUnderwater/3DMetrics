#ifndef STYLE_TOOLS_UTILS_H_
#define STYLE_TOOLS_UTILS_H_



#include <QString>
#include <QRegExp>
#include <QMap>
#include <QtDebug>
#include <QFile>

#define PROPERTY_NAME_PATTERN           "\\w+(?:\\.\\w+)*"
#define PLACEHOLDER_PATTERN_CSS         "<%(%1)%>"          // Placeholder pattern compatible with CSS syntax
#define PLACEHOLDER_PATTERN_XML_1       "\\{%(%1)%\\}"          // Placeholder pattern compatible with XML syntax
#define PLACEHOLDER_PATTERN_XML_2       "\\{&(%1)&\\}"          // Placeholder pattern compatible with XML syntax (for embedding in pattern #1)
#define PLACEHOLDER_PATTERN_DEFAULT     PLACEHOLDER_PATTERN_CSS

// This pattern contains a placeholder (%1) to insert the property name pattern
#define PROPERTY_DEFINITION_PATTERN     "%1\\s*\\=[^\\=]*"
#define TEMP_DIR_TEMPLATE_FORMAT        "'%1'yyyyMMdd'-'hhmmss'.'zzz"
#define TEMP_DIR_MAX_TRIALS             3
#define DEFAULT_TEMP_DIR_PREFIX         "tdm-"

namespace style_tools {

class StyleUtils
{
public:
    static QString substitutePlaceHolders(QString _source, QMap<QString,QString> _properties,
                                          QString _place_holder_pattern = QString(PLACEHOLDER_PATTERN_DEFAULT));
    static QMap<QString, QString> StyleUtils::readPropertiesFile(QString _prop_file_path);
};

} // namespace style_tools

#endif // STYLE_TOOLS_UTILS_H_
