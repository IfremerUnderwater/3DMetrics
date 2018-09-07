/***************************************************************************
 *  TDMLayerRegistry.cpp  -  Singleton class for tracking mMapLayers.
 *                         -------------------

 ***************************************************************************/

#include "tdmmaplayerregistry.h"
#include "tdmlayer.h"

//
// Static calls to enforce singleton behaviour
//
TDMLayerRegistry *TDMLayerRegistry::instance()
{
  static TDMLayerRegistry sInstance;
  return &sInstance;
}

TDMLayerRegistry::TDMLayerRegistry( QObject *parent )
    : QObject( parent )
{}

TDMLayerRegistry::~TDMLayerRegistry()
{
  removeAllMapLayers();
}

int TDMLayerRegistry::count() const
{
  return mMapLayers.size();
}

TDMLayer * TDMLayerRegistry::mapLayer( const QString& theLayerId ) const
{
  return mMapLayers.value( theLayerId );
}

QList<TDMLayer *> TDMLayerRegistry::mapLayersByName( const QString& layerName ) const
{
  QList<TDMLayer *> myResultList;
  Q_FOREACH ( TDMLayer* layer, mMapLayers )
  {
    if ( layer->name() == layerName )
    {
      myResultList << layer;
    }
  }
  return myResultList;
}

QList<TDMLayer *> TDMLayerRegistry::addMapLayers(
  const QList<TDMLayer *>& theMapLayers,
  bool addToLegend,
  bool takeOwnership )
{
  QList<TDMLayer *> myResultList;
  Q_FOREACH ( TDMLayer* myLayer, theMapLayers )
  {
    if ( !myLayer )
    {
      continue;
    }
    //check the layer is not already registered!
    if ( !mMapLayers.contains( myLayer->id() ) )
    {
      mMapLayers[myLayer->id()] = myLayer;
      myResultList << mMapLayers[myLayer->id()];
      if ( takeOwnership )
      {
        myLayer->setParent( this );
      }
      connect( myLayer, SIGNAL( destroyed( QObject* ) ), this, SLOT( onMapLayerDeleted( QObject* ) ) );
      emit layerWasAdded( myLayer );
    }
  }
  if ( !myResultList.isEmpty() )
  {
    emit layersAdded( myResultList );

    if ( addToLegend )
      emit legendLayersAdded( myResultList );
  }
  return myResultList;
}

TDMLayer *
TDMLayerRegistry::addMapLayer( TDMLayer* theMapLayer,
                                  bool addToLegend,
                                  bool takeOwnership )
{
  QList<TDMLayer *> addedLayers;
  addedLayers = addMapLayers( QList<TDMLayer*>() << theMapLayer, addToLegend, takeOwnership );
  return addedLayers.isEmpty() ? nullptr : addedLayers[0];
}

void TDMLayerRegistry::removeMapLayers( const QStringList& theLayerIds )
{
  QList<TDMLayer*> layers;
  Q_FOREACH ( const QString &myId, theLayerIds )
  {
    layers << mMapLayers.value( myId );
  }

  removeMapLayers( layers );
}

void TDMLayerRegistry::removeMapLayers( const QList<TDMLayer*>& layers )
{
  if ( layers.isEmpty() )
    return;

  QStringList layerIds;
  QList<TDMLayer*> layerList;

  Q_FOREACH ( TDMLayer* layer, layers )
  {
    // check layer and the registry contains it
    if ( layer && mMapLayers.contains( layer->id() ) )
    {
      layerIds << layer->id();
      layerList << layer;
    }
  }

  if ( layerIds.isEmpty() )
    return;

  emit layersWillBeRemoved( layerIds );
  emit layersWillBeRemoved( layerList );

  Q_FOREACH ( TDMLayer* lyr, layerList )
  {
    QString myId( lyr->id() );
    emit layerWillBeRemoved( myId );
    emit layerWillBeRemoved( lyr );
    mMapLayers.remove( myId );
    if ( lyr->parent() == this )
    {
      delete lyr;
    }
    emit layerRemoved( myId );
  }

  emit layersRemoved( layerIds );
}

void TDMLayerRegistry::removeMapLayer( const QString& theLayerId )
{
  removeMapLayers( QList<TDMLayer*>() << mMapLayers.value( theLayerId ) );
}

void TDMLayerRegistry::removeMapLayer( TDMLayer* layer )
{
  if ( layer )
    removeMapLayers( QList<TDMLayer*>() << layer );
}

void TDMLayerRegistry::removeAllMapLayers()
{
  emit removeAll();
  // now let all observers know to clear themselves,
  // and then consequently any of their map legends
  removeMapLayers( mMapLayers.keys() );
  mMapLayers.clear();
}


void TDMLayerRegistry::onMapLayerDeleted( QObject* obj )
{
  QString id = mMapLayers.key( static_cast<TDMLayer*>( obj ) );

  if ( !id.isNull() )
  {
    //TDMDebugMsg( QString( "Map layer deleted without unregistering! %1" ).arg( id ) );
    mMapLayers.remove( id );
  }
}

QMap<QString, TDMLayer*> TDMLayerRegistry::mapLayers() const
{
  return mMapLayers;
}


#if 0
void TDMLayerRegistry::connectNotify( const char * signal )
{
  Q_UNUSED( signal );
  //TDMDebugMsg("TDMLayerRegistry connected to " + QString(signal));
} //  TDMLayerRegistry::connectNotify
#endif
