
/*
 *	IOPValueConditionM
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

#ifndef IOPVALUECONDITIONM_H
#define IOPVALUECONDITIONM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include "actionconditionm.h"



/**
 * @brief The IOPValueConditionM class defines the main controller of our application
 */
class IOPValueConditionM: public ActionConditionM
{
    Q_OBJECT

    // Agent IOP
    I2_QML_PROPERTY(AgentIOPM* , agentIOP)


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit IOPValueConditionM(QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~IOPValueConditionM();



Q_SIGNALS:


public Q_SLOTS:


protected:



};

QML_DECLARE_TYPE(IOPValueConditionM)

#endif // IOPVALUECONDITIONM_H
