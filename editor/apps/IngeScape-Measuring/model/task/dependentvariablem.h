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

#ifndef DEPENDENTVARIABLEM_H
#define DEPENDENTVARIABLEM_H

#include <QObject>
#include <I2PropertyHelpers.h>


/**
 * @brief The DependentVariableM class defines a model of dependent variable
 */
class DependentVariableM : public QObject
{
    Q_OBJECT

    // Name of our dependent variable
    I2_QML_PROPERTY(QString, name)

    // Description of our dependent variable
    I2_QML_PROPERTY(QString, description)

    // Unit of our dependent variable
    //I2_QML_PROPERTY(QString, unit)

    // Name of the agent in the platform associated to the task
    I2_QML_PROPERTY(QString, agentName)

    // Name of the (agent) output in the platform associated to the task
    I2_QML_PROPERTY(QString, outputName)

    // Frequency (double ? (nb répétitions par secondes Hertz))


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit DependentVariableM(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~DependentVariableM();


Q_SIGNALS:


public Q_SLOTS:


private:


};

QML_DECLARE_TYPE(DependentVariableM)

#endif // DEPENDENTVARIABLEM_H
