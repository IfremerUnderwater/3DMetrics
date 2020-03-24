#include "slope_tool.h"
#include "ui_slope_tool.h"

#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Point>
#include <osg/LineWidth>

#include "OSGWidget/osg_widget.h"
#include "OSGWidget/osg_widget_tool.h"

SlopeTool::SlopeTool(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SlopeTool), m_centerSet(false), m_xcenter(0), m_ycenter(0)
{ 
    m_center = {0,0,0};

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
    m_geode->removeDrawables(0, 3);

    OSGWidgetTool::instance()->slot_cancelTool();


    // start tool
    QString msg = "Slope tool started";
    emit signal_toolStarted(msg);

    m_centerSet = false;

    OSGWidgetTool *tool = OSGWidgetTool::instance();
    connect(tool, SIGNAL(signal_clickedXY(Point3D&, int, int)), this, SLOT(slot_toolClickedXY(Point3D&, int, int)));
    connect(tool, SIGNAL(signal_endTool()), this, SLOT(slot_toolEnded()));
    connect(tool, SIGNAL(signal_cancelTool()), this, SLOT(slot_toolCanceled()));
    connect(tool, SIGNAL(signal_removeLastPointTool()), this, SLOT(slot_toolRemoveLastPoint()));

    //connect(tool->getOSGWidget(), SIGNAL(signal_onMouseMove(int, int)), this, SLOT(slot_mouse_moved(int, int)));

    tool->startTool(OSGWidgetTool::Slope);
}

void SlopeTool::slot_toolCanceled()
{
    m_geode->removeDrawables(0, 3);
    m_centerSet = false;
}

void SlopeTool::slot_toolRemoveLastPoint()
{
    if(m_centerSet)
    {
        m_geode->removeDrawables(1, 2);
    }
}

void SlopeTool::slot_toolEnded()
{
    m_centerSet = false;

    disconnect(OSGWidgetTool::instance(), 0, this, 0);
    disconnect(OSGWidgetTool::instance()->getOSGWidget(), 0, this, 0);

    OSGWidgetTool::instance()->endTool();

    QString msg = "Slope tool ended";
    emit signal_toolEnded(msg);
}

void SlopeTool::slot_toolClicked(Point3D& _p)
{
    osg::Vec3d eye, center, up;
    OSGWidgetTool::instance()->getOSGWidget()->getCamera()->getViewMatrixAsLookAt(eye, center, up);
    //osg::Vec3d lookDir = center - eye;
    osg::Vec3d lookDir = eye - center; //center - -eye;
    lookDir.normalize();
    //    osg::Vec3d side = lookDir ^up;
    //    side.normalize();

    //m_geode->removeDrawables(0, 3);
    m_geode->removeDrawables(0, 4);

    // first clic : point
    if(!m_centerSet)
    {
        m_center[0] = _p.x;
        m_center[1] = _p.y;
        m_center[2] = _p.z;
    }

    // draw center
    {
        // create point in geode
        // point
        osg::Geometry* shape_point_drawable = new osg::Geometry();
        osg::Vec3Array* vertices = new osg::Vec3Array;
        vertices->push_back(m_center);

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

    // second clic : line (RADIUS)
    if(m_centerSet)
    {
        //draw points
        osg::Vec3Array* vertices = new osg::Vec3Array;
        osg::Vec3d point;
        // center
        vertices->push_back( m_center );

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

        shape_line_drawable->getOrCreateStateSet()->setAttribute(new osg::LineWidth(1.f), osg::StateAttribute::ON);

        // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
        // parameter passed to the DrawArrays constructor is the Primitive::Mode which
        // in this case is POINTS (which has the same value GL_POINTS), the second
        // parameter is the index position into the vertex array of the first point
        // to draw, and the third parameter is the number of points to draw.
        shape_line_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vertices->size()));

        m_geode->addDrawable(shape_line_drawable);
    }

    // area
    if(m_centerSet)
    {
        //        osg::Camera * cam = OSGWidgetTool::instance()->getOSGWidget()->getCamera();
        //        osg::Matrix viewMatrix = cam->getViewMatrix();
        //        osg::Matrix projectionMatrix = cam->getProjectionMatrix();
        //        osg::Matrix windowMatrix = cam->getViewport()->computeWindowMatrix();
        //        osg::Vec4d refPoint;
        //        refPoint[0] = m_center[0];
        //        refPoint[1] = m_center[1];
        //        refPoint[2] = m_center[2];
        //        refPoint[3] = 1.0;
        //        osg::Vec4d screenCoordsCenter = windowMatrix.postMult (projectionMatrix.postMult (viewMatrix.postMult (refPoint)));
        //        screenCoordsCenter.normalize();
        //        refPoint[0] = _p.x;
        //        refPoint[1] = _p.y;
        //        refPoint[2] = _p.z;
        //        refPoint[3] = 1.0;
        //        osg::Vec4d screenCoordsP = windowMatrix.postMult (projectionMatrix.postMult (viewMatrix.postMult (refPoint)));
        //        screenCoordsP.normalize();

        //        // add circle = polygon with 64 sides
        //        double radius = hypot( ( screenCoordsP[0] - screenCoordsCenter[0]), (screenCoordsP[1] - screenCoordsCenter[1]));
        //        osg::Vec3Array* circle = new osg::Vec3Array;
        //        for(int i=0; i<64; i++)
        //        {
        //            bool exist;
        //            double x = screenCoordsCenter[0] + radius* sin(i*2*M_PI / 64);
        //            double y = screenCoordsCenter[1] + radius* sin(i*2*M_PI / 64);
        //            osg::Vec3d inter;
        //            OSGWidgetTool::instance()->getOSGWidget()->getIntersectionPoint(x, y, inter, exist);
        //            if(exist)
        //            {
        //                circle->push_back(inter);
        //            }
        //        }

        //        // add circle = polygon with 64 sides
        //        double radius = hypot( ( _p.x - m_center[0]), ( _p.y - m_center[1]));
        //        osg::Vec3Array* circle = new osg::Vec3Array;
        //        for(int i=0; i<64; i++)
        //        {
        //            point[0] = m_center[0] +  radius* sin(i*2*M_PI / 64);
        //            point[1] = m_center[1] +  radius* cos(i*2*M_PI / 64);;
        //            point[2] = _p.z; // TODO ???
        //            circle->push_back(point);
        //        }


        // normal direction : lookDir
        osg::Vec3d point;
        point[0] = _p.x;
        point[1] = _p.y;
        point[2] = _p.z;

        osg::Vec3d delta = point - m_center;
        double d = delta[0]*lookDir[0] + delta[1]*lookDir[1] + delta[2]*lookDir[2];
        osg::Vec3d center;
        //        osg::Vec3d deltaN = delta;
        //        deltaN.normalize();
        center[0] = lookDir[0]*d + m_center[0];
        center[1] = lookDir[1]*d + m_center[1];
        center[2] = lookDir[2]*d + m_center[2];

        // test draw center
        {
            osg::Geometry* shape_point_drawable = new osg::Geometry();
            osg::Vec3Array* vertices = new osg::Vec3Array;
            vertices->push_back(center);

            // pass the created vertex array to the points geometry object.
            shape_point_drawable->setVertexArray(vertices);

            osg::Vec4Array* colors = new osg::Vec4Array;
            // add a white color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
            osg::Vec4 color(1.0f,0.0f,0.0f,1.0f);
            colors->push_back(color);

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
            shape_point_drawable->getOrCreateStateSet()->setAttribute(new osg::Point(5.f), osg::StateAttribute::ON);

            m_geode->addDrawable(shape_point_drawable);
        }


        osg::Vec3d ds = delta ^ lookDir;

        // add circle = polygon with 64 sides
        double radius = sqrt( ds[0] * ds[0] + ds[1] * ds[1] );
        osg::Vec3Array* circle = new osg::Vec3Array;
        for(int i=0; i<64; i++)
        {
            point[0] = radius* sin(i*2*M_PI / 64);
            point[1] = radius* cos(i*2*M_PI / 64);;
            point[2] = 0;
            point = lookDir ^ point;
            point = point + center;

            circle->push_back(point);
        }

        // line
        osg::Geometry* shape_circle_drawable = new osg::Geometry();

        // pass the created vertex array to the points geometry object.
        shape_circle_drawable->setVertexArray(circle);

        osg::Vec4Array* colorsl = new osg::Vec4Array;
        // add a red color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
        //osg::Vec4 colorl(1.0f,0.0f,0.0f,1.0f);
        colorsl->push_back(m_color);

        // pass the color array to points geometry, note the binding to tell the geometry
        // that only use one color for the whole object.
        shape_circle_drawable->setColorArray(colorsl, osg::Array::BIND_OVERALL);

        shape_circle_drawable->getOrCreateStateSet()->setAttribute(new osg::LineWidth(3.f), osg::StateAttribute::ON);

        // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
        // parameter passed to the DrawArrays constructor is the Primitive::Mode which
        // in this case is POINTS (which has the same value GL_POINTS), the second
        // parameter is the index position into the vertex array of the first point
        // to draw, and the third parameter is the number of points to draw.
        shape_circle_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,circle->size()));

        m_geode->addDrawable(shape_circle_drawable);
    }

    m_centerSet = true;
}


void SlopeTool::slot_toolClickedXY(Point3D& _p, int _x, int _y)
{
    m_geode->removeDrawables(0, 3);

    // first clic : point
    if(!m_centerSet)
    {
        m_center[0] = _p.x;
        m_center[1] = _p.y;
        m_center[2] = _p.z;
        m_xcenter = _x;
        m_ycenter = _y;
    }

    // draw center
    {
        // create point in geode
        // point
        osg::Geometry* shape_point_drawable = new osg::Geometry();
        osg::Vec3Array* vertices = new osg::Vec3Array;
        vertices->push_back(m_center);

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
        shape_point_drawable->getOrCreateStateSet()->setAttribute(new osg::Point(8.f), osg::StateAttribute::ON);

        m_geode->addDrawable(shape_point_drawable);
    }

    //    // second clic : line (RADIUS) in 3D
    //    if(m_centerSet)
    //    {
    //        //draw points
    //        osg::Vec3Array* vertices = new osg::Vec3Array;
    //        osg::Vec3d point;
    //        // center
    //        vertices->push_back( m_center );

    //        point[0] = _p.x;
    //        point[1] = _p.y;
    //        point[2] = _p.z;
    //        vertices->push_back(point);

    //        // line
    //        osg::Geometry* shape_line_drawable = new osg::Geometry();

    //        // pass the created vertex array to the points geometry object.
    //        shape_line_drawable->setVertexArray(vertices);

    //        osg::Vec4Array* colorsl = new osg::Vec4Array;
    //        // add a green color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
    //        //osg::Vec4 colorl(0.0f,1.0f,0.0f,1.0f);
    //        colorsl->push_back(m_color);

    //        // pass the color array to points geometry, note the binding to tell the geometry
    //        // that only use one color for the whole object.
    //        shape_line_drawable->setColorArray(colorsl, osg::Array::BIND_OVERALL);

    //        shape_line_drawable->getOrCreateStateSet()->setAttribute(new osg::LineWidth(1.f), osg::StateAttribute::ON);

    //        // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
    //        // parameter passed to the DrawArrays constructor is the Primitive::Mode which
    //        // in this case is POINTS (which has the same value GL_POINTS), the second
    //        // parameter is the index position into the vertex array of the first point
    //        // to draw, and the third parameter is the number of points to draw.
    //        shape_line_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,0,vertices->size()));

    //        m_geode->addDrawable(shape_line_drawable);
    //    }

    // area
    if(m_centerSet)
    {
        double radius = hypot(_x - m_xcenter, _y - m_ycenter);

        // add circle = polygon with 64 sides
        osg::Vec3Array* circle = new osg::Vec3Array;
        for(int i=0; i<64; i++)
        {
            int px = m_xcenter + radius* sin(i*2*M_PI / 64);
            int py = m_ycenter + radius* cos(i*2*M_PI / 64);;

            bool exists = false;
            osg::Vec3d vect;
            OSGWidgetTool::instance()->getOSGWidget()->getIntersectionPoint(px, py, vect, exists);

            if(exists)
            {
                circle->push_back(vect);
            }
        }

        // line
        osg::Geometry* shape_circle_drawable = new osg::Geometry();

        // pass the created vertex array to the points geometry object.
        shape_circle_drawable->setVertexArray(circle);

        osg::Vec4f pcolor = m_color;
        pcolor[3] = 0.25f;
        // add a red color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
        //osg::Vec4 colorl(1.0f,0.0f,0.0f,1.0f);
        osg::Vec4Array* colorsl = new osg::Vec4Array;
        colorsl->push_back(pcolor);

        // pass the color array to points geometry, note the binding to tell the geometry
        // that only use one color for the whole object.
        shape_circle_drawable->setColorArray(colorsl, osg::Array::BIND_OVERALL);

        shape_circle_drawable->getOrCreateStateSet()->setAttribute(new osg::LineWidth(3.f), osg::StateAttribute::ON);

        // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
        // parameter passed to the DrawArrays constructor is the Primitive::Mode which
        // in this case is POINTS (which has the same value GL_POINTS), the second
        // parameter is the index position into the vertex array of the first point
        // to draw, and the third parameter is the number of points to draw.
        shape_circle_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,circle->size()));

        m_geode->addDrawable(shape_circle_drawable);

        // add border polyline
        shape_circle_drawable = new osg::Geometry();
        shape_circle_drawable->setVertexArray(circle);
        colorsl = new osg::Vec4Array;
        colorsl->push_back(m_color);
        shape_circle_drawable->setColorArray(colorsl, osg::Array::BIND_OVERALL);
        shape_circle_drawable->getOrCreateStateSet()->setAttribute(new osg::LineWidth(2.f), osg::StateAttribute::ON);
        shape_circle_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,circle->size()));
        m_geode->addDrawable(shape_circle_drawable);
    }

    //    if(m_centerSet)
    //    {
    //        OSGWidgetTool::instance()->endTool();
    //    }
    m_centerSet = true;
}

// not used : too slow
void SlopeTool::slot_mouse_moved(int _x, int _y)
{
    if(!m_centerSet)
        return;

    m_geode->removeDrawables(1, 2);

    osg::Matrix matrix = OSGWidgetTool::instance()->getOSGWidget()->getCamera()->getProjectionMatrix();

    // area
    if(m_centerSet)
    {
        double radius = hypot(_x - m_xcenter, _y - m_ycenter);

        // add circle = polygon with 64 sides
        osg::Vec3Array* circle = new osg::Vec3Array;
        for(int i=0; i<64; i++)
        {
            int px = m_xcenter + radius* sin(i*2*M_PI / 64);
            int py = m_ycenter + radius* cos(i*2*M_PI / 64);;

            bool exists = false;
            osg::Vec3d vect;
            OSGWidgetTool::instance()->getOSGWidget()->getIntersectionPoint(px, py, vect, exists);

            if(exists)
            {
                circle->push_back(vect);
            }
        }

        // line
        osg::Geometry* shape_circle_drawable = new osg::Geometry();

        // pass the created vertex array to the points geometry object.
        shape_circle_drawable->setVertexArray(circle);

        osg::Vec4f pcolor = m_color;
        pcolor[3] = 0.25f;
        // add a red color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
        //osg::Vec4 colorl(1.0f,0.0f,0.0f,1.0f);
        osg::Vec4Array* colorsl = new osg::Vec4Array;
        colorsl->push_back(pcolor);

        // pass the color array to points geometry, note the binding to tell the geometry
        // that only use one color for the whole object.
        shape_circle_drawable->setColorArray(colorsl, osg::Array::BIND_OVERALL);

        shape_circle_drawable->getOrCreateStateSet()->setAttribute(new osg::LineWidth(3.f), osg::StateAttribute::ON);

        // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
        // parameter passed to the DrawArrays constructor is the Primitive::Mode which
        // in this case is POINTS (which has the same value GL_POINTS), the second
        // parameter is the index position into the vertex array of the first point
        // to draw, and the third parameter is the number of points to draw.
        shape_circle_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,circle->size()));

        m_geode->addDrawable(shape_circle_drawable);

        // add border polyline
        shape_circle_drawable = new osg::Geometry();
        shape_circle_drawable->setVertexArray(circle);
        colorsl = new osg::Vec4Array;
        colorsl->push_back(m_color);
        shape_circle_drawable->setColorArray(colorsl, osg::Array::BIND_OVERALL);
        shape_circle_drawable->getOrCreateStateSet()->setAttribute(new osg::LineWidth(2.f), osg::StateAttribute::ON);
        shape_circle_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,circle->size()));
        m_geode->addDrawable(shape_circle_drawable);
    }

}

