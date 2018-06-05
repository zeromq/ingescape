/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef CONDITIONONAGENTM_H
#define CONDITIONONAGENTM_H

#include <QObject>

#include "I2PropertyHelpers.h"
#include "actionconditionm.h"

/**
 * @brief The ConditionOnAgentM class defines an action condition on an agent
 * condition on the state (ON/OFF) of an agent
 */
class ConditionOnAgentM : public ActionConditionM
{
    Q_OBJECT


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit ConditionOnAgentM(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~ConditionOnAgentM();


    /**
      * @brief Redefinition of action condition copy
      */
    void copyFrom(ActionConditionM* condition) Q_DECL_OVERRIDE;


    /**
    * @brief Setter for property "Agent"
    * @param agent
    */
    void setagent(AgentInMappingVM* agent) Q_DECL_OVERRIDE;


    /**
      * @brief Initialize the agent connections for the action condition
      */
    void initializeConnections() Q_DECL_OVERRIDE;


    /**
      * @brief Reset the agent connections for the action condition
      */
    void resetConnections() Q_DECL_OVERRIDE;


Q_SIGNALS:


protected Q_SLOTS:

    /**
      * @brief Slot called when the flag "is ON" of an agent changed
      */
    void _onAgentModelIsOnChanged(bool isON) Q_DECL_OVERRIDE;

};

QML_DECLARE_TYPE(ConditionOnAgentM)

#endif // CONDITIONONAGENTM_H
