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

#ifndef LOGM_H
#define LOGM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <model/enums.h>


/**
 * @brief The LogM class defines a model of log
 */
class LogM : public QObject
{
    Q_OBJECT

    // Date and time of our log
    I2_QML_PROPERTY_READONLY(QDateTime, logDateTime)

    // Type of our log
    I2_QML_PROPERTY_READONLY(LogTypes::Value, logType)

    // Content of our log
    I2_QML_PROPERTY_READONLY(QString, logContent)


public:
    /**
     * @brief Constructor
     * @param logDateTime
     * @param logType
     * @param logContent
     * @param parent
     */
    explicit LogM(QDateTime logDateTime,
                  LogTypes::Value logType,
                  QString logContent,
                  QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~LogM();

};

QML_DECLARE_TYPE(LogM)

#endif // LOGM_H
