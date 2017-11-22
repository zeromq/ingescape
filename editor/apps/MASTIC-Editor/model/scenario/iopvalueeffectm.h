
/*
 *	IOPValueEffectM
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

#ifndef IOPVALUEEFFECTM_H
#define IOPVALUEEFFECTM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"

#include "model/iop/agentiopm.h"
#include "model/scenario/actioneffectm.h"




/**
 * @brief The IOPValueEffectM class defines an action effect on iop value
 */
class IOPValueEffectM: public ActionEffectM
{
    Q_OBJECT

    // Agent IOP
    I2_QML_PROPERTY(AgentIOPM* , agentIOP)

    // value converted into string
    I2_QML_PROPERTY(QString, value)

    // Concatened list of iop agents items
    I2_QOBJECT_LISTMODEL(AgentIOPM , agentIopList)

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
    * @brief Custom setter on set agent model
    *        to fill inputs and outputs
    * @param agentModel
    */
    void setagentModel(AgentInMappingVM* agentModel);



Q_SIGNALS:


public Q_SLOTS:


protected:



};

QML_DECLARE_TYPE(IOPValueEffectM)

#endif // IOPVALUEEFFECTM_H
