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

#ifndef ACTIONEFFECTVM_H
#define ACTIONEFFECTVM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>

#include <model/scenario/effect/actioneffectm.h>
#include <model/scenario/effect/effectonagentm.h>
#include <model/scenario/effect/iopvalueeffectm.h>
#include <model/scenario/effect/mappingeffectm.h>

#include "I2PropertyHelpers.h"


/**
  * Types of Action Effect : AGENT, VALUE, MAPPING
  */
I2_ENUM_CUSTOM(ActionEffectTypes, VALUE, MAPPING, AGENT)


/**
 * @brief The ActionEffectVM class defines an action effect view model
 */
class ActionEffectVM : public QObject
{
    Q_OBJECT

    // Effect model
    I2_QML_PROPERTY(ActionEffectM*, modelM)

    // Type of action effect
    I2_QML_PROPERTY_CUSTOM_SETTER(ActionEffectTypes::Value, effectType)

    // FIXME ???
    // [Optional] second agent in mapping to initalize the combobox
    I2_QML_PROPERTY(AgentInMappingVM*, secondAgentInMapping)


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit ActionEffectVM(QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~ActionEffectVM();


    /**
     * @brief Get the command and parameters of our effect
     * @return
     */
    //QPair<QString, QStringList> getCommandAndParameters();


Q_SIGNALS:


public Q_SLOTS:


private:
    /**
     * @brief Configure action effect VM into a specific type
     */
    void _configureToType(ActionEffectTypes::Value effectType);


};

QML_DECLARE_TYPE(ActionEffectVM)

#endif // ACTIONEFFECTVM_H
