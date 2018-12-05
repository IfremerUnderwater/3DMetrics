#ifndef TDMMEASURELAYERDATA_H
#define TDMMEASURELAYERDATA_H

#include <osg/ref_ptr>
#include <osg/Group>

#include <QString>
#include <QMetaType>

//#include "tool_handler.h"

#include "Measurement/measurement_pattern.h"
#include "Measurement/osg_measurement_row.h"

// private data used in Measure Layer
// usable in QVariant
// stored in hidden colums in the treeitem
//
class TDMMeasurementLayerData
{
public:
    TDMMeasurementLayerData();
    TDMMeasurementLayerData(const QString _fileName, MeasurePattern _pattern, const osg::ref_ptr<osg::Group> _group);
    TDMMeasurementLayerData(const TDMMeasurementLayerData &_other);
    ~TDMMeasurementLayerData();

    QString fileName() const { return m_file_name; }
    void setFileName(QString _name) { m_file_name = _name; }

    MeasurePattern & pattern() { return m_pattern; }

    osg::ref_ptr<osg::Group> group() const { return m_group; }

    std::vector<osgMeasurementRow*> &rows() { return m_rows; }
    void deleteRow(int row);
    void addRow(osgMeasurementRow* _row, int row);

private:
    QString m_file_name; // may be empty when just created
    MeasurePattern m_pattern;
    osg::ref_ptr<osg::Group> m_group;
    std::vector<osgMeasurementRow*> m_rows; // NB : to be deleted when removed from OSGWidget only
};

// needed for using with QVariant
Q_DECLARE_METATYPE(TDMMeasurementLayerData)

#endif // TDMMEASURELAYERDATA_H
