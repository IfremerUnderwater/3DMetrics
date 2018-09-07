#include <TDMLayers/tdmlayer.h>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings> // TODO: get rid of it [MD]


TDMLayer::TDMLayer( TDMLayer::LayerType type,
                          const QString& layername,
                          const QString& source )
    : mDataSource( source )
    , mLayerOrigName( layername ) // store the original name
    , mID( "" )
    , mLayerType( type )
{
  // Set the display name = internal name
  mLayerName = capitaliseLayerName( mLayerOrigName );

  // Generate the unique ID of this layer
  QDateTime dt = QDateTime::currentDateTime();
  mID = layername + dt.toString( "yyyyMMddhhmmsszzz" );
  // Tidy the ID up to avoid characters that may cause problems
  // elsewhere (e.g in some parts of XML). Replaces every non-word
  // character (word characters are the alphabet, numbers and
  // underscore) with an underscore.
  // Note that the first backslashe in the regular expression is
  // there for the compiler, so the pattern is actually \W
  mID.replace( QRegExp( "[\\W]" ), "_" );

}

TDMLayer::~TDMLayer()
{
}

TDMLayer::LayerType TDMLayer::type() const
{
  return mLayerType;
}

/** Get this layer's unique ID */
QString TDMLayer::id() const
{
  return mID;
}

void TDMLayer::setName( const QString& name )
{
  QString newName = capitaliseLayerName( name );
  if ( name == mLayerOrigName && newName == mLayerName )
    return;

  mLayerOrigName = name; // store the new original name
  mLayerName = newName;

  emit nameChanged();
}

/** Read property of QString layerName. */
QString TDMLayer::name() const
{
  return mLayerName;
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

QString TDMLayer::capitaliseLayerName( const QString& name )
{
  // Capitalise the first letter of the layer name if requested
  QSettings settings;
  bool capitaliseLayerName =
    settings.value( "/qgis/capitaliseLayerName", QVariant( false ) ).toBool();

  QString layerName( name );

  if ( capitaliseLayerName && !layerName.isEmpty() )
    layerName = layerName.at( 0 ).toUpper() + layerName.mid( 1 );

  return layerName;
}


bool TDMLayer::isEditable() const
{
  return false;
}



/*void TDMLayer::setExtent( const QgsRectangle &r )
{
  mExtent = r;
}*/
