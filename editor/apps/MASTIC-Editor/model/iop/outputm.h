/*
 *	MASTIC Editor
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

#ifndef OUTPUTM_H
#define OUTPUTM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <model/iop/agentiopm.h>

/**
 * @brief The OutputM class defines a model of output
 */
class OutputM : public AgentIOPM
{
    Q_OBJECT

    // Flag indicating if our output is muted
    I2_QML_PROPERTY(bool, isMuted)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit OutputM(QObject *parent = nullptr);


    /**
     * @brief Constructor
     * @param name
     * @param agentIOPValueType
     * @param parent
     */
    OutputM(QString name,
            AgentIOPValueTypes::Value agentIOPValueType,
            QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~OutputM();


    /**
     * @brief Mute/UN-mute our output
     * @param mute
     */
    Q_INVOKABLE void updateMute(bool mute);


Q_SIGNALS:

    /**
     * @brief Signal emitted when a command must be sent on the network
     * @param command
     * @param name
     */
    void commandAsked(QString command, QString name);


public Q_SLOTS:
};

QML_DECLARE_TYPE(OutputM)

#endif // OUTPUTM_H
