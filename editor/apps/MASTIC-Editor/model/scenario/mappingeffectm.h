
/*
 *	MappingEffectM
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

#ifndef MAPPINGEFFECTM_H
#define MAPPINGEFFECTM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"

#include "model/mapping/elementmappingm.h"
#include "model/scenario/actioneffectm.h"


/**
 * @brief The MappingEffectM class defines the main controller of our application
 */
class MappingEffectM: public ActionEffectM
{
    Q_OBJECT

    // TO Agent IOP
    I2_QML_PROPERTY(AgentIOPM *, fromAgentIOP)

    // TO Agent model
    I2_QML_PROPERTY_CUSTOM_SETTER(AgentInMappingVM *, toAgentModel)

    // TO Agent IOP
    I2_QML_PROPERTY(AgentIOPM *, toAgentIOP)

    // Concatened list of FROM iop agents items
    I2_QOBJECT_LISTMODEL(AgentIOPM , fromAgentIopList)

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
    * @brief Custom setter on set agent model
    *        to fill inputs and outputs
    * @param agentModel
    */
    bool setagentModel(AgentInMappingVM* agentModel);

Q_SIGNALS:


public Q_SLOTS:


protected:



};

QML_DECLARE_TYPE(MappingEffectM)

#endif // MAPPINGEFFECTM_H
