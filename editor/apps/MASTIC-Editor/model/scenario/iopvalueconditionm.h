
/*
 *	IOPValueConditionM
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

#ifndef IOPVALUECONDITIONM_H
#define IOPVALUECONDITIONM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include "actionconditionm.h"



/**
 * @brief The IOPValueConditionM class defines an action condition on iop value
 */
class IOPValueConditionM: public ActionConditionM
{
    Q_OBJECT

    // Agent IOP
    I2_QML_PROPERTY_CUSTOM_SETTER(AgentIOPM* , agentIOP)

    // Agent IOP name
    I2_QML_PROPERTY(QString , agentIOPName)

    // value in string format
    I2_QML_PROPERTY(QString , value)

    // Concatened list of iop agents items
    I2_QOBJECT_LISTMODEL(AgentIOPM , agentIopList)


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit IOPValueConditionM(QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~IOPValueConditionM();

    /**
      * @brief Redefinition of action condition copy
      */
    void copyFrom(ActionConditionM* condition);

    /**
    * @brief Custom setter on set agent
    *        to fill inputs and outputs
    * @param agent
    */
    void setagent(AgentInMappingVM* agent);

    /**
      * @brief Initialize the agent connections for the action condition
      */
    void initializeConnections();

    /**
      * @brief Reset the agent connections for the action condition
      */
    void resetConnections();

Q_SIGNALS:


public Q_SLOTS:

    /**
      * @brief Slot on agent inputs list change
      */
    void onInputsListChange(QList<InputVM*> inputsList);

    /**
      * @brief Slot on agent outputs list change
      */
    void onOutputsListChange(QList<OutputVM *> outputsList);

    /**
      * @brief Slot on IsON flag agent change
      */
    void onAgentModelIsOnChange(bool isON);

protected Q_SLOTS:
    /**
     * @brief Called when our agent iop model is destroyed
     * @param sender
     */
    void _onAgentIopModelDestroyed(QObject* sender);

    /**
      * @brief Slot on agent iop value change
      */
    void _onCurrentValueChange(QVariant currentValue);

protected:

private:
    /**
    * @brief Update the selected agent iop
    */
    void updateAgentIOPSelected();

};

QML_DECLARE_TYPE(IOPValueConditionM)

#endif // IOPVALUECONDITIONM_H
