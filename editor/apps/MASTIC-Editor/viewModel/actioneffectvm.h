
/*
 *	ActionEffectVM
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

#ifndef ACTIONEFFECTVM_H
#define ACTIONEFFECTVM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include "model/scenario/actioneffectm.h"
#include "model/scenario/iopvalueeffectm.h"
#include "model/scenario/mappingeffectm.h"

/**
  * Action effect type : AGENT, VALUE, MAPPING
  */
I2_ENUM_CUSTOM(ActionEffectType, AGENT, VALUE, MAPPING)


/**
 * @brief The ActionEffectVM class defines the main controller of our application
 */
class ActionEffectVM: public QObject
{
    Q_OBJECT

    // Effect model
    I2_QML_PROPERTY(ActionEffectM*, effect)

    // Action effect type
    I2_QML_PROPERTY_CUSTOM_SETTER(ActionEffectType::Value, effectType)

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



Q_SIGNALS:


public Q_SLOTS:

private :
    /**
     * @brief Configure action effect VM into a specific type
     */
    void _configureToType(ActionEffectType::Value value);

protected:



};

QML_DECLARE_TYPE(ActionEffectVM)

#endif // ACTIONEFFECTVM_H
