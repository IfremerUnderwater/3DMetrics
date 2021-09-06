#ifndef NODE_USER_DATA_H
#define NODE_USER_DATA_H

#include <osg/Referenced>

// models' user data
class NodeUserData : public osg::Referenced
{
public:
    NodeUserData() : Referenced() {}
    virtual ~NodeUserData() {}

    // values zmin and zmax from model (without offset)
    float zmin;
    float zmax;

    float zoffset;
    float originalZoffset;

    // use or not shader
    bool useShader;
    bool hasMesh;

    // points + generated triangles added
    bool composite;
    bool swappriorities;
};

#endif // NODE_USER_DATA_H
