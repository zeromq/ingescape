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
     * @brief Called by QQuickWindow::afterRendering to change the visualize mode of our window when it is possible
     */
    void _applyCurrentWindowRenderingMode();


    /**
     * @brief Second part of _applyCurrentWindowRenderingMode
     */
    void _applyCurrentWindowRenderingModeFinished();



protected:
    /**
     * @brief Update rendering of our window
     */
    void _updateCurrentWindowRenderingMode();



protected:
    // Mutex used to update the rendering mode of our window
    QMutex _mutexWindowRenderingMode;

    // Connection used to update the rendering mode of our window
    QMetaObject::Connection _connectionWindowRenderingMode;
};


QML_DECLARE_TYPE(DebugQuickInspector)


#endif // DEBUGQUICKINSPECTOR_H
