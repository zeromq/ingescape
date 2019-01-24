/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef SCENARIOM_H
#define SCENARIOM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"

//#include "model/scenario/scenariomarkerm.h"
#include "model/scenario/actionm.h"
#include "viewModel/scenario/actionvm.h"
#include "viewModel/scenario/actioninpalettevm.h"


/**
 * @brief The ScenarioM class defines a model of scenario
 */
class ScenarioM: public QObject
{
    Q_OBJECT

    // Scenario name
    I2_QML_PROPERTY(QString, name)

    // Current date time
    //I2_QML_PROPERTY(QDateTime, currentTime)

    // List of markers
    //I2_QOBJECT_LISTMODEL(ScenarioMarkerM, markersList)

    // List of actions
    I2_QOBJECT_LISTMODEL(ActionM, actionsList)

    // List of actions in the palette
    I2_QOBJECT_LISTMODEL(ActionInPaletteVM, actionsInPaletteList)

    // List of actions in the timeline
    I2_QOBJECT_LISTMODEL(ActionVM, actionsInTimelineList)


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit ScenarioM(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~ScenarioM();



Q_SIGNALS:


public Q_SLOTS:


private:

};

QML_DECLARE_TYPE(ScenarioM)

#endif // SCENARIOM_H
