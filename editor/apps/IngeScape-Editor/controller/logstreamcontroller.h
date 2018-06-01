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
#include <model/logm.h>

extern "C" {
//#include <ingescape_advanced.h>
#include <ingescape_private.h>
//#include <czmq.h>
}


/**
 * @brief The LogStreamController class defines the controller to manage log stream of an agent
 */
class LogStreamController : public QObject
{
    Q_OBJECT

    // Name of the corresponding agent
    I2_QML_PROPERTY_READONLY(QString, agentName)

    // Host name of the corresponding agent
    I2_QML_PROPERTY_READONLY(QString, agentHostname)

    // Address of subscriber
    I2_CPP_NOSIGNAL_PROPERTY(QString, subscriberAddress)

    // List of filtered logs
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(LogM, filteredLogs)

    // List with all log types
    I2_ENUM_LISTMODEL(LogTypes, allLogTypes)

    // List with selected log types
    I2_ENUM_LISTMODEL(LogTypes, selectedLogTypes)


public:
    /**
     * @brief Constructor
     * @param agentName
     * @param agentHostname
     * @param subscriberAddress
     * @param parent
     */
    explicit LogStreamController(QString agentName,
                                 QString agentHostname,
                                 QString subscriberAddress,
                                 QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~LogStreamController();


    /**
     * @brief Return true if the "Log Type" is selected
     * @param nLogType
     * @return
     */
    Q_INVOKABLE bool isSelectedLogType(int nLogType);


    /**
     * @brief Show logs of the type
     * @param nLogType
     */
    Q_INVOKABLE void showLogsOfType(int nLogType);


    /**
     * @brief Hide logs of the type
     * @param nLogType
     */
    Q_INVOKABLE void hideLogsOfType(int nLogType);


    /**
     * @brief Show all logs (select all log types)
     */
    Q_INVOKABLE void showAllLogs();


    /**
     * @brief Hide all logs (un-select all log types)
     */
    Q_INVOKABLE void hideAllLogs();


signals:

    /**
     * @brief Signal emitted when a log has been received from the agent
     * @param logDateTime
     * @param parametersOfLog
     */
    void logReceived(QDateTime logDateTime, QStringList parametersOfLog);


private slots:

    /**
     * @brief Slot called when a log has been received from the agent
     * @param logDateTime
     * @param parametersOfLog
     */
    void _onLogReceived(QDateTime logDateTime, QStringList parametersOfLog);


private:
    // zactor
    zactor_t *_zActor;

};

QML_DECLARE_TYPE(LogStreamController)

#endif // LOGSTREAMCONTROLLER_H
