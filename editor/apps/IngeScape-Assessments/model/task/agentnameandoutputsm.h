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

#ifndef AGENTNAMEANDOUTPUTSM_H
#define AGENTNAMEANDOUTPUTSM_H

#include <QObject>
#include "I2PropertyHelpers.h"


/**
 * @brief The AgentNameAndOutputsM class defines a (simplified) model of agent with its name and its outputs
 */
class AgentNameAndOutputsM : public QObject
{
    Q_OBJECT

    // Name of our agent
    I2_QML_PROPERTY(QString, agentName)

    // List of output names of our agent
    I2_QML_PROPERTY_READONLY(QStringList, outputNamesList)


public:

    /**
     * @brief Constructor
     * @param agentName
     * @param outputNamesList
     * @param parent
     */
    explicit AgentNameAndOutputsM(QString agentName,
                                  QStringList outputNamesList,
                                  QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentNameAndOutputsM();


};

QML_DECLARE_TYPE(AgentNameAndOutputsM)

#endif // AGENTNAMEANDOUTPUTSM_H
