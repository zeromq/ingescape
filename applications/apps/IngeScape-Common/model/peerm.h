/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2020 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef PEERM_H
#define PEERM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include "model/enums.h"

class PeerM : public QObject
{
    Q_OBJECT

    I2_QML_PROPERTY_READONLY(IngeScapeTypes::Value, igsType)
    I2_QML_PROPERTY_READONLY(QString, uid)
    I2_QML_PROPERTY_READONLY(QString, name)
    I2_QML_PROPERTY_READONLY(QString, ipAddress)
    I2_QML_PROPERTY_READONLY(QString, hostname)
    I2_QML_PROPERTY(QString, commandLine)

    I2_QML_PROPERTY_READONLY(bool, isON)

    I2_CPP_PROPERTY(QString, loggerPort)
    I2_CPP_PROPERTY(QString, streamingPort)


public:
    explicit PeerM(IngeScapeTypes::Value igsType,
                   QString uid,
                   QString name,
                   QString ipAddress,
                   QString hostname,
                   QString commandLine,
                   bool isON = true,
                   QObject *parent = nullptr);
    ~PeerM();


};

QML_DECLARE_TYPE(PeerM)

#endif // PEERM_H
