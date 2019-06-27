#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Point>
#include <osg/ShapeDrawable>

#include "osg_axes.h"


#include "OSGWidget/osg_widget.h"
#include "OSGWidget/osg_widget_tool.h"
#include "Measurement/measurement_point.h"


OSGAxes::OSGAxes(QWidget *_parent) :
    QWidget(_parent)
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


void OSGAxes::slot_toolClickedAxes(Point3D &_point)
{
    m_geode->removeDrawables(0, 4);

    // draw points
    setP(_point);
    osg::Geometry* shape_point_drawable = new osg::Geometry();
    osg::Vec3Array* vertices = new osg::Vec3Array;
    osg::Vec3Array* vertices_x = new osg::Vec3Array;
    osg::Vec3Array* vertices_y = new osg::Vec3Array;
    osg::Vec3Array* vertices_z = new osg::Vec3Array;

    // origin
    osg::Vec3d origin_point;
    origin_point[0] = m_point.x;
    origin_point[1] = m_point.y;
    origin_point[2] = m_point.z;
    vertices_x->push_back(origin_point);
    vertices_y->push_back(origin_point);
    vertices_z->push_back(origin_point);

    vertices->push_back(origin_point);

    // x-axis
    osg::Vec3d x_axis_point;
    x_axis_point[0] = m_point.x + m_scale;
    x_axis_point[1] = m_point.y;
    x_axis_point[2] = m_point.z;
    vertices_x->push_back(x_axis_point);

    vertices->push_back(x_axis_point);

    // y-axis
    osg::Vec3d y_axis_point;
    y_axis_point[0] = m_point.x;
    y_axis_point[1] = m_point.y + m_scale;
    y_axis_point[2] = m_point.z;
    vertices_y->push_back(y_axis_point);

    vertices->push_back(y_axis_point);

    //z-axis
    osg::Vec3d z_axis_point;
    z_axis_point[0] = m_point.x;
    z_axis_point[1] = m_point.y;
    z_axis_point[2] = m_point.z + m_scale;
    vertices_z->push_back(z_axis_point);

    vertices->push_back(z_axis_point);

    // pass the created vertex array to the points geometry object.
    shape_point_drawable->setVertexArray(vertices);
    osg::Vec4Array* colors = new osg::Vec4Array;
    // add a white color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
    osg::Vec4f green_color(0.0f,1.0f,0.0f,1.0f);
    osg::Vec4f blue_color(0.0f,0.0f,1.0f,1.0f);
    osg::Vec4 red_color(1.0f,0.0f,0.0f,1.0f);
    colors->push_back(green_color);

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
    osg::Geometry* shape_line_drawable_x = new osg::Geometry();
    shape_line_drawable_x->setVertexArray(vertices_x);
    osg::Vec4Array* x_axis_color = new osg::Vec4Array;
    x_axis_color->push_back(green_color);
    shape_line_drawable_x->setColorArray(x_axis_color, osg::Array::BIND_OVERALL);
    shape_line_drawable_x->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vertices_x->size()));

    // y-axis
    osg::Geometry* shape_line_drawable_y = new osg::Geometry();
    shape_line_drawable_y->setVertexArray(vertices_y);
    osg::Vec4Array* y_axis_color = new osg::Vec4Array;
    y_axis_color->push_back(blue_color);
    shape_line_drawable_y->setColorArray(y_axis_color, osg::Array::BIND_OVERALL);
    shape_line_drawable_y->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vertices_y->size()));

    //z-axis
    osg::Geometry* shape_line_drawable_z = new osg::Geometry();
    shape_line_drawable_z->setVertexArray(vertices_z);
    osg::Vec4Array* z_axis_color = new osg::Vec4Array;
    z_axis_color->push_back(red_color);
    shape_line_drawable_z->setColorArray(z_axis_color, osg::Array::BIND_OVERALL);
    shape_line_drawable_z->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vertices_z->size()));


    m_geode->addDrawable(shape_line_drawable_x);
    m_geode->addDrawable(shape_line_drawable_y);
    m_geode->addDrawable(shape_line_drawable_z);
    m_geode->addDrawable(shape_point_drawable);
    // only one point for "point"
    slot_toolEndedAxes();
}

void OSGAxes::slot_toolEndedAxes()
{
    disconnect(OSGWidgetTool::instance(), 0, this, 0);
    OSGWidgetTool::instance()->endTool();


}

void OSGAxes::mouseDoubleClickEvent( QMouseEvent * _e )
{
    if ( _e->button() == Qt::LeftButton )
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
