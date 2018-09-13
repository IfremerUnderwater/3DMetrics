#include <TDMLayers/tdmlayer.h>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings> // TODO: get rid of it [MD]


TDMLayer::TDMLayer( TDMLayer::LayerType _type,
                    const QString& _layername,
                    const QString& _source )
    : m_data_source( _source )
    , m_layer_orig_name( _layername ) // store the original name
    , m_ID( "" )
    , m_layer_type( _type )
{
    // Set the display name = internal name
    m_layer_name = m_layer_orig_name; // capitaliseLayerName( m_layer_orig_name );

    // Generate the unique ID of this layer
    QDateTime dt = QDateTime::currentDateTime();
    m_ID = _layername + dt.toString( "yyyyMMddhhmmsszzz" );
    // Tidy the ID up to avoid characters that may cause problems
    // elsewhere (e.g in some parts of XML). Replaces every non-word
    // character (word characters are the alphabet, numbers and
    // underscore) with an underscore.
    // Note that the first backslashe in the regular expression is
    // there for the compiler, so the pattern is actually \W
    m_ID.replace( QRegExp( "[\\W]" ), "_" );

}

TDMLayer::~TDMLayer()
{
}

TDMLayer::LayerType TDMLayer::type() const
{
    return m_layer_type;
}

/** Get this layer's unique ID */
QString TDMLayer::id() const
{
    return m_ID;
}

void TDMLayer::setName( const QString& _name )
{
    QString newName = _name; //capitaliseLayerName( name );
    if ( _name == m_layer_orig_name && newName == m_layer_name )
        return;

    m_layer_orig_name = _name; // store the new original name
    m_layer_name = newName;

    emit nameChanged();
}

/** Read property of QString layerName. */
QString TDMLayer::name() const
{
    return m_layer_name;
}


/*QgsRectangle TDMLayer::extent()
{
  return mExtent;
}*/


QStringList TDMLayer::subLayers() const
{
    return QStringList();  // Empty
}

/*const QgsCoordinateReferenceSystem& TDMLayer::crs() const
{
  return mCRS;
}*/

/*void TDMLayer::setCrs( const QgsCoordinateReferenceSystem& srs, bool emitSignal )
{
  mCRS = srs;

  if ( !mCRS.isValid() )
  {
    mCRS.setValidationHint( tr( "Specify CRS for layer %1" ).arg( name() ) );
    mCRS.validate();
  }

  if ( emitSignal )
    emit layerCrsChanged();
}*/

//QString TDMLayer::capitaliseLayerName(const QString& _name )
//{
//  // Capitalise the first letter of the layer name if requested
//  QSettings settings;
//  bool capitaliseLayerName =
//    settings.value( "/qgis/capitaliseLayerName", QVariant( false ) ).toBool();

//  QString layerName( _name );

//  if ( capitaliseLayerName && !layerName.isEmpty() )
//    layerName = layerName.at( 0 ).toUpper() + layerName.mid( 1 );

//  return layerName;
//}


bool TDMLayer::isEditable() const
{
    return false;
}



/*void TDMLayer::setExtent( const QgsRectangle &r )
{
  mExtent = r;
}*/
