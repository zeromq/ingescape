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

#ifndef ACTIONCONDITIONVM_H
#define ACTIONCONDITIONVM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include <model/scenario/condition/actionconditionm.h>
#include <model/scenario/condition/iopvalueconditionm.h>
#include <model/scenario/condition/conditiononagentm.h>


/**
  * Types of Action Condition
  */
I2_ENUM_CUSTOM(ActionConditionTypes, VALUE, AGENT)


/**
 * @brief The ActionConditionVM class defines a view model of action condition
 */
class ActionConditionVM: public QObject
{
    Q_OBJECT

    // Condition model
    I2_QML_PROPERTY(ActionConditionM*, modelM)

    // Action condition type
    I2_QML_PROPERTY_CUSTOM_SETTER(ActionConditionTypes::Value, conditionType)


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit ActionConditionVM(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~ActionConditionVM();



Q_SIGNALS:


public Q_SLOTS:

private:

    /**
     * @brief Configure action condition VM into a specific type
     */
    void _configureToType(ActionConditionTypes::Value value);

protected:



};

QML_DECLARE_TYPE(ActionConditionVM)

#endif // ACTIONCONDITIONVM_H
