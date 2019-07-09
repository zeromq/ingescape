/*
 *	IngeScape Expe
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

#ifndef PLATFORMM_H
#define PLATFORMM_H

#include <QObject>
#include <I2PropertyHelpers.h>


/**
 * @brief The PlatformM class defines a model of IngeScape platform
 */
class PlatformM : public QObject
{
    Q_OBJECT

    // Name of our platform
    I2_QML_PROPERTY_READONLY(QString, name)

    // File path of our platform
    I2_QML_PROPERTY_READONLY(QString, filePath)


public:

    /**
     * @brief Constructor
     * @param name
     * @param filePath
     * @param parent
     */
    explicit PlatformM(QString name,
                       QString filePath,
                       QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~PlatformM();


};

QML_DECLARE_TYPE(PlatformM)

#endif // PLATFORMM_H
