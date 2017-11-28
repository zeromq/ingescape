/*
 *	MASTIC Editor
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
    I2_QML_PROPERTY_CUSTOM_SETTER(AgentIOPM* , agentIOP)

    // Agent IOP name
    I2_QML_PROPERTY(QString , agentIOPName)

    // Value converted into string
    I2_QML_PROPERTY(QString, value)

    // Merged list of Inputs and Outputs of the agent
    I2_QOBJECT_LISTMODEL(AgentIOPM , iopMergedList)


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
    void copyFrom(ActionEffectM* effect);


    /**
    * @brief Custom setter on set agent to fill inputs and outputs
    * @param agent
    */
    void setagent(AgentInMappingVM* agent);

public Q_SLOTS:

    /**
      * @brief Slot on agent inputs list change
      */
    void onInputsListChange(QList<InputVM*> inputsList);

    /**
      * @brief Slot on agent outputs list change
      */
    void onOutputsListChange(QList<OutputVM *> outputsList);

protected Q_SLOTS:
    /**
     * @brief Called when our agent iop model is destroyed
     * @param sender
     */
    void _onAgentIopModelDestroyed(QObject* sender);

private:
    /**
    * @brief Update the selected agent iop
    */
    void updateAgentIOPSelected();

};

QML_DECLARE_TYPE(IOPValueEffectM)

#endif // IOPVALUEEFFECTM_H
