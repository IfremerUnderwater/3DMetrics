#include "slope_tool.h"
#include "ui_slope_tool.h"

#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Point>

#include "OSGWidget/osg_widget.h"
#include "OSGWidget/osg_widget_tool.h"

SlopeTool::SlopeTool(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SlopeTool), centerSet(false), m_centerX(0), m_centerY(0), m_centerZ(0)
{ 
    ui->setupUi(this);
    setWindowFlags(Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    m_geode = new osg::Geode();

    osg::Vec4f color(0.0f,1.0f,1.0f,1.0f); // cyan (r,g,b,a)
    m_color = color;

    //ui->point_widget->setPoint(m_meas_point,false);
    OSGWidgetTool::instance()->getOSGWidget()->addGeode(m_geode);

    QObject::connect(ui->close_btn, SIGNAL(clicked(bool)), this, SLOT(close()));
    QObject::connect(ui->start_btn, SIGNAL(clicked(bool)), this, SLOT(start()));

}

SlopeTool::~SlopeTool()
{
    delete ui;
}


void SlopeTool::mouseDoubleClickEvent( QMouseEvent * _e )
{
    if ( _e->button() == Qt::LeftButton )
    {
        start();
    }
}

void SlopeTool::closeEvent(QCloseEvent *)
{
    slot_toolEnded();
    OSGWidgetTool::instance()->getOSGWidget()->removeGeode(m_geode);
}

// ESC key hit
void SlopeTool::reject()
{
    slot_toolEnded();
    OSGWidgetTool::instance()->getOSGWidget()->removeGeode(m_geode);
    close();
}

void SlopeTool::start()
{
    OSGWidgetTool::instance()->slot_cancelTool();

    // start tool
    QString msg = "Slope tool started";
    emit signal_toolStarted(msg);

    centerSet = false;

    OSGWidgetTool *tool = OSGWidgetTool::instance();
    connect(tool, SIGNAL(signal_clicked(Point3D&)), this, SLOT(slot_toolClicked(Point3D&)));
    connect(tool, SIGNAL(signal_endTool()), this, SLOT(slot_toolEnded()));

    tool->startTool(OSGWidgetTool::Slope);
}

void SlopeTool::slot_toolEnded()
{
    centerSet = false;

    disconnect(OSGWidgetTool::instance(), 0, this, 0);
    OSGWidgetTool::instance()->endTool();

    QString msg = "Slope tool ended";
    emit signal_toolEnded(msg);
}

 void SlopeTool::slot_toolClicked(Point3D& _p)
 {
     m_geode->removeDrawables(0, 2);

     // first clic : point
     if(!centerSet)
     {
         centerSet = true;

         m_centerX = _p.x;
         m_centerY = _p.y;
         m_centerZ = _p.z;;
     }

     // draw center
     {
         osg::Vec3d point;
         point[0] = m_centerX;
         point[1] = m_centerY;
         point[2] = m_centerZ;
         // create point in geode
         // point
         osg::Geometry* shape_point_drawable = new osg::Geometry();
         osg::Vec3Array* vertices = new osg::Vec3Array;
         vertices->push_back(point);

         // pass the created vertex array to the points geometry object.
         shape_point_drawable->setVertexArray(vertices);

         osg::Vec4Array* colors = new osg::Vec4Array;
         // add a white color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
         //osg::Vec4 color(0.0f,0.0f,1.0f,1.0f);
         colors->push_back(m_color);

         // pass the color array to points geometry, note the binding to tell the geometry
         // that only use one color for the whole object.
         shape_point_drawable->setColorArray(colors, osg::Array::BIND_OVERALL);

         // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
         // parameter passed to the DrawArrays constructor is the Primitive::Mode which
         // in this case is POINTS (which has the same value GL_POINTS), the second
         // parameter is the index position into the vertex array of the first point
         // to draw, and the third parameter is the number of points to draw.
         shape_point_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS,0,vertices->size()));

         // fixed size points
         shape_point_drawable->getOrCreateStateSet()->setAttribute(new osg::Point(10.f), osg::StateAttribute::ON);

         m_geode->addDrawable(shape_point_drawable);
     }

     // second clic : area
     {
         //draw points
         osg::Vec3Array* vertices = new osg::Vec3Array;
         osg::Vec3d point;
         // center
         point[0] = m_centerX;
         point[1] = m_centerY;
         point[2] = m_centerZ;
         vertices->push_back(point);

         point[0] = _p.x;
         point[1] = _p.y;
         point[2] = _p.z;
         vertices->push_back(point);

         // line
         osg::Geometry* shape_line_drawable = new osg::Geometry();

         // pass the created vertex array to the points geometry object.
         shape_line_drawable->setVertexArray(vertices);

         osg::Vec4Array* colorsl = new osg::Vec4Array;
         // add a green color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
         //osg::Vec4 colorl(0.0f,1.0f,0.0f,1.0f);
         colorsl->push_back(m_color);

         // pass the color array to points geometry, note the binding to tell the geometry
         // that only use one color for the whole object.
         shape_line_drawable->setColorArray(colorsl, osg::Array::BIND_OVERALL);

         // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
         // parameter passed to the DrawArrays constructor is the Primitive::Mode which
         // in this case is POINTS (which has the same value GL_POINTS), the second
         // parameter is the index position into the vertex array of the first point
         // to draw, and the third parameter is the number of points to draw.
         shape_line_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vertices->size()));

         m_geode->addDrawable(shape_line_drawable);
     }
 }
