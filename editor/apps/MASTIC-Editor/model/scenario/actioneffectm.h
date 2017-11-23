/*
 *	ActionEffectM
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

#ifndef ACTIONEFFECTM_H
#define ACTIONEFFECTM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include "viewModel/agentinmappingvm.h"


/**
 * Values of effect on agent: ON, OFF
 */
I2_ENUM_CUSTOM(AgentEffectValues, ON, OFF)


/**
 * @brief The ActionEffectM class defines an action effect model
 */
class ActionEffectM : public QObject
{
    Q_OBJECT

    // View model of agent in mapping
    I2_QML_PROPERTY_CUSTOM_SETTER(AgentInMappingVM*, agent)

    // Value of our effect on agent
    I2_QML_PROPERTY(AgentEffectValues::Value, agentEffectValue)


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit ActionEffectM(QObject *parent = 0);


    /**
      * @brief Destructor
      */
    virtual ~ActionEffectM();

    /**
    * @brief Copy from another effect model
    * @param effct to copy
    */
    void copyFrom(ActionEffectM* effect);



Q_SIGNALS:


public Q_SLOTS:

    /**
     * @brief Called when our agent is destroyed
     * @param sender
     */
    void _onAgentDestroyed(QObject* sender);

protected:



};

QML_DECLARE_TYPE(ActionEffectM)

#endif // ACTIONEFFECTM_H
