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

#ifndef HOSTM_H
#define HOSTM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include "model/peerm.h"


/**
 * @brief The HostM class defines a model of host
 */
class HostM : public QObject
{
    Q_OBJECT

    // Name of our host
    I2_QML_PROPERTY_READONLY(QString, name)

    I2_QML_PROPERTY_CUSTOM_SETTER(PeerM*, peer)

    // Streaming port of the host
    //I2_QML_PROPERTY_READONLY(QString, streamingPort)

    // Flag indicating if our host is ON (vs OFF)
    //I2_QML_PROPERTY_READONLY(bool, isON)


public:
    /**
     * @brief Constructor
     */
    explicit HostM(QString name,
                   PeerM* peer = nullptr,
                   QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~HostM();

};

QML_DECLARE_TYPE(HostM)

#endif // HOSTM_H
