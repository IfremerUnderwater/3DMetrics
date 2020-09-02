#include "lod_tools.h"

///
/// \brief LODTools::hasLODInTree
/// \param _node
/// \return
///
bool LODTools::hasLODInTree(osg::Node *_node)
{
    osg::ref_ptr<osg::LOD> lod = dynamic_cast<osg::LOD*>(_node);
    if(lod.valid())
    {
        // can modify LOD
        return true;
    }
    osg::ref_ptr<osg::Group> group = _node->asGroup();
    if(!group.valid())
    {
        return false;
    }
    for(int i=0; i<group->getNumChildren(); i++)
    {
        if(hasLODInTree(group->getChild(i)))
            return true;
    }
    return false;
}

///
/// \brief LODTools::getFirstLODNode
/// \param _node
/// \return
///
osg::ref_ptr<osg::LOD> LODTools::getFirstLODNode(osg::Node *_node)
{
    osg::ref_ptr<osg::LOD> lod = dynamic_cast<osg::LOD*>(_node);
    if(lod.valid())
    {
        // can modify LOD
        return lod;
    }
    osg::ref_ptr<osg::Group> group = _node->asGroup();
    if(!group.valid())
    {
        return lod;
    }
    for(int i=0; i<group->getNumChildren(); i++)
    {
        lod = getFirstLODNode(group->getChild(i));
        if(lod.valid())
            return lod;
    }
    return lod;
}

///
/// \brief LODTools::countLODInTree
/// \param _node
/// \return
///
int LODTools::countLODInTree(osg::Node *_node)
{

    osg::ref_ptr<osg::LOD> lod = dynamic_cast<osg::LOD*>(_node);
    if(lod.valid())
    {
        return 1;
    }
    osg::ref_ptr<osg::Group> group = _node->asGroup();
    if(!group.valid())
    {
        return 0;
    }
    int count = 0;
    for(int i=0; i<group->getNumChildren(); i++)
    {
        count += countLODInTree(group->getChild(i));
    }
    return count;
}

///
/// \brief LODTools::applyLODValuesInTree
/// \param _node
/// \param _threshold1
/// \param _threshold2
/// \return
///
int LODTools::applyLODValuesInTree( osg::Node *_node, float _threshold1, float _threshold2)
{

    osg::ref_ptr<osg::LOD> lod = dynamic_cast<osg::LOD*>(_node);
    if(lod.valid())
    {
        if(!lod->getNumChildren() == 3)
        {
            return 0;
        }
        lod->setRange(0, 0.0f, _threshold1);
        lod->setRange(1, _threshold1, _threshold2);
        lod->setRange(2, _threshold2, FLT_MAX);

        return 1;
    }
    osg::ref_ptr<osg::Group> group = _node->asGroup();
    if(!group.valid())
    {
        return 0;
    }
    int count = 0;
    for(int i=0; i<group->getNumChildren(); i++)
    {
        count += applyLODValuesInTree(group->getChild(i), _threshold1, _threshold2);
    }
    return count;
}
