
/*
 *	ActionConditionVM
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

#ifndef ACTIONCONDITIONVM_H
#define ACTIONCONDITIONVM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include "model/scenario/actionconditionm.h"
#include "model/scenario/iopvalueconditionm.h"

/**
  * Action condition type : AGENT, VALUE
  */
I2_ENUM_CUSTOM(ActionConditionType, AGENT, VALUE)


/**
 * @brief The ActionConditionVM class defines an action condition view model
 */
class ActionConditionVM: public QObject
{
    Q_OBJECT

    // Condition model
    I2_QML_PROPERTY(ActionConditionM*, condition)

    // Action condition type
    I2_QML_PROPERTY_CUSTOM_SETTER(ActionConditionType::Value, conditionType)


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit ActionConditionVM(QObject *parent = 0);


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
    void _configureToType(ActionConditionType::Value value);

protected:



};

QML_DECLARE_TYPE(ActionConditionVM)

#endif // ACTIONCONDITIONVM_H
