#ifndef DELAUNAYTRIANGULATORNOSORT_H
#define DELAUNAYTRIANGULATORNOSORT_H

#include <osgUtil/DelaunayTriangulator>

namespace osg3DMETRICS
{


class DelaunayTriangulatorNosort: public osg::Referenced {
public:

    DelaunayTriangulatorNosort();
    explicit DelaunayTriangulatorNosort(osg::Vec3Array *points, osg::Vec3Array *normals = 0);
    DelaunayTriangulatorNosort(const DelaunayTriangulatorNosort &copy, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY);

    typedef std::vector< osg::ref_ptr<osgUtil::DelaunayConstraint> > linelist;

    /** Set the input point array. */
    inline void setInputPointArray(osg::Vec3Array* points) { points_ = points; }

    /** Get the const input point array. */
    inline const osg::Vec3Array* getInputPointArray() const {  return points_.get(); }

    /** Get the input point array. */
    inline osg::Vec3Array* getInputPointArray() {  return points_.get(); }


    /** Set the output normal array (optional). */
    inline void setOutputNormalArray(osg::Vec3Array* normals) { normals_ = normals; }

    /** Get the const output normal array (optional). */
    inline const osg::Vec3Array *getOutputNormalArray() const { return normals_.get(); }

    /** Get the output normal array (optional). */
    inline osg::Vec3Array *getOutputNormalArray() { return normals_.get(); }


    /** Add an input constraint loop.
     ** the edges of the loop will constrain the triangulation.
     ** if remove!=0, the internal triangles of the constraint will be removed;
     ** the user may the replace the constraint line with an equivalent geometry.
     ** GWM July 2005 */
    void addInputConstraint(osgUtil::DelaunayConstraint *dc) { constraint_lines.push_back(dc); }


    /** Start triangulation. */
    bool triangulate();

    /** Get the generated primitive (call triangulate() first). */
    inline const osg::DrawElementsUInt *getTriangles() const { return prim_tris_.get(); }

    /** Get the generated primitive (call triangulate() first). */
    inline osg::DrawElementsUInt *getTriangles() { return prim_tris_.get(); }

    /** remove the triangles internal to the constraint loops.
     * (Line strips cannot remove any internal triangles). */
    void removeInternalTriangles(osgUtil::DelaunayConstraint *constraint);


protected:
    virtual ~DelaunayTriangulatorNosort();
    DelaunayTriangulatorNosort &operator=(const DelaunayTriangulatorNosort &) { return *this; }
    int getindex(const osg::Vec3 &pt,const osg::Vec3Array *points);

private:
    osg::ref_ptr<osg::Vec3Array> points_;
    osg::ref_ptr<osg::Vec3Array> normals_;
    osg::ref_ptr<osg::DrawElementsUInt> prim_tris_;

    // GWM these lines provide required edges in the triangulated shape.
    linelist constraint_lines;

    void _uniqueifyPoints();
};

// INLINE METHODS



}


#endif // DELAUNAYTRIANGULATORNOSORT_H
