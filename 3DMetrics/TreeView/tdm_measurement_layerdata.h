#ifndef TDM_MEASUREMENT_LAYER_DATA_H
#define TDM_MEASUREMENT_LAYER_DATA_H

#include <osg/ref_ptr>
#include <osg/Group>

#include <QString>
#include <QMetaType>

//#include "tool_handler.h"

#include "Measurement/measurement_pattern.h"
#include "Measurement/osg_measurement_row.h"

// private data used in Meas Layer
// usable in QVariant
// stored in hidden colums in the treeitem
//
class TDMMeasurementLayerData
{
public:
    TDMMeasurementLayerData();
    TDMMeasurementLayerData(const QString _filename, MeasPattern _pattern, const osg::ref_ptr<osg::Group> _group);
    TDMMeasurementLayerData(const TDMMeasurementLayerData &_other);
    ~TDMMeasurementLayerData();

    QString fileName() const { return m_filename; }
    void setFileName(QString _name) { m_filename = _name; }

    MeasPattern & pattern() { return m_pattern; }

    osg::ref_ptr<osg::Group> group() const { return m_group; }

    std::vector<osgMeasurementRow*> &rows() { return m_rows; }
    void deleteRow(int row);
    void addRow(osgMeasurementRow* _row, int row);

private:
    QString m_filename; // may be empty when just created
    MeasPattern m_pattern;
    osg::ref_ptr<osg::Group> m_group;
    std::vector<osgMeasurementRow*> m_rows; // NB : to be deleted when removed from OSGWidget only
};

// needed for using with QVariant
Q_DECLARE_METATYPE(TDMMeasurementLayerData)

#endif // TDM_MEASUREMENT_LAYER_DATA_H
