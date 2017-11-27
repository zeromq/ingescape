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

#ifndef MAPPINGEFFECTM_H
#define MAPPINGEFFECTM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"

#include <model/scenario/effect/actioneffectm.h>


/**
 * Values of effect on mapping: MAPPED, UNMAPPED
 */
I2_ENUM_CUSTOM(MappingEffectValues, MAPPED, UNMAPPED)


/**
 * @brief The MappingEffectM class defines an action effect on a mapping between two iop value
 */
class MappingEffectM: public ActionEffectM
{
    Q_OBJECT

    // Value of our effect on mapping
    I2_QML_PROPERTY(MappingEffectValues::Value, mappingEffectValue)

    // Output of the output agent
    I2_QML_PROPERTY(AgentIOPM*, output)

    // Input Agent
    I2_QML_PROPERTY_CUSTOM_SETTER(AgentInMappingVM*, inputAgent)

    // Input of the input agent
    I2_QML_PROPERTY(AgentIOPM*, input)

    // List of outputs
    I2_QOBJECT_LISTMODEL(AgentIOPM , outputsList)

    // Concatened list of TO iop agents items
    I2_QOBJECT_LISTMODEL(AgentIOPM , toAgentIopList)


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit MappingEffectM(QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~MappingEffectM();


    /**
      * @brief Redefinition of action effect copy
      */
    void copyFrom(ActionEffectM* effect);


    /**
    * @brief Custom setter on set agent to fill inputs and outputs
    * @param agent
    */
    void setagent(AgentInMappingVM* agent);


Q_SIGNALS:


public Q_SLOTS:


protected:

    /**
     * @brief Called when our "input agent" is destroyed
     * @param sender
     */
    void _onInputAgentDestroyed(QObject* sender);


};

QML_DECLARE_TYPE(MappingEffectM)

#endif // MAPPINGEFFECTM_H
