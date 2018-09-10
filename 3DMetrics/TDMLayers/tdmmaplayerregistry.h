/***************************************************************************
                          qgsmaplayerregistry.h
           Singleton class for keeping track of loaded layers
                             -------------------
    begin                : Sept 7 2018

 ***************************************************************************/

#ifndef TDMMAPLAYERREGISTRY_H
#define TDMMAPLAYERREGISTRY_H

#include <QMap>
#include <QAbstractItemModel>
#include <QStringList>

class QString;
class TDMLayer;

/** \ingroup core
 * \class TDMLayerRegistry
 * This class tracks map layers that are currently loaded and provides
 * various methods to retrieve matching layers from the registry.
 */
class TDMLayerRegistry : public QAbstractItemModel
{
    Q_OBJECT

  public:

    //! Returns the instance pointer, creating the object on the first call
    static TDMLayerRegistry * instance();

    ~TDMLayerRegistry();

    // Pure QAbstractItemModel members ******************************************
    QVariant data(const QModelIndex &_index, int _role) const override;
    Qt::ItemFlags flags(const QModelIndex &_index) const override;
    QVariant headerData(int _section, Qt::Orientation orientation,
                        int _role = Qt::DisplayRole) const override;
    QModelIndex index(int _row, int _column,
                      const QModelIndex &_parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &_index) const override;
    int rowCount(const QModelIndex &_parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &_parent = QModelIndex()) const override;
    // **************************************************************************

    //! Returns the number of registered layers.
    int count() const;

    /** Retrieve a pointer to a registered layer by layer ID.
     * @param _layer_id ID of layer to retrieve
     * @returns matching layer, or nullptr if no matching layer found
     * @see mapLayersByName()
     * @see mapLayers()
     */
    TDMLayer* mapLayer( const QString& _layer_id ) const;

    /** Retrieve a list of matching registered layers by layer name.
     * @param _layer_name name of layers to match
     * @returns list of matching layers
     * @see mapLayer()
     * @see mapLayers()
     */
    QList<TDMLayer *> mapLayersByName( const QString& _layer_name ) const;

    /** Returns a map of all registered layers by layer ID.
     * @see mapLayer()
     * @see mapLayersByName()
     * @see layers()
     */
    QMap<QString, TDMLayer*> mapLayers() const;

    /** Returns a list of registered map layers with a specified layer type.
     *
     * Example:
     *
     *     QVector<TDMVectorLayer*> vectorLayers = TDMLayerRegistry::instance()->layers<TDMVectorLayer*>();
     *

     */
    template <typename T>
    QVector<T> layers() const
    {
      QVector<T> layers;
      QMap<QString, TDMLayer*>::const_iterator layerIt = m_map_layers.constBegin();
      for ( ; layerIt != m_map_layers.constEnd(); ++layerIt )
      {
        T tLayer = qobject_cast<T>( layerIt.value() );
        if ( tLayer )
        {
          layers << tLayer;
        }
      }
      return layers;
    }

    /**
     * @brief
     * Add a list of layers to the map of loaded layers.
     *
     * The layersAdded() and layerWasAdded() signals will always be emitted.
     * The legendLayersAdded() signal is emitted only if addToLegend is true.
     *
     * @param _the_map_layers  A list of layer which should be added to the registry
     * @param _add_to_legend   If true (by default), the layers will be added to the
     *                      legend and to the main canvas. If you have a private
     *                      layer you can set this parameter to false to hide it.
     * @param _take_ownership Ownership will be transferred to the layer registry.
     *                      If you specify false here you have take care of deleting
     *                      the layers yourself. Not available in python.
     *
     * @return a list of the map layers that were added
     *         successfully. If a layer is invalid, or already exists in the registry,
     *         it will not be part of the returned QList.
     *
     * @note As a side-effect TDMProject is made dirty.
     * @note takeOwnership is not available in the Python bindings - the registry will always
     * take ownership
     * @note added in QGIS 1.8
     * @see addMapLayer()
     */
    QList<TDMLayer *> addMapLayers( const QList<TDMLayer*>& _the_map_layers,
//                                       bool _add_to_legend = true,
                                       bool _take_ownership = true );

    /**
     * @brief
     * Add a layer to the map of loaded layers.
     *
     * The layersAdded() and layerWasAdded() signals will always be emitted.
     * The legendLayersAdded() signal is emitted only if addToLegend is true.
     * If you are adding multiple layers at once, you should use
     * addMapLayers() instead.
     *
     * @param _map_layer A layer to add to the registry
     * @param _add_to_legend If true (by default), the layer will be added to the
     *                    legend and to the main canvas. If you have a private
     *                    layer you can set this parameter to false to hide it.
     * @param _take_ownership Ownership will be transferred to the layer registry.
     *                      If you specify false here you have take care of deleting
     *                      the layer yourself. Not available in python.
     *
     * @return nullptr if unable to add layer, otherwise pointer to newly added layer
     *
     * @see addMapLayers
     *
     * @note Use addMapLayers if adding more than one layer at a time

     */
    TDMLayer* addMapLayer( TDMLayer * _map_layer,
//                           bool _add_to_legend = true,
                           bool _take_ownership = true );

    /**
     * @brief
     * Remove a set of layers from the registry by layer ID.
     *
     * The specified layers will be removed from the registry. If the registry has ownership
     * of any layers these layers will also be deleted.
     *
     * @param _layer_ids list of IDs of the layers to remove
     *
     * @note As a side-effect the TDMProject instance is marked dirty.

     * @see removeMapLayer()
     * @see removeAllMapLayers()
     */
    // TODO QGIS 3.0 - rename theLayerIds to layerIds
    void removeMapLayers( const QStringList& _layer_ids );

    /**
     * @brief
     * Remove a set of layers from the registry.
     *
     * The specified layers will be removed from the registry. If the registry has ownership
     * of any layers these layers will also be deleted.
     *
     * @param _layers A list of layers to remove. Null pointers are ignored.
     *
     * @note As a side-effect the TDMProject instance is marked dirty.
     * @see removeMapLayer()
     * @see removeAllMapLayers()
     */
    void removeMapLayers( const QList<TDMLayer*>& _layers );

    /**
     * @brief
     * Remove a layer from the registry by layer ID.
     *
     * The specified layer will be removed from the registry. If the registry has ownership
     * of the layer then it will also be deleted.
     *
     * @param _layer_id ID of the layer to remove
     *
     * @note As a side-effect the TDMProject instance is marked dirty.
     * @see removeMapLayers()
     * @see removeAllMapLayers()
     */
    void removeMapLayer( const QString& _layer_id );

    /**
     * @brief
     * Remove a layer from the registry.
     *
     * The specified layer will be removed from the registry. If the registry has ownership
     * of the layer then it will also be deleted.
     *
     * @param _layer The layer to remove. Null pointers are ignored.
     *
     * @note As a side-effect the TDMProject instance is marked dirty.
     * @see removeMapLayers()
     * @see removeAllMapLayers()
     */
    void removeMapLayer( TDMLayer* _layer );

    /**
     * Removes all registered layers. If the registry has ownership
     * of any layers these layers will also be deleted.
     *
     * @note As a side-effect the TDMProject instance is marked dirty.
     * @note Calling this method will cause the removeAll() signal to
     * be emitted.
     * @see removeMapLayer()
     * @see removeMapLayers()
     */
    void removeAllMapLayers();


  signals:

    /**
     * Emitted when one or more layers are about to be removed from the registry.
     *
     * @param _layer_ids A list of IDs for the layers which are to be removed.
     * @see layerWillBeRemoved()
     * @see layersRemoved()
     */
    void layersWillBeRemoved( const QStringList& _layer_ids );

    /**
     * Emitted when one or more layers are about to be removed from the registry.
     *
     * @param _layers A list of layers which are to be removed.
     * @see layerWillBeRemoved()
     * @see layersRemoved()
     */
    void layersWillBeRemoved( const QList<TDMLayer*>& _layers );

    /**
     * Emitted when a layer is about to be removed from the registry.
     *
     * @param _layer_id The ID of the layer to be removed.
     *
     * @note Consider using {@link layersWillBeRemoved()} instead
     * @see layersWillBeRemoved()
     * @see layerRemoved()
     */
    void layerWillBeRemoved( const QString& _layer_id );

    /**
     * Emitted when a layer is about to be removed from the registry.
     *
     * @param _layer The layer to be removed.
     *
     * @note Consider using {@link layersWillBeRemoved()} instead
     * @see layersWillBeRemoved()
     * @see layerRemoved()
     */
    void layerWillBeRemoved( TDMLayer* _layer );

    /**
     * Emitted after one or more layers were removed from the registry.
     *
     * @param _layer_ids  A list of IDs of the layers which were removed.
     * @see layersWillBeRemoved()
     */
    void layersRemoved( const QStringList& _layer_ids );

    /**
     * Emitted after a layer was removed from the registry.
     *
     * @param _layer_id The ID of the layer removed.
     *
     * @note Consider using {@link layersRemoved()} instead
     * @see layerWillBeRemoved()
     */
    void layerRemoved( const QString& _layer_id );

    /**
     * Emitted when all layers are removed, before {@link layersWillBeRemoved()} and
     * {@link layerWillBeRemoved()} signals are emitted. The layersWillBeRemoved() and
     * layerWillBeRemoved() signals will still be emitted following this signal.
     * You can use this signal to do easy (and fast) cleanup.
     */
    void removeAll();

    /**
     * Emitted when one or more layers were added to the registry.
     * This signal is also emitted for layers added to the registry,
     * but not to the legend.
     *
     * @param _map_layers List of layers which have been added.
     *
     * @see legendLayersAdded()
     * @see layerWasAdded()
     */
    void layersAdded( const QList<TDMLayer *>& _map_layers );

    /**
     * Emitted when a layer was added to the registry.
     *
     * @param _map_layer The ID of the layer which has been added.
     *
     * @note Consider using {@link layersAdded()} instead
     * @see layersAdded()
     */
    void layerWasAdded( TDMLayer* _map_layer );

//    /**
//     * Emitted, when a layer was added to the registry and the legend.
//     * Layers can also be private layers, which are signalled by
//     * {@link layersAdded()} and {@link layerWasAdded()} but will not be
//     * advertised by this signal.
//     *
//     * @param _map_layers List of {@link TDMLayer}s which were added to the legend.
//     */
//    void legendLayersAdded( const QList<TDMLayer*>& _map_layers );

  protected:
#if 0
    /** Debugging member
     *  invoked when a connect() is made to this object
     */
    void connectNotify( const char * signal ) override;
#endif

  private slots:
    void onMapLayerDeleted( QObject* _obj );

  private:
    //! private singleton constructor
    TDMLayerRegistry( QObject * _parent = nullptr );

    QMap<QString, TDMLayer*> m_map_layers;
};

#endif //TDMMAPLAYERREGISTRY_H

