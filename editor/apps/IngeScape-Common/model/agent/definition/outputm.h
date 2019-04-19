/*
 *	IngeScape Common
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef OUTPUTM_H
#define OUTPUTM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <model/agent/definition/agentiopm.h>

/**
 * @brief The OutputM class defines a model of output
 */
class OutputM : public AgentIOPM
{
    Q_OBJECT

    // Flag indicating if our output is muted (for feedbacks on HMI)
    I2_QML_PROPERTY(bool, isMuted)

    // Flag indicating if our output is muted (updated with message from the network)
    I2_QML_PROPERTY_CUSTOM_SETTER(bool, isMutedOutput)

    // Flag indicating if all outputs of the agent are muted (updated with message from the network)
    I2_CPP_PROPERTY_CUSTOM_SETTER(bool, isMutedAllOutputs)


public:

    /**
     * @brief Constructor
     * @param name
     * @param agentIOPValueType
     * @param parent
     */
    explicit OutputM(QString name,
                     AgentIOPValueTypes::Value agentIOPValueType,
                     QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~OutputM();


    /**
     * @brief Mute / UN-mute our output
     */
    Q_INVOKABLE void changeMuteOutput();


Q_SIGNALS:

    /**
     * @brief Signal emitted when a command must be sent on the network
     * @param command
     * @param name
     */
    void commandAsked(QString command, QString name);


private:

    /**
     * @brief Update the flag "Is Muted" (for feedbacks on HMI)
     */
    void _updateIsMuted();


public Q_SLOTS:
};

QML_DECLARE_TYPE(OutputM)

#endif // OUTPUTM_H
