#include "smartlod.h"
#include <QDebug>
#include <QDateTime>
#include <osg/CullStack>
#include <osgDB/ReadFile>

SmartLOD::SmartLOD() : osg::LOD(), m_lastIndex(-1)
{
}

SmartLOD::SmartLOD(const SmartLOD& plod,const osg::CopyOp& copyop):
    LOD(plod,copyop),
    m_databaseOptions(plod.m_databaseOptions),
    m_lastIndex(plod.m_lastIndex),
    m_perRangeDataList(plod.m_perRangeDataList)
{
}

SmartLOD::~SmartLOD()
{
}

SmartLOD::PerRangeData::PerRangeData() : _nodeLoaded(false), _doNotDiscard(false)
{
}

SmartLOD::PerRangeData::PerRangeData(const PerRangeData& prd):
    _filename(prd._filename)
{
}

SmartLOD::PerRangeData& SmartLOD::PerRangeData::operator = (const PerRangeData& prd)
{
    if (this==&prd) return *this;

    _filename = prd._filename;
    _nodeLoaded = prd._nodeLoaded;
    _doNotDiscard = prd._doNotDiscard;

    return *this;
}


void SmartLOD::traverse(osg::NodeVisitor& nv)
{
    switch(nv.getTraversalMode())
    {
    case(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN):
        std::for_each(_children.begin(),_children.end(),osg::NodeAcceptOp(nv));
        break;
    case(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN):
    {
        float required_range = 0;
        if (_rangeMode==DISTANCE_FROM_EYE_POINT)
        {
            required_range = nv.getDistanceToViewPoint(getCenter(),true);
        }
        else
        {
            osg::CullStack* cullStack = nv.asCullStack();
            if (cullStack && cullStack->getLODScale())
            {
                required_range = cullStack->clampedPixelSize(getBound()) / cullStack->getLODScale();
            }
            else
            {
                // fallback to selecting the highest res tile by
                // finding out the max range
                for(unsigned int i=0;i<_rangeList.size();++i)
                {
                    required_range = osg::maximum(required_range,_rangeList[i].first);
                }
            }
        }

        if(required_range == 0 && m_lastIndex >=0)
        {
            _children[m_lastIndex]->accept(nv);
            break;
        }

        //qDebug() << "required_range=" << required_range;

        unsigned int numChildren = _children.size();
        if (_rangeList.size()<numChildren) numChildren=_rangeList.size();

        bool discard = false;
        for(unsigned int i=0;i<numChildren;++i)
        {
            //qDebug() << "range " << i << "" << _rangeList[i].first << " < " << _rangeList[i].second;

            if (_rangeList[i].first<=required_range && required_range<_rangeList[i].second)
            {
                if( i != m_lastIndex)
                {
                    qDebug() << "required_range=" << required_range;
                    qDebug() << "Child=" << i << "/" << numChildren << " "
                             << m_perRangeDataList[i]._filename.c_str();

                    if(!m_perRangeDataList[i]._nodeLoaded)
                    {
                        QDateTime start = QDateTime::currentDateTime();

                        qDebug() << "Loading "  << m_perRangeDataList[i]._filename.c_str();
                        _children[i]=osgDB::readRefNodeFile(m_perRangeDataList[i]._filename.c_str(), new osgDB::Options(dynamic_cast<osgDB::Options*>(m_databaseOptions.get())->getOptionString()));
                        QDateTime current = QDateTime::currentDateTime();
                        uint msecs = start.time().msecsTo(current.time());
                        qDebug() << "Loaded in " << msecs << " ms";
                        m_perRangeDataList[i]._nodeLoaded = true;
                    }
                    m_lastIndex = i;
                    discard = true;
                }

                _children[i]->accept(nv);
            }
        }

        if(discard)
        {
            for(unsigned int i=0;i<numChildren;++i)
            {
                if(i != m_lastIndex && m_perRangeDataList[i]._nodeLoaded)
                {
                    // discard other nodes if not tagged
                    if(!m_perRangeDataList[i]._doNotDiscard)
                    {
                        qDebug() << "Discard " << m_perRangeDataList[i]._filename.c_str();
                        _children[i] = new osg::Node;
                        m_perRangeDataList[i]._nodeLoaded = false;
                    }
                    else
                    {
                        qDebug() << "Level " << i << " not discarded";
                    }
                }
            }
        }
        break;
    }
    default:
        break;
    }
}

void SmartLOD::expandPerRangeDataTo(unsigned int pos)
{
    if (pos>=m_perRangeDataList.size()) m_perRangeDataList.resize(pos+1);
}

bool SmartLOD::addChild( Node *child )
{
    if (LOD::addChild(child))
    {
        expandPerRangeDataTo(_children.size()-1);
        return true;
    }
    return false;
}

bool SmartLOD::addChild(Node *child, float min, float max)
{
    if (LOD::addChild(child,min,max))
    {
        expandPerRangeDataTo(_children.size()-1);
        m_lastIndex = _children.size()-1;
        m_perRangeDataList[m_lastIndex]._nodeLoaded = true;
        return true;
    }
    return false;
}

bool SmartLOD::addChild(const std::string& filename, float rmin, float rmax)
{
    if (LOD::addChild(new osg::Node,rmin,rmax))
    {
        expandPerRangeDataTo(_children.size()-1);
        setFileName(_children.size()-1, filename);
        return true;
    }
    return false;
}


bool SmartLOD::removeChildren( unsigned int pos,unsigned int numChildrenToRemove)
{
    if (pos<_rangeList.size()) _rangeList.erase(_rangeList.begin()+pos, osg::minimum(_rangeList.begin()+(pos+numChildrenToRemove), _rangeList.end()) );
    if (pos<m_perRangeDataList.size()) m_perRangeDataList.erase(m_perRangeDataList.begin()+pos, osg::minimum(m_perRangeDataList.begin()+ (pos+numChildrenToRemove), m_perRangeDataList.end()) );

    // just to be sure...
    m_lastIndex = -1;

    return Group::removeChildren(pos,numChildrenToRemove);
}

void SmartLOD::doNotDiscardChild(int _pos, bool _state)
{
    if(_pos < 0 || _pos >= _rangeList.size())
    {
        // error
        return;
    }
    m_perRangeDataList[_pos]._doNotDiscard = _state;
}
