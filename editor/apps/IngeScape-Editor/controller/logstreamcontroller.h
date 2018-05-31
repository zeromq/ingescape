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

#ifndef LOGSTREAMCONTROLLER_H
#define LOGSTREAMCONTROLLER_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>


/**
 * @brief The LogStreamController class defines the controller to manage log stream of an agent
 */
class LogStreamController : public QObject
{
    Q_OBJECT

    // Name of the corresponding agent
    I2_QML_PROPERTY_READONLY(QString, agentName)

    // Address of subscriber
    I2_CPP_NOSIGNAL_PROPERTY(QString, subscriberAddress)

    // List of logs
    //I2_QOBJECT_LISTMODEL()
    I2_QML_PROPERTY(QStringList, logs)


public:
    /**
     * @brief Constructor
     * @param agentName
     * @param subscriberAddress
     * @param parent
     */
    explicit LogStreamController(QString agentName,
                                 QString subscriberAddress,
                                 QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~LogStreamController();


signals:

    /**
     * @brief Signal emitted when a log has been received from the agent
     * @param log
     */
    void logReceived(QString log);


private slots:

    /**
     * @brief Slot called when a log has been received from the agent
     * @param log
     */
    void _onLogReceived(QString log);


private:

};

QML_DECLARE_TYPE(LogStreamController)

#endif // LOGSTREAMCONTROLLER_H
