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
#include "model/scenario/actioneffectm.h"




/**
 * @brief The IOPValueEffectM class defines an action effect on iop value
 */
class IOPValueEffectM: public ActionEffectM
{
    Q_OBJECT

    // Agent IOP
    I2_QML_PROPERTY(AgentIOPM* , agentIOP)

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



Q_SIGNALS:


public Q_SLOTS:


protected:



};

QML_DECLARE_TYPE(IOPValueEffectM)

#endif // IOPVALUEEFFECTM_H
