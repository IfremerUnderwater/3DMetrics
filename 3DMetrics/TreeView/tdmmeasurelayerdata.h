#ifndef TDMMEASURELAYERDATA_H
#define TDMMEASURELAYERDATA_H

#include <osg/ref_ptr>
#include <osg/Group>

#include <QString>
#include <QMetaType>

//#include "tool_handler.h"

#include "Measure/measurepattern.h"
#include "Measure/osgmeasurerow.h"

// private data used in Measure Layer
// usable in QVariant
// stored in hidden colums in the treeitem
//
class TDMMeasureLayerData
{
public:
    TDMMeasureLayerData();
    TDMMeasureLayerData(const QString _fileName, MeasurePattern _pattern, const osg::ref_ptr<osg::Group> _group);
    TDMMeasureLayerData(const TDMMeasureLayerData &_other);
    ~TDMMeasureLayerData();

    QString fileName() const { return m_file_name; }
    void setFileName(QString _name) { m_file_name = _name; }

    MeasurePattern & pattern() { return m_pattern; }

    osg::ref_ptr<osg::Group> group() const { return m_group; }

    std::vector<osgMeasureRow*> &rows() { return m_rows; }
    void deleteRow(int row);
    void addRow(osgMeasureRow* _row, int row);

private:
    QString m_file_name; // may be empty when just created
    MeasurePattern m_pattern;
    osg::ref_ptr<osg::Group> m_group;
    std::vector<osgMeasureRow*> m_rows; // NB : to be deleted when removed from OSGWidget only
};

// needed for using with QVariant
Q_DECLARE_METATYPE(TDMMeasureLayerData)

#endif // TDMMEASURELAYERDATA_H
