
#ifndef TDMLAYER_H
#define TDMLAYER_H

#include <QDateTime>
#include <QObject>

/** \ingroup core
 * Base class for all layer types.
 * This is the base class for all layer types (3D model, measurement).
 */
class TDMLayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QString _name READ _name WRITE setName NOTIFY nameChanged )

  public:
    /** Layers enum defining the types of layers that can be added to 3D view */
    enum LayerType
    {
      ModelLayer,
      MeasurementLayer,
      PluginLayer
    };

    /** Constructor
     * @param type Type of layer as defined in TDMLayer::LayerType enum
     * @param layername Display Name of the layer
     * @param source datasource of layer
     */
    TDMLayer( TDMLayer::LayerType _type = ModelLayer, const QString& _layername = QString::null, const QString& _source = QString::null );

    /** Destructor */
    virtual ~TDMLayer();

    /** Get the type of the layer
     * @return Integer matching a value in the TDMLayer::LayerType enum
     */
    TDMLayer::LayerType type() const;

    /** Get this layer's unique ID, this ID is used to access this layer from tdm layer registry */
    QString id() const;

    /**
     * Set the display name of the layer
     * @param name New name for the layer

     */
    void setName( const QString& _name );

    /** Get the display name of the layer
     * @return the layer name
     */
    QString _name() const;

    /** Get the original name of the layer
     * @return the original layer name
     */
    QString originalName() const { return m_layer_orig_name; }

    /** Returns if this layer is read only. */
    bool readOnly() const { return isReadOnly(); }

    /** Return the extent of the layer */
    //virtual box extent();

    /**
     * Returns the sublayers of this layer
     * (Useful for providers that manage their own layers, such as WMS)
     */
    virtual QStringList subLayers() const;

    /** Returns layer's spatial reference system
     */
    //const QgsCoordinateReferenceSystem& crs() const;

    /** Sets layer's spatial reference system */
    //void setCrs( const QgsCoordinateReferenceSystem& srs, bool emitSignal = true );

    /** A convenience function to (un)capitalise the layer name */
    static QString capitaliseLayerName( const QString& _name );

    bool isEditable() const;

signals:

    /**
     * Emitted when the name has been changed
     *
     */
    void nameChanged();

    /** Emit a signal that layer's CRS has been reset */
    void layerCrsChanged();

    /** Data of layer changed */
    void dataChanged();

    /**
     * Signal emitted when legend of the layer has changed
     */
    void legendChanged();


  protected:

    /** Set the extent */
    //virtual void setExtent( const QgsRectangle &rect );

    /** Extent of the layer */
    //QgsRectangle mExtent;

    /** Data source description string, varies by layer type */
    QString m_data_source;

    /** Name of the layer - used for display */
    QString m_layer_name;

    /** Original name of the layer
     */
    QString m_layer_orig_name;

    QString m_title;

  private:
    /**
     * This method returns true by default but can be overwritten to specify
     * that a certain layer is writable.
     */
    virtual bool isReadOnly() const { return true; }

    /** Layer's spatial reference system.
        private to make sure setCrs must be used and layerCrsChanged() is emitted */
    //QgsCoordinateReferenceSystem mCRS;

    /** Private copy constructor - TDMLayer not copyable */
    TDMLayer( TDMLayer const & );

    /** Private assign operator - TDMLayer not copyable */
    TDMLayer & operator=( TDMLayer const & );

    /** Unique ID of this layer - used to refer to this layer in map layer registry */
    QString m_ID;

    /** Type of the layer (eg. vector, raster) */
    TDMLayer::LayerType m_layer_type;

};

Q_DECLARE_METATYPE( TDMLayer* )

#endif
