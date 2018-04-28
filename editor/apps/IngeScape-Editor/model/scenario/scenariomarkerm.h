
/*
 *	ScenarioMarkerM
 *
 *  Copyright (c) 2016-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *
 */

#ifndef SCENARIOMARKERM_H
#define SCENARIOMARKERM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"



/**
 * @brief The ScenarioMarkerM class defines a scenario marker model
 */
class ScenarioMarkerM: public QObject
{
    Q_OBJECT

    // Time in milliseconds
    I2_QML_PROPERTY(int, time)

public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit ScenarioMarkerM(QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~ScenarioMarkerM();



Q_SIGNALS:


public Q_SLOTS:


protected:



};

QML_DECLARE_TYPE(ScenarioMarkerM)

#endif // SCENARIOMARKERM_H
