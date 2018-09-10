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

TDMLayerRegistry::TDMLayerRegistry(QObject *_parent )
    : QAbstractItemModel( _parent )
{}

TDMLayerRegistry::~TDMLayerRegistry()
{
  removeAllMapLayers();
}

int TDMLayerRegistry::count() const
{
  return m_map_layers.size();
}

//***************** Model members **************************************************

QVariant TDMLayerRegistry::data(const QModelIndex &_index, int _role) const
{
    /*if (!_index.isValid())
        return QVariant();

    SGDTTreeItem *item = static_cast<SGDTTreeItem*>(_index.internalPointer());

    if (_role == Qt::DisplayRole){
        return item->data(_index.column());

    }

    if ( _role == Qt::ForegroundRole )
    {
        return QVariant ( m_types_colors[item->type()] );
    }

    if ( _role == Qt::BackgroundRole )
    {
        if (item->type()=="Equipements" && !item->hasChildren()){
            return QVariant(QColor( Qt::darkRed ));
        }
    }*/

    return QVariant();
}

Qt::ItemFlags TDMLayerRegistry::flags(const QModelIndex &_index) const
{
    if (!_index.isValid())
        return 0;

    return QAbstractItemModel::flags(_index);
}

QVariant TDMLayerRegistry::headerData(int _section, Qt::Orientation orientation,
                                   int _role) const
{
    /*if (orientation == Qt::Horizontal && _role == Qt::DisplayRole)
        return m_root_item->data(_section);*/

    return QVariant();
}

QModelIndex TDMLayerRegistry::index(int _row, int _column, const QModelIndex &_parent)
const
{
    /*if (!hasIndex(_row, _column, _parent))
        return QModelIndex();

    SGDTTreeItem *parentItem;

    if (!_parent.isValid())
        parentItem = m_root_item;
    else
        parentItem = static_cast<SGDTTreeItem*>(_parent.internalPointer());

    SGDTTreeItem *childItem = parentItem->child(_row);
    if (childItem)
        return createIndex(_row, _column, childItem);
    else
        return QModelIndex();*/
}

QModelIndex TDMLayerRegistry::parent(const QModelIndex &_index) const
{
    /*if (!_index.isValid())
        return QModelIndex();

    SGDTTreeItem *childItem = static_cast<SGDTTreeItem*>(_index.internalPointer());
    SGDTTreeItem *parentItem = childItem->parentItem();

    if (parentItem == m_root_item)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);*/
}

int TDMLayerRegistry::rowCount(const QModelIndex &_parent) const
{
    /*SGDTTreeItem *parentItem;
    if (_parent.column() > 0)
        return 0;

    if (!_parent.isValid())
        parentItem = m_root_item;
    else
        parentItem = static_cast<SGDTTreeItem*>(_parent.internalPointer());

    return parentItem->childCount();*/
}

int TDMLayerRegistry::columnCount(const QModelIndex &_parent) const
{
    /*if (_parent.isValid())
        return static_cast<SGDTTreeItem*>(_parent.internalPointer())->columnCount();
    else
        return m_root_item->columnCount();*/
}

//************************************************************************************

TDMLayer * TDMLayerRegistry::mapLayer(const QString& _layer_id ) const
{
  return m_map_layers.value( _layer_id );
}

QList<TDMLayer *> TDMLayerRegistry::mapLayersByName(const QString& _layer_name ) const
{
  QList<TDMLayer *> myResultList;
  Q_FOREACH ( TDMLayer* layer, m_map_layers )
  {
    if ( layer->_name() == _layer_name )
    {
      myResultList << layer;
    }
  }
  return myResultList;
}

QList<TDMLayer *> TDMLayerRegistry::addMapLayers(const QList<TDMLayer *>& _the_map_layers,
//  bool _add_to_legend,
  bool _take_ownership )
{
  QList<TDMLayer *> myResultList;
  Q_FOREACH ( TDMLayer* myLayer, _the_map_layers )
  {
    if ( !myLayer )
    {
      continue;
    }
    //check the layer is not already registered!
    if ( !m_map_layers.contains( myLayer->id() ) )
    {
      m_map_layers[myLayer->id()] = myLayer;
      myResultList << m_map_layers[myLayer->id()];
      if ( _take_ownership )
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

//    if ( _add_to_legend )
//      emit legendLayersAdded( myResultList );
  }
  return myResultList;
}

TDMLayer *
TDMLayerRegistry::addMapLayer(TDMLayer* _map_layer,
//                                  bool _add_to_legend,
                                  bool _take_ownership )
{
  QList<TDMLayer *> addedLayers;
  addedLayers = addMapLayers( QList<TDMLayer*>() << _map_layer, /*_add_to_legend,*/ _take_ownership );
  return addedLayers.isEmpty() ? nullptr : addedLayers[0];
}

void TDMLayerRegistry::removeMapLayers(const QStringList& _layer_ids )
{
  QList<TDMLayer*> layers;
  Q_FOREACH ( const QString &myId, _layer_ids )
  {
    layers << m_map_layers.value( myId );
  }

  removeMapLayers( layers );
}

void TDMLayerRegistry::removeMapLayers(const QList<TDMLayer*>& _layers )
{
  if ( _layers.isEmpty() )
    return;

  QStringList layerIds;
  QList<TDMLayer*> layerList;

  Q_FOREACH ( TDMLayer* layer, _layers )
  {
    // check layer and the registry contains it
    if ( layer && m_map_layers.contains( layer->id() ) )
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
    m_map_layers.remove( myId );
    if ( lyr->parent() == this )
    {
      delete lyr;
    }
    emit layerRemoved( myId );
  }

  emit layersRemoved( layerIds );
}

void TDMLayerRegistry::removeMapLayer(const QString& _layer_id )
{
  removeMapLayers( QList<TDMLayer*>() << m_map_layers.value( _layer_id ) );
}

void TDMLayerRegistry::removeMapLayer(TDMLayer* _layer )
{
  if ( _layer )
    removeMapLayers( QList<TDMLayer*>() << _layer );
}

void TDMLayerRegistry::removeAllMapLayers()
{
  emit removeAll();
  // now let all observers know to clear themselves,
  // and then consequently any of their map legends
  removeMapLayers( m_map_layers.keys() );
  m_map_layers.clear();
}


void TDMLayerRegistry::onMapLayerDeleted(QObject* _obj )
{
  QString id = m_map_layers.key( static_cast<TDMLayer*>( _obj ) );

  if ( !id.isNull() )
  {
    //TDMDebugMsg( QString( "Map layer deleted without unregistering! %1" ).arg( id ) );
    m_map_layers.remove( id );
  }
}

QMap<QString, TDMLayer*> TDMLayerRegistry::mapLayers() const
{
  return m_map_layers;
}


#if 0
void TDMLayerRegistry::connectNotify( const char * signal )
{
  Q_UNUSED( signal );
  //TDMDebugMsg("TDMLayerRegistry connected to " + QString(signal));
} //  TDMLayerRegistry::connectNotify
#endif
