
/*
 *	ActionM
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

#ifndef ACTIONM_H
#define ACTIONM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"

#include "model/scenario/actioneffectm.h"
#include "model/scenario/actionconditionm.h"


/**
 * @brief The ActionM class defines the main controller of our application
 */
class ActionM: public QObject
{
    Q_OBJECT

    // Action name
    I2_QML_PROPERTY(QString, name)

    // Start time in milliseconds
    I2_QML_PROPERTY(int, startTime)

    // Validity duration in milliseconds
    I2_QML_PROPERTY(int, validityDuration)

    // Flag to revert the action
    I2_QML_PROPERTY(bool, shallRevert)

    // Flag to revert action when validity is over
    I2_QML_PROPERTY(bool, revertWhenValidityIsOver)

    // Time in milliseconds when to revert
    I2_QML_PROPERTY(int, revertAtTime)

    // Flag to rearm the action
    I2_QML_PROPERTY(bool, shallRearm)

    // FIXME : Liste des temps de déclenchement (1 ou plus si réarmable) >> VP

public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit ActionM(QString name, QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~ActionM();



Q_SIGNALS:


public Q_SLOTS:


protected:



};

QML_DECLARE_TYPE(ActionM)

#endif // ACTIONM_H
