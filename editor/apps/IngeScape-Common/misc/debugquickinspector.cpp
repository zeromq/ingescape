/*
 *	I2Quick
 *
 *  Copyright (c) 2018 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort <lemort@ingenuity.io>
 *
 */

#include "debugquickinspector.h"

#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLFunctions>


#include <private/qquickwindow_p.h>
#include <private/qsgrenderloop_p.h>


/**
 * @brief Default constructor
 * @param parent
 */
DebugQuickInspector::DebugQuickInspector(QObject *parent) : QObject(parent),
    _currentWindow(nullptr),
    _currentWindowRenderingMode(DebugWindowRenderingMode::Normal),
    _qtCompilationVersion(QT_VERSION_STR),
    _qtRuntimeVersion(qVersion()),
    _sceneGraphBackend(""),
    _renderLoop(""),
    _useOpenGL(false),
    _openGLVendor(""),
    _openGLRenderer(""),
    _openGLVersion("")
{
}



/**
 * @brief Destructor
 */
DebugQuickInspector::~DebugQuickInspector()
{
    // Clean-up current window
    QMutexLocker lock(&_mutexWindowRenderingMode);

    if (_connectionWindowRenderingMode)
    {
        disconnect(_connectionWindowRenderingMode);
    }

    setcurrentWindow(nullptr);
}



/**
 * @brief Method used to provide a singleton to QML
 * @param engine
 * @param scriptEngine
 * @return
 */
QObject* DebugQuickInspector::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new DebugQuickInspector();
}



/**
 * @brief Set our current window
 * @param value
 */
void DebugQuickInspector::setcurrentWindow(QQuickWindow *value)
{
    if (_currentWindow != value)
    {
        // Clean-up if needed
        if (_currentWindow != nullptr)
        {
            disconnect(_currentWindow, &QQuickWindow::destroyed, this, &DebugQuickInspector::_onCurrentWindowDestroyed);
            disconnect(_currentWindow, &QQuickWindow::beforeRendering, this, &DebugQuickInspector::_onCurrentWindowBeforeRendering);
        }

        // Save our new value
        _currentWindow = value;

        // Update
        if (_currentWindow != nullptr)
        {
            connect(_currentWindow, &QQuickWindow::destroyed, this, &DebugQuickInspector::_onCurrentWindowDestroyed);

            // Init Qt Quick infos
            connect(_currentWindow, &QQuickWindow::beforeRendering, this, &DebugQuickInspector::_onCurrentWindowBeforeRendering, Qt::DirectConnection);

            // Update the rendering mode of our window
            _updateCurrentWindowRenderingMode();
        }
        else
        {
            _resetQtQuickInfos();
        }

        // Notify change
        Q_EMIT currentWindowChanged(value);
    }
}



/**
 * @brief Set the rendering mode of our current window
 * @param value
 */
void DebugQuickInspector::setcurrentWindowRenderingMode(DebugWindowRenderingMode::Value value)
{
    if (_currentWindowRenderingMode != value)
    {
        // Save value
        _currentWindowRenderingMode = value;

        // Update the rendering mode of our window
        _updateCurrentWindowRenderingMode();

        // Notify change
        Q_EMIT currentWindowRenderingModeChanged(value);
    }
}


/**
 * @brief Called when our current window is destroyed
 */
void DebugQuickInspector::_onCurrentWindowDestroyed()
{
    QMutexLocker lock(&_mutexWindowRenderingMode);

    _currentWindow = nullptr;
    Q_EMIT currentWindowChanged(nullptr);
}


/**
 * @briefCalled when our window emits a beforeRendering signal
 */
void DebugQuickInspector::_onCurrentWindowBeforeRendering()
{
    _initQtQuickInfos();

    disconnect(_currentWindow, &QQuickWindow::beforeRendering, this, &DebugQuickInspector::_onCurrentWindowBeforeRendering);
}


/**
 * @brief Convert a DebugWindowRenderingMode::Value into a QByteArray
 * @param value
 * @return
 */
QByteArray DebugQuickInspector::_windowRenderingModeToByteArray(DebugWindowRenderingMode::Value value)
{
    QByteArray newRenderingMode;
    switch (value)
    {
        case DebugWindowRenderingMode::VisualizeBatches:
            newRenderingMode = QByteArray("batches");
            break;

        case DebugWindowRenderingMode::VisualizeClipping:
            newRenderingMode = QByteArray("clip");
            break;

        case DebugWindowRenderingMode::VisualizeChanges:
           newRenderingMode = QByteArray("changes");
            break;

        case DebugWindowRenderingMode::VisualizeOverdraw:
           newRenderingMode = QByteArray("overdraw");
           break;

        default:
           break;
    }

    return newRenderingMode;
}



/**
* @brief Called by QQuickWindow::afterRendering to change the visualize mode of our window when it is possible
*/
void DebugQuickInspector::_applyCurrentWindowRenderingMode()
{
    QMutexLocker lock(&_mutexWindowRenderingMode);

    // Clean-up our connection
    if (_connectionWindowRenderingMode)
    {
        disconnect(_connectionWindowRenderingMode);
    }

    // Update our window if needed
    if (_currentWindow != nullptr)
    {
        // Get the private API of our window
        QQuickWindowPrivate* windowPrivate = QQuickWindowPrivate::get(_currentWindow);
        if (windowPrivate != nullptr)
        {
            // Required rendering mode
            QByteArray newRenderingMode = _windowRenderingModeToByteArray(_currentWindowRenderingMode);

            // Check if rendering mode will change
            if (windowPrivate->customRenderMode != newRenderingMode)
            {
                // Clean the scene graph of our window
                QMetaObject::invokeMethod(_currentWindow, "cleanupSceneGraph", Qt::DirectConnection);

                // Change the rendering mode
                windowPrivate->customRenderMode = newRenderingMode;

                // Prepare update
                QMetaObject::invokeMethod(this, "_applyCurrentWindowRenderingModeFinished", Qt::QueuedConnection);
            }
        }
    }
}



/**
* @brief Second part of _applyCurrentWindowRenderingMode
*/
void DebugQuickInspector::_applyCurrentWindowRenderingModeFinished()
{
    QMutexLocker lock(&_mutexWindowRenderingMode);

    if (_currentWindow != nullptr)
    {
        _currentWindow->update();
    }
}



/**
* @brief Update rendering of our window
*/
void DebugQuickInspector::_updateCurrentWindowRenderingMode()
{
    QMutexLocker lock(&_mutexWindowRenderingMode);

    if (_currentWindow != nullptr)
    {
        // Clean-up previous connection
        if (_connectionWindowRenderingMode)
        {
            disconnect(_connectionWindowRenderingMode);
        }

        // Subscribe to afterRendering
        _connectionWindowRenderingMode = connect(_currentWindow, &QQuickWindow::afterRendering, this, &DebugQuickInspector::_applyCurrentWindowRenderingMode, Qt::DirectConnection);

        // Force update of our current window to trigger an afterRendering signal
        QMetaObject::invokeMethod(_currentWindow, "update", Qt::QueuedConnection);
    }
}


/**
 * @brief Init Qt Quick infos
 */
void DebugQuickInspector::_initQtQuickInfos()
{
    if (_currentWindow != nullptr)
    {
        // Render loop
        QSGRenderLoop* qsgRenderLoop = QSGRenderLoop::instance();
        if ((qsgRenderLoop != nullptr) && (qsgRenderLoop->metaObject() != nullptr))
        {
            QString renderLoopClass = qsgRenderLoop->metaObject()->className();

            if (QString::compare(renderLoopClass, "QSGThreadedRenderLoop", Qt::CaseInsensitive) == 0)
            {
                setrenderLoop("threaded");
            }
            else if (QString::compare(renderLoopClass, "QSGWindowsRenderLoop", Qt::CaseInsensitive) == 0)
            {
                setrenderLoop("window");
            }
            else if (QString::compare(renderLoopClass, "QSGGuiThreadRenderLoop", Qt::CaseInsensitive) == 0)
            {
                setrenderLoop("basic");
            }
            else
            {
                setrenderLoop("unknown");
            }
        }
        else
        {
            setrenderLoop("unknown");
        }


        // Scene graph backend
        if (_currentWindow->rendererInterface() != nullptr)
        {
            QSGRendererInterface::GraphicsApi graphicsApi = _currentWindow->rendererInterface()->graphicsApi();
            switch (graphicsApi)
            {
                case QSGRendererInterface::Software:
                    setsceneGraphBackend("software");
                    break;

                case QSGRendererInterface::OpenGL:
                    setsceneGraphBackend("OpenGL");
                    break;

                case QSGRendererInterface::Direct3D12:
                    setsceneGraphBackend("DirectX 12");
                    break;

                case QSGRendererInterface::OpenVG:
                    setsceneGraphBackend("OpenVG");
                    break;

                default:
                    setsceneGraphBackend("unknown");
                    break;
            }
        }
        else
        {
            setsceneGraphBackend("unknown");
        }


        // OpenGL infos
        if (_currentWindow->openglContext())
        {
            QOpenGLFunctions* functions = _currentWindow->openglContext()->functions();
            if (functions != nullptr)
            {
                setopenGLVendor( QString(reinterpret_cast<const char*>(functions->glGetString(GL_VENDOR))) );
                setopenGLRenderer( QString(reinterpret_cast<const char*>(functions->glGetString(GL_RENDERER))) );
                setopenGLVersion( QString(reinterpret_cast<const char*>(functions->glGetString(GL_VERSION))) );
            }
            else
            {
                setopenGLVendor("");
                setopenGLRenderer("");
                setopenGLVersion("");
            }

            setuseOpenGL(true);
        }
        else
        {
            setuseOpenGL(false);
            setopenGLVendor("");
            setopenGLRenderer("");
            setopenGLVersion("");
        }
    }
}


/**
 * @brief Reset Qt Quick infos
 */
void DebugQuickInspector::_resetQtQuickInfos()
{
    setsceneGraphBackend("");
    setrenderLoop("");
    setuseOpenGL(false);
    setopenGLVendor("");
    setopenGLRenderer("");
    setopenGLVersion("");
}

