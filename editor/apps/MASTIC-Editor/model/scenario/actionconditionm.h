
/*
 *	ActionConditionM
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

#ifndef ACTIONCONDITIONM_H
#define ACTIONCONDITIONM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"

#include "model/agentm.h"

/**
  * Comparison type for an action: SUPERIOR_TO, INFERIOR_TO, DIFFER_TO, ON, OFF
  */
I2_ENUM_CUSTOM(ActionComparisonValueType, SUPERIOR_TO, INFERIOR_TO, EQUAL_TO, ON, OFF)


/**
 * @brief The ActionConditionM class defines the main controller of our application
 */
class ActionConditionM: public QObject
{
    Q_OBJECT

    // Agent model
    I2_QML_PROPERTY(AgentM*, agentModel)

    // Effect type
    I2_QML_PROPERTY(ActionComparisonValueType::Value, comparison)


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit ActionConditionM(QObject *parent = 0);


    /**
      * @brief Destructor
      */
    virtual ~ActionConditionM();

    /**
    * @brief Copy from another condition model
    * @param condition to copy
    */
    void copyFrom(ActionConditionM* condition);


Q_SIGNALS:


public Q_SLOTS:


protected:



};

QML_DECLARE_TYPE(ActionConditionM)

#endif // ACTIONCONDITIONM_H
