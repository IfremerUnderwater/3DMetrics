#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Point>
#include <osg/ShapeDrawable>

#include "osg_axes.h"


#include "OSGWidget/osg_widget.h"
#include "OSGWidget/osg_widget_tool.h"
#include "Measurement/measurement_point.h"


OSGAxes::OSGAxes(QWidget *parent) :
    QWidget(parent)
{

    m_geode = new osg::Geode();


}

OSGAxes::~OSGAxes()
{
    // just in case
    slot_toolEndedAxes();
}

void OSGAxes::clicked()
{
    OSGWidgetTool *tool = OSGWidgetTool::instance();
    connect(tool, SIGNAL(signal_clicked(Point3D&)), this, SLOT(slot_toolClickedAxes(Point3D&)));
    connect(tool, SIGNAL(signal_endTool()), this, SLOT(slot_toolEndedAxes()));

    tool->startTool(OSGWidgetTool::Point);
}


void OSGAxes::slot_toolClickedAxes(Point3D &p)
{
    m_geode->removeDrawables(0, 4);

    // draw points
    setP(p);
    osg::Geometry* shape_point_drawable = new osg::Geometry();
    osg::Vec3Array* vertices = new osg::Vec3Array;
    osg::Vec3Array* verticesX = new osg::Vec3Array;
    osg::Vec3Array* verticesY = new osg::Vec3Array;
    osg::Vec3Array* verticesZ = new osg::Vec3Array;

    // origin
    osg::Vec3d point;
    point[0] = m_p.x;
    point[1] = m_p.y;
    point[2] = m_p.z;
    verticesX->push_back(point);
    verticesY->push_back(point);
    verticesZ->push_back(point);

    vertices->push_back(point);

    // x-axis
    osg::Vec3d point1;
    point1[0] = m_p.x +3;
    point1[1] = m_p.y;
    point1[2] = m_p.z;
    verticesX->push_back(point1);

    vertices->push_back(point1);

    // y-axis
    osg::Vec3d point2;
    point2[0] = m_p.x;
    point2[1] = m_p.y +3;
    point2[2] = m_p.z;
    verticesY->push_back(point2);

    vertices->push_back(point2);

    //z-axis
    osg::Vec3d point3;
    point3[0] = m_p.x;
    point3[1] = m_p.y;
    point3[2] = m_p.z +3;
    verticesZ->push_back(point3);

    vertices->push_back(point3);

    // pass the created vertex array to the points geometry object.
    shape_point_drawable->setVertexArray(vertices);
    osg::Vec4Array* colors = new osg::Vec4Array;
    // add a white color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
    osg::Vec4f colorGreen(0.0f,1.0f,0.0f,1.0f);
    osg::Vec4f colorBlue(0.0f,0.0f,1.0f,1.0f);
    osg::Vec4 colorRed(1.0f,1.0f,0.0f,1.0f);
    colors->push_back(m_colorGreen);

    // pass the color array to points geometry, note the binding to tell the geometry
    // that only use one color for the whole object.
    shape_point_drawable->setColorArray(colors, osg::Array::BIND_OVERALL);

    // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
    // parameter passed to the DrawArrays constructor is the Primitive::Mode which
    // in this case is POINTS (which has the same value GL_POINTS), the second
    // parameter is the index position into the vertex array of the first point
    // to draw, and the third parameter is the number of points to draw.
    shape_point_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertices->size()));
    shape_point_drawable->getOrCreateStateSet()->setAttribute(new osg::Point(4.f), osg::StateAttribute::ON);

    // lines
    // x-axis
    osg::Geometry* shape_line_drawableX = new osg::Geometry();
    shape_line_drawableX->setVertexArray(verticesX);
    osg::Vec4Array* colorslX = new osg::Vec4Array;
    colorslX->push_back(colorGreen);
    shape_line_drawableX->setColorArray(colorslX, osg::Array::BIND_OVERALL);
    shape_line_drawableX->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,verticesX->size()));

    // y-axis
    osg::Geometry* shape_line_drawableY = new osg::Geometry();
    shape_line_drawableY->setVertexArray(verticesY);
    osg::Vec4Array* colorslY = new osg::Vec4Array;
    colorslY->push_back(colorBlue);
    shape_line_drawableY->setColorArray(colorslY, osg::Array::BIND_OVERALL);
    shape_line_drawableY->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,verticesY->size()));

    //z-axis
    osg::Geometry* shape_line_drawableZ = new osg::Geometry();
    shape_line_drawableZ->setVertexArray(verticesZ);
    osg::Vec4Array* colorslZ = new osg::Vec4Array;
    colorslZ->push_back(colorRed);
    shape_line_drawableZ->setColorArray(colorslZ, osg::Array::BIND_OVERALL);
    shape_line_drawableZ->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,verticesZ->size()));


    m_geode->addDrawable(shape_line_drawableX);
    m_geode->addDrawable(shape_line_drawableY);
    m_geode->addDrawable(shape_line_drawableZ);
    m_geode->addDrawable(shape_point_drawable);
    //m_geode->addDrawable( new osg::ShapeDrawable( new osg::Cone(point1,1.0f, 2.0f) ) );
    // only one point for "point"
    slot_toolEndedAxes();
}

void OSGAxes::slot_toolEndedAxes()
{
    disconnect(OSGWidgetTool::instance(), 0, this, 0);
    OSGWidgetTool::instance()->endTool();


}

void OSGAxes::mouseDoubleClickEvent( QMouseEvent * e )
{
    if ( e->button() == Qt::LeftButton )
    {
        start();
    }
}
void OSGAxes::start()
{
    OSGWidgetTool::instance()->slot_cancelTool();
    clicked();
}

void OSGAxes::show(){
    OSGWidgetTool::instance()->getOSGWidget()->addGeode(m_geode);
}

void OSGAxes::removeAxe()
{
    slot_toolEndedAxes();
    m_geode->removeDrawables(0, 4);
    OSGWidgetTool::instance()->getOSGWidget()->removeGeode(m_geode);
    start();
}
