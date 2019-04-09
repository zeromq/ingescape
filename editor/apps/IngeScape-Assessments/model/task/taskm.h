/*
 *	IngeScape Assessments
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef TASKM_H
#define TASKM_H

#include <QObject>
#include "I2PropertyHelpers.h"

#include <model/task/dependentvariablem.h>
#include <model/task/independentvariablem.h>


/**
 * @brief The TaskM class defines a model of task
 */
class TaskM : public QObject
{
    Q_OBJECT

    // Name of our task
    I2_QML_PROPERTY(QString, name)

    // JSON of Platform
    //I2_QML_PROPERTY(QString, ...)

    // List of independent variables
    //I2_QOBJECT_LISTMODEL(IndependentVariableM, independentVariables)

    // List of dependent variables
    //I2_QOBJECT_LISTMODEL(DependentVariableM, dependentVariables)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit TaskM(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~TaskM();


Q_SIGNALS:


public Q_SLOTS:


private:


};

QML_DECLARE_TYPE(TaskM)

#endif // TASKM_H
