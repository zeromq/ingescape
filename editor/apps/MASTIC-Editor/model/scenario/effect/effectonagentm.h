/*
 *	MASTIC Editor
 *
 *  Copyright Â© 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef EFFECTONAGENTM_H
#define EFFECTONAGENTM_H

#include <QObject>
#include <model/scenario/effect/actioneffectm.h>

#include "I2PropertyHelpers.h"

/**
 * Values of effect on agent: ON, OFF
 */
I2_ENUM_CUSTOM(AgentEffectValues, ON, OFF)


/**
 * @brief The EffectOnAgentM class defines a model of action effect on an agent
 */
class EffectOnAgentM : public ActionEffectM
{
    Q_OBJECT

    // Value of our effect on agent
    I2_QML_PROPERTY(AgentEffectValues::Value, agentEffectValue)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit EffectOnAgentM(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~EffectOnAgentM();


    /**
     * @brief Copy from another effect model
     * @param effct to copy
     */
    void copyFrom(ActionEffectM* effect);

};

QML_DECLARE_TYPE(EffectOnAgentM)

#endif // EFFECTONAGENTM_H
