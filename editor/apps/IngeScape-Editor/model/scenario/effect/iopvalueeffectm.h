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

#ifndef IOPVALUEEFFECTM_H
#define IOPVALUEEFFECTM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"

#include "model/iop/agentiopm.h"
#include <model/scenario/effect/actioneffectm.h>


/**
 * @brief The IOPValueEffectM class defines an action effect on iop value
 */
class IOPValueEffectM: public ActionEffectM
{
    Q_OBJECT

    // Agent IOP
    I2_QML_PROPERTY_CUSTOM_SETTER(AgentIOPM*, agentIOP)

    // Name of our Agent IOP
    I2_QML_PROPERTY(QString, agentIOPName)

    // Value converted into string
    I2_QML_PROPERTY(QString, value)

    // Merged list of Inputs/Outputs/Parameters of the agent
    I2_QOBJECT_LISTMODEL(AgentIOPM, iopMergedList)


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit IOPValueEffectM(QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~IOPValueEffectM();


    /**
      * @brief Redefinition of action effect copy
      */
    void copyFrom(ActionEffectM* effect) Q_DECL_OVERRIDE;


    /**
    * @brief Setter for property "Agent"
    * @param agent
    */
    void setagent(AgentInMappingVM* agent) Q_DECL_OVERRIDE;


    /**
     * @brief Get a pair with the agent and the command (with parameters) of our effect
     * @return
     */
    QPair<AgentInMappingVM*, QStringList> getAgentAndCommandWithParameters() Q_DECL_OVERRIDE;


    /**
     * @brief Get a pair with the agent name and the reverse command (with parameters) of our effect
     * @return
     */
    QPair<QString, QStringList> getAgentNameAndReverseCommandWithParameters() Q_DECL_OVERRIDE;


Q_SIGNALS:


protected Q_SLOTS:

    /**
      * @brief Slot called when the models of Inputs/Outputs/Parameters changed of the agent in mapping
      */
    void _onModelsOfIOPChanged();


    /**
     * @brief Called when our agent iop model is destroyed
     * @param sender
     */
    void _onAgentIopModelDestroyed(QObject* sender);

};

QML_DECLARE_TYPE(IOPValueEffectM)

#endif // IOPVALUEEFFECTM_H
