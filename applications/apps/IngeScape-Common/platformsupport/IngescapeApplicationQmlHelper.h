/*
 *	IngeScape Common
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */


#ifndef INGESCAPEAPPLICATIONQMLHELPER_H
#define INGESCAPEAPPLICATIONQMLHELPER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>
#include <QQuickWindow>


#include "I2PropertyHelpers.h"


/**
 * @brief The IngescapeApplicationQmlHelper class defines an helper class used by QML
 *        to define properties of IngescapeApplication
 */
class IngescapeApplicationQmlHelper : public QObject
{
    Q_OBJECT

    // Current window
    I2_QML_PROPERTY_DELETE_PROOF(QQuickWindow*, currentWindow)

public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit IngescapeApplicationQmlHelper(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~IngescapeApplicationQmlHelper();


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


 protected Q_SLOTS:
     /**
      * @brief Called when our current window has changed
      */
     void _onCurrentWindowChanged();
};

QML_DECLARE_TYPE(IngescapeApplicationQmlHelper)

#endif // INGESCAPEAPPLICATIONQMLHELPER_H
