#include "OSGWidget/OSGWidget.h"
#include "osgwidgettool.h"

OSGWidgetTool *OSGWidgetTool::s_instance = 0;

OSGWidgetTool::OSGWidgetTool() : m_currentType(OSGWidgetTool::None)
{
}

OSGWidgetTool::~OSGWidgetTool()
{
}

// must be initialized
void OSGWidgetTool::initialize(OSGWidget *_osgwidget)
{
    s_instance = new OSGWidgetTool();
    s_instance->m_osgWidget = _osgwidget;
    s_instance->m_currentType = None;

    connect(s_instance->m_osgWidget,SIGNAL(signal_onMousePress(Qt::MouseButton,int,int)),
            s_instance,SLOT(slot_mouseButtonDown(Qt::MouseButton,int,int)));
}


void OSGWidgetTool::slot_mouseButtonDown(Qt::MouseButton _button, int _x, int _y)
{
    if(m_currentType == None)
        return;

    if(_button == Qt::MouseButton::RightButton)
    {
        // cancel or end
        endTool();
    }

    if(_button == Qt::MouseButton::LeftButton)
    {
        // clic
        bool exists = false;
        osg::Vec3d vect;
        m_osgWidget->getIntersectionPoint(_x, _y, vect, exists);
        if(exists)
        {
            Point3D p;
            p.x = vect[0];
            p.y = vect[1];
            p.z = vect[2];
            emit signal_clicked(p);
        }
    }
}

void OSGWidgetTool::startTool(const type _type)
{
    endTool();

    m_currentType = _type;

    QString msg = tr("Start tool ");
    switch(_type)
    {
    case Point:
        msg += "Point";
        break;
    case Line:
        msg += "Line";
        break;
    case Area:
        msg += "Area";
        break;
    default:
        break;
    }

    m_osgWidget->startTool(msg);
    m_osgWidget->setCursor(Qt::CrossCursor);
}

void OSGWidgetTool::endTool()
{
    if(m_currentType != None)
    {
        if(!m_osgWidget->isValid())
            return;

        m_osgWidget->setCursor(Qt::ArrowCursor);


        // cancel or end
        QString msg = tr("End tool ");
        switch(m_currentType)
        {
        case Point:
            msg += "Point";
            break;
        case Line:
            msg += "Line";
            break;
        case Area:
            msg += "Area";
            break;
        default:
            break;
        }
        m_osgWidget->endTool(msg);

        emit signal_endTool();
        m_currentType = None;
    }
}


void OSGWidgetTool::slot_cancelTool()
{
    if(m_currentType != None)
    {
        emit signal_cancelTool();

        endTool();
    }
}
