/*
 *	ActionVM
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

#ifndef ACTIONVM_H
#define ACTIONVM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"

#include "model/scenario/actionm.h"


/**
 * @brief The ActionVM class defines an action view model
 */
class ActionVM: public QObject
{
    Q_OBJECT

    // Action model
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(ActionM*, actionModel)

    // Start time in seconds
    I2_CPP_PROPERTY(int, startTime)

    // Start date time in string format
    I2_QML_PROPERTY_CUSTOM_SETTER(QString, startTimeString)

    // Color
    I2_QML_PROPERTY(QColor, color)

    // Line number in timeline
    I2_QML_PROPERTY(int, lineInTimeLine)

    // Is valid flag
    I2_QML_PROPERTY(bool, isValid)

public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit ActionVM(ActionM* actionModel, int startTime, QObject *parent = 0);

    /**
      * @brief Destructor
      */
    ~ActionVM();

    /**
     * @brief Copy from another action view model
     * @param action VM to copy
     */
    void copyFrom(ActionVM* actionVM);

Q_SIGNALS:


public Q_SLOTS:

    /**
     * @brief Slot on the is valid flag change on the action Model
     * @param is valid flag
     */
    void onActionIsValidChange(bool isValid);

protected:



};

QML_DECLARE_TYPE(ActionVM)

#endif // ACTIONVM_H
