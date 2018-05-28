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


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit LogStreamController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~LogStreamController();


signals:

public slots:
};

QML_DECLARE_TYPE(LogStreamController)

#endif // LOGSTREAMCONTROLLER_H
