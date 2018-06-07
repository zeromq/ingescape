/*
 *	ScenarioM
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

#ifndef SCENARIOM_H
#define SCENARIOM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"

#include "model/scenario/scenariomarkerm.h"
#include "model/scenario/actionm.h"

/**
 * @brief The ScenarioM class defines a scenario model
 */
class ScenarioM: public QObject
{
    Q_OBJECT

    // Scenario name
    I2_QML_PROPERTY(QString, name)

    // Current date time
    I2_QML_PROPERTY(QDateTime, currentTime)

    // Global mapping
    //I2_QML_PROPERTY(GlobalMappingM, globalMapping)

    // List of markers
    I2_QOBJECT_LISTMODEL(ScenarioMarkerM, markersList)

    // List of actions
    I2_QOBJECT_LISTMODEL(ActionM, actionsList)

public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit ScenarioM(QString name, QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~ScenarioM();



Q_SIGNALS:


public Q_SLOTS:


protected:



};

QML_DECLARE_TYPE(ScenarioM)

#endif // SCENARIOM_H
