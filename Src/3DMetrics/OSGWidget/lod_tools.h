#ifndef LODTOOLS_H
#define LODTOOLS_H

#include <osg/Node>
#include <osg/Referenced>
#include <osg/LOD>

class LODTools
{
public:

    ///
    /// \brief hasLODInTree
    /// \param _node
    /// \return
    ///
    static bool hasLODInTree(osg::Node *_node);

    ///
    /// \brief getFirstLODNode
    /// \param _node
    /// \return
    ///
    static osg::ref_ptr<osg::LOD> getFirstLODNode(osg::Node *_node);

    ///
    /// \brief countLODInTree
    /// \param _node
    /// \return
    ///
    static int countLODInTree(osg::Node *_node);

    ///
    /// \brief applyLODValuesInTree
    /// \param _node
    /// \param _threshold1
    /// \param _threshold2
    /// \return
    ///
    static int applyLODValuesInTree(osg::Node *_node, float _threshold1, float _threshold2);
};

#endif // LODTOOLS_H
