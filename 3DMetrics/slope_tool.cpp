#include "slope_tool.h"
#include "ui_slope_tool.h"

#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Point>
#include <osg/LineWidth>

#include "OSGWidget/osg_widget.h"
#include "OSGWidget/osg_widget_tool.h"

#include "Tools/geometry.h"

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

    ui->message_label->setText("Click Start to Start");
    ui->x_label->setText("");
    ui->y_label->setText("");
    ui->z_label->setText("");
    ui->slope_label->setText("");
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

    ui->x_label->setText("");
    ui->y_label->setText("");
    ui->z_label->setText("");
    ui->slope_label->setText("");

    ui->message_label->setText("Select Center Point");

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
    ui->message_label->setText("");
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
    if(m_centerSet && (m_xcenter != m_xclicked || m_ycenter != m_yclicked))
    {
        OSGWidgetTool::instance()->getOSGWidget()->getView()->requestContinuousUpdate(false);
        ui->message_label->setText("Please wait...");
        update();
        repaint();
        qApp->processEvents();
        QApplication::setOverrideCursor(Qt::WaitCursor);

        // compute plane
        double radius = hypot(m_xclicked - m_xcenter, m_yclicked - m_ycenter);
        double radius2 = radius * radius;
        double delta = radius/10;

        std::vector<Eigen::Vector3f> points;

        for(int x = -10; x <= 10; x++)
        {
            for(int y = -10; y <= 10; y++)
            {
                if( (x * delta) * (x * delta) + (y * delta) * (y * delta) <= radius2)
                {
                    int px = m_xcenter + x * delta;
                    int py = m_ycenter + y * delta;

                    bool exists = false;
                    osg::Vec3d vect;
                    OSGWidgetTool::instance()->getOSGWidget()->getIntersectionPoint(px, py, vect, exists);

                    if(exists)
                    {
                        Eigen::Vector3f point;
                        point[0] = vect[0];
                        point[1] = vect[1];
                        point[2] = vect[2];

                        points.push_back(point);
                    }
                }
            }
        }

        if(points.size() > 3)
        {
            // Fit points to plane
            std::pair<Eigen::Vector3f, Eigen::Vector3f> plane_coeffs = Geometry::bestPlaneFromPoints(points);

            // add lozange = polygon with 4 sides
            std::vector<Eigen::Vector3f> pcircle;
            for(int i=0; i<4; i++)
            {
                int px = m_xcenter + radius* sin(i*2*M_PI / 4 );
                int py = m_ycenter + radius* cos(i*2*M_PI / 4);;

                bool exists = false;
                osg::Vec3d vect;
                OSGWidgetTool::instance()->getOSGWidget()->getIntersectionPoint(px, py, vect, exists);

                if(exists)
                {
                    Eigen::Vector3f point;
                    point[0]= vect[0];
                    point[1]= vect[1];
                    point[2]= vect[2];

                    pcircle.push_back(point);
                }
            }

            QString txt = QString::number(plane_coeffs.second[0],'f',3);
            ui->x_label->setText("X= " + txt);
            txt = QString::number(plane_coeffs.second[1],'f',3);
            ui->y_label->setText("Y= " + txt);
            txt = QString::number(plane_coeffs.second[2],'f',3);
            ui->z_label->setText("Z= " + txt);

            osg::Vec3d vect;
            vect[0] = plane_coeffs.second[0];
            vect[1] = plane_coeffs.second[1];
            vect[2] = plane_coeffs.second[2];

            vect.normalize();
            double slope = acos( fabs(vect[2])) * 180.0 / M_PI;
            txt = QString::number(slope,'f',1);
            ui->slope_label->setText("slope= " + txt +"°");
            // Project points on plane
            std::vector<Eigen::Vector3f> proj_points;
            Geometry::project3DPointsToPlane(plane_coeffs, pcircle, proj_points);

            // draw geometry
            m_geode->removeDrawables(1, 3);
            osg::Vec3Array* circle = new osg::Vec3Array;
            for(unsigned int i=0; i<proj_points.size(); i++)
            {
                osg::Vec3d p;
                p[0] = proj_points[i][0];
                p[1] = proj_points[i][1];
                p[2] = proj_points[i][2];
                circle->push_back((p));
            }
            // line
            osg::Geometry* shape_circle_drawable = new osg::Geometry();

            // pass the created vertex array to the points geometry object.
            shape_circle_drawable->setVertexArray(circle);

            osg::Vec4f pcolor; // = m_color;
            pcolor[0] = 1.0f;
            pcolor[1] = 0.0f;
            pcolor[2] = 0.0f;
            pcolor[3] = 0.60f;
            // add a red color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
            //osg::Vec4 colorl(1.0f,0.0f,0.0f,1.0f);
            osg::Vec4Array* colorsl = new osg::Vec4Array;
            colorsl->push_back(pcolor);

            // pass the color array to points geometry, note the binding to tell the geometry
            // that only use one color for the whole object.
            shape_circle_drawable->setColorArray(colorsl, osg::Array::BIND_OVERALL);

            shape_circle_drawable->getOrCreateStateSet()->setAttribute(new osg::LineWidth(5.f), osg::StateAttribute::ON);

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
            pcolor[3] =1.0f;
            colorsl->push_back(pcolor);
            shape_circle_drawable->setColorArray(colorsl, osg::Array::BIND_OVERALL);
            shape_circle_drawable->getOrCreateStateSet()->setAttribute(new osg::LineWidth(2.f), osg::StateAttribute::ON);
            shape_circle_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,circle->size()));
            m_geode->addDrawable(shape_circle_drawable);

            QApplication::restoreOverrideCursor();

            ui->message_label->setText("Computed");
            OSGWidgetTool::instance()->getOSGWidget()->getView()->requestContinuousUpdate(true);
        }
    }


    m_centerSet = false;

    disconnect(OSGWidgetTool::instance(), 0, this, 0);
    disconnect(OSGWidgetTool::instance()->getOSGWidget(), 0, this, 0);

    OSGWidgetTool::instance()->endTool();

    QString msg = "Slope tool ended";
    emit signal_toolEnded(msg);
}

void SlopeTool::slot_toolClickedXY(Point3D& _p, int _x, int _y)
{
    m_geode->removeDrawables(0, 3);

    m_xclicked = _x;
    m_yclicked = _y;

    // first clic : point
    if(!m_centerSet)
    {
        m_center[0] = _p.x;
        m_center[1] = _p.y;
        m_center[2] = _p.z;
        m_xcenter = _x;
        m_ycenter = _y;

        // just in case...
        m_clicked = m_center;
        ui->message_label->setText("Select Circle Radius");
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


    // area
    if(m_centerSet)
    {
        ui->message_label->setText("Please wait...");
        update();
        repaint();
        qApp->processEvents();
        QApplication::setOverrideCursor(Qt::WaitCursor);

        // save clicked point
        m_clicked[0] = _p.x;
        m_clicked[1] = _p.y;
        m_clicked[2] = _p.z;

        double radius = hypot(_x - m_xcenter, _y - m_ycenter);

        // add circle = polygon with 32 sides
        osg::Vec3Array* circle = new osg::Vec3Array;
        for(int i=0; i<32; i++)
        {
            int px = m_xcenter + radius* sin(i*2*M_PI / 32);
            int py = m_ycenter + radius* cos(i*2*M_PI / 32);;

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

        QApplication::restoreOverrideCursor();

        ui->message_label->setText("Right Click to Compute");

    }

    //    if(m_centerSet)
    //    {
    //        OSGWidgetTool::instance()->endTool();
    //    }
    m_centerSet = true;
}

//// not used : too slow
//void SlopeTool::slot_mouse_moved(int _x, int _y)
//{
//    if(!m_centerSet)
//        return;

//    m_geode->removeDrawables(1, 2);

//    osg::Matrix matrix = OSGWidgetTool::instance()->getOSGWidget()->getCamera()->getProjectionMatrix();

//    // area
//    if(m_centerSet)
//    {
//        double radius = hypot(_x - m_xcenter, _y - m_ycenter);

//        // add circle = polygon with 64 sides
//        osg::Vec3Array* circle = new osg::Vec3Array;
//        for(int i=0; i<64; i++)
//        {
//            int px = m_xcenter + radius* sin(i*2*M_PI / 64);
//            int py = m_ycenter + radius* cos(i*2*M_PI / 64);;

//            bool exists = false;
//            osg::Vec3d vect;
//            OSGWidgetTool::instance()->getOSGWidget()->getIntersectionPoint(px, py, vect, exists);

//            if(exists)
//            {
//                circle->push_back(vect);
//            }
//        }

//        // line
//        osg::Geometry* shape_circle_drawable = new osg::Geometry();

//        // pass the created vertex array to the points geometry object.
//        shape_circle_drawable->setVertexArray(circle);

//        osg::Vec4f pcolor = m_color;
//        pcolor[3] = 0.25f;
//        // add a red color, colors take the form r,g,b,a with 0.0 off, 1.0 full on.
//        //osg::Vec4 colorl(1.0f,0.0f,0.0f,1.0f);
//        osg::Vec4Array* colorsl = new osg::Vec4Array;
//        colorsl->push_back(pcolor);

//        // pass the color array to points geometry, note the binding to tell the geometry
//        // that only use one color for the whole object.
//        shape_circle_drawable->setColorArray(colorsl, osg::Array::BIND_OVERALL);

//        shape_circle_drawable->getOrCreateStateSet()->setAttribute(new osg::LineWidth(3.f), osg::StateAttribute::ON);

//        // create and add a DrawArray Primitive (see include/osg/Primitive).  The first
//        // parameter passed to the DrawArrays constructor is the Primitive::Mode which
//        // in this case is POINTS (which has the same value GL_POINTS), the second
//        // parameter is the index position into the vertex array of the first point
//        // to draw, and the third parameter is the number of points to draw.
//        shape_circle_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,circle->size()));

//        m_geode->addDrawable(shape_circle_drawable);

//        // add border polyline
//        shape_circle_drawable = new osg::Geometry();
//        shape_circle_drawable->setVertexArray(circle);
//        colorsl = new osg::Vec4Array;
//        colorsl->push_back(m_color);
//        shape_circle_drawable->setColorArray(colorsl, osg::Array::BIND_OVERALL);
//        shape_circle_drawable->getOrCreateStateSet()->setAttribute(new osg::LineWidth(2.f), osg::StateAttribute::ON);
//        shape_circle_drawable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,circle->size()));
//        m_geode->addDrawable(shape_circle_drawable);
//    }

//}

