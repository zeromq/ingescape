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


#ifndef DEBUGQUICKINSPECTOR_H
#define DEBUGQUICKINSPECTOR_H

#include <QObject>
#include <QtQml>
#include <QQuickWindow>


#include "I2PropertyHelpers.h"


/**
 * @brief Enum defining the different rendering modes i.e. the value of QSG_VISUALIZE
 */
I2_ENUM(DebugWindowRenderingMode,
        Normal,
        VisualizeBatches,
        VisualizeClipping,
        VisualizeChanges,
        VisualizeOverdraw
        )


/**
 * @brief The DebugQuickInspector class defines a singleton used to introspect a QML application (overdraw, clipping, batches, etc.)
 */
class DebugQuickInspector : public QObject
{
    Q_OBJECT

    // Current window
    I2_QML_PROPERTY_CUSTOM_SETTER(QQuickWindow*, currentWindow)

    // Rendering mode of our current window
    I2_QML_PROPERTY_CUSTOM_SETTER(DebugWindowRenderingMode::Value, currentWindowRenderingMode)

    // Version of Qt used to compile the application
    I2_QML_PROPERTY_READONLY(QString, qtCompilationVersion)

    // Version of Qt used at runtime
    I2_QML_PROPERTY_READONLY(QString, qtRuntimeVersion)

    // Scene graph backend
    I2_QML_PROPERTY_READONLY(QString, sceneGraphBackend)

    // Render loop
    I2_QML_PROPERTY_READONLY(QString, renderLoop)

    // Flag indicating if we use OpenGL
    I2_QML_PROPERTY_READONLY(bool, useOpenGL)

    // OpenGL vendor
    I2_QML_PROPERTY_READONLY(QString, openGLVendor)

    // OpenGL renderer
    I2_QML_PROPERTY_READONLY(QString, openGLRenderer)

    // OpenGL version
    I2_QML_PROPERTY_READONLY(QString, openGLVersion)

    // CPU architecture
    I2_QML_PROPERTY_READONLY(QString, cpuArchitecture)

    // OS
    I2_QML_PROPERTY_READONLY(QString, os)

    // System information
    I2_QML_PROPERTY_READONLY(QString, systemInformation)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit DebugQuickInspector(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~DebugQuickInspector();


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


protected Q_SLOTS:
     /**
      * @brief Called when our current window is destroyed
      */
     void _onCurrentWindowDestroyed();


     /**
      * @briefCalled when our window emits a beforeRendering signal
      */
     void _onCurrentWindowBeforeRendering();


    /**
     * @brief Called by QQuickWindow::afterRendering to change the visualize mode of our window when it is possible
     */
    void _applyCurrentWindowRenderingMode();


    /**
     * @brief Second part of _applyCurrentWindowRenderingMode
     */
    void _applyCurrentWindowRenderingModeFinished();


protected:
    /**
     * @brief Convert a DebugWindowRenderingMode::Value into a QByteArray
     * @param value
     * @return
     */
    QByteArray _windowRenderingModeToByteArray(DebugWindowRenderingMode::Value value);


    /**
     * @brief Update rendering of our window
     */
    void _updateCurrentWindowRenderingMode();


    /**
     * @brief Init Qt Quick infos
     */
    void _initQtQuickInfos();


    /**
     * @brief Reset Qt Quick infos
     */
    void _resetQtQuickInfos();


    /**
     * @brief Update our systemInformation property
     */
    void _updateSystemInformation();


protected:
    // Mutex used to update the rendering mode of our window
    QMutex _mutexWindowRenderingMode;

    // Connection used to update the rendering mode of our window
    QMetaObject::Connection _connectionWindowRenderingMode;
};


QML_DECLARE_TYPE(DebugQuickInspector)


#endif // DEBUGQUICKINSPECTOR_H
