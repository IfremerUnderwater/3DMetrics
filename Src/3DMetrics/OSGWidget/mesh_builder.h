#ifndef MESHBUILDER_H
#define MESHBUILDER_H

#include <osg/Node>
#include <osg/Geometry>

class MeshBuilder
{
public:
    MeshBuilder(osg::ref_ptr<osg::Node> _node);

    bool hasPointsAndNotMesh();

    bool Triangulate();

    bool saveGeneratedMesh(std::string _fileName);

    void setThreshold(float threshold);

    void setThresholdx(float thresholdx);
    void setThresholdy(float thresholdy);
    void setThresholdz(float thresholdz);

private:
    osg::ref_ptr<osg::Node> m_node;
    osg::ref_ptr<osg::Geometry> m_generatedGeometry;

    // thresholds
    // on mean triangle size (squares edge sizes)
    float m_threshold;

    // on mean absolute delta
    float m_thresholdx;
    float m_thresholdy;
    float m_thresholdz;
};

#endif // MESHBUILDER_H
