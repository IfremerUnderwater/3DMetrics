#ifndef SNAPGEOTIFFDEPTH_H
#define SNAPGEOTIFFDEPTH_H

#include <osgViewer/Viewer>
#include <osgViewer/Renderer>

#include <QWidget>

class SnapGeotiffDepth
{
    /** Capture the frame buffer and write image to disk*/
    class WindowCaptureCallback : public osg::Camera::DrawCallback
    {
    public:
        WindowCaptureCallback(GLenum readBuffer, const std::string& name) :
            _readBuffer(readBuffer),
            _fileName(name)
        {
        }

        virtual void operator () (osg::RenderInfo& renderInfo) const
        {
#if !defined(OSG_GLES1_AVAILABLE) && !defined(OSG_GLES2_AVAILABLE)
            osg::State& state = *renderInfo.getState();
            state.glReadBuffer(_readBuffer);
#else
            osg::notify(osg::NOTICE) << "Error: GLES unable to do glReadBuffer" << std::endl;
#endif

            OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
            osg::GraphicsContext* gc = renderInfo.getState()->getGraphicsContext();
            if (gc->getTraits())
            {
                GLenum pixelFormat;

                if (gc->getTraits()->alpha)
                    pixelFormat = GL_RGBA;
                else
                    pixelFormat = GL_RGB;

#if defined(OSG_GLES1_AVAILABLE) || defined(OSG_GLES2_AVAILABLE)
                if (pixelFormat == GL_RGB)
                {
                    GLint value = 0;
#ifndef GL_IMPLEMENTATION_COLOR_READ_FORMAT
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#endif
                    glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &value);
                    if (value != GL_RGB ||
                            value != GL_UNSIGNED_BYTE)
                    {
                        pixelFormat = GL_RGBA;//always supported
                    }
                }
#endif
            }
        }

    protected:
        GLenum                      _readBuffer;
        std::string                 _fileName;
        mutable OpenThreads::Mutex  _mutex;
    };

    /** Do Culling only while loading PagedLODs*/
    class CustomRenderer : public osgViewer::Renderer
    {
    public:
        CustomRenderer(osg::Camera* camera)
            : osgViewer::Renderer(camera),
              _cullOnly(true)
        {
        }

        /** Set flag to omit drawing in renderingTraversals */
        void setCullOnly(bool on) { _cullOnly = on; }

        virtual void operator () (osg::GraphicsContext* /*context*/)
        {
            if (_graphicsThreadDoesCull)
            {
                if (_cullOnly)
                    cull();
                else
                    cull_draw();
            }
        }

        virtual void cull()
        {
            osgUtil::SceneView* sceneView = _sceneView[0].get();
            if (!sceneView || _done) return;

            updateSceneView(sceneView);

            osgViewer::View* view = dynamic_cast<osgViewer::View*>(_camera->getView());
            if (view) sceneView->setFusionDistance(view->getFusionDistanceMode(), view->getFusionDistanceValue());

            sceneView->inheritCullSettings(*(sceneView->getCamera()));
            sceneView->cull();
        }

        bool _cullOnly;
    };

public:
    SnapGeotiffDepth(osgViewer::Viewer* pView);
    static bool Capture(osg::ref_ptr<osg::Node> _node, std::string fileName, const double _pixel_size, double _refLatitude, double _refLongitude, QWidget *_parentWidget);

    ~SnapGeotiffDepth();

protected:
    osgViewer::Viewer* pViewer;
};

#endif // SNAPGEOTIFFDEPTH_H
