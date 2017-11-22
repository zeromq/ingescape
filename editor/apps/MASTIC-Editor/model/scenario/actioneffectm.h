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
  * Effect type for an action: ON, OFF, ENABLE, DISABLE
  */
I2_ENUM_CUSTOM(ActionEffectValueType, ON, OFF, ENABLE, DISABLE)


/**
 * @brief The ActionEffectM class defines the main controller of our application
 */
class ActionEffectM: public QObject
{
    Q_OBJECT

    // Agent model
    I2_QML_PROPERTY_CUSTOM_SETTER(AgentInMappingVM*, agentModel)

    // Effect type
    I2_QML_PROPERTY(ActionEffectValueType::Value, effect)

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
     * @brief Called when our agent model is destroyed
     * @param sender
     */
    void _onAgentModelDestroyed(QObject* sender);

protected:



};

QML_DECLARE_TYPE(ActionEffectM)

#endif // ACTIONEFFECTM_H
