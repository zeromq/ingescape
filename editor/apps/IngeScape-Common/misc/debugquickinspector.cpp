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

//#include <private/qquickwindow_p.h>


/**
 * @brief Default constructor
 * @param parent
 */
DebugQuickInspector::DebugQuickInspector(QObject *parent) : QObject(parent),
    _currentWindow(nullptr),
    _currentWindowRenderingMode(DebugWindowRenderingMode::Normal)
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
        }

        // Save our new value
        _currentWindow = value;

        // Update
        if (_currentWindow != nullptr)
        {
            connect(_currentWindow, &QQuickWindow::destroyed, this, &DebugQuickInspector::_onCurrentWindowDestroyed);

            // Update the rendering mode of our window
            _updateCurrentWindowRenderingMode();
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
/*
        // Get the private API of our window
        QQuickWindowPrivate *windowPrivate = QQuickWindowPrivate::get(_currentWindow);
        if (windowPrivate != nullptr)
        {
            // Required rendering mode
            QByteArray newRenderingMode;
            switch (_currentWindowRenderingMode)
            {
               case DebugWindowRenderingMode::Normal:
                   break;

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
 */
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
