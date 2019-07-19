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
  * States for the record
  */
I2_ENUM(RecordStates, NOT_RECORDED, RECORDING, RECORDED)


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

    // Flag indicating if our platform is loaded (in the "Editor app")
    I2_QML_PROPERTY_READONLY(bool, isLoaded)

    // Current state about record
    I2_QML_PROPERTY_READONLY(RecordStates::Value, recordState)

    // Current index of our platform in the list (for sorting)
    I2_QML_PROPERTY_READONLY(int, currentIndex)

    // Index of our platform in the list while sorted by alphabetical order
    I2_QML_PROPERTY_READONLY(int, indexOfAlphabeticOrder)


public:

    /**
     * @brief Constructor
     * @param name
     * @param filePath
     * @param parent
     */
    explicit PlatformM(QString name,
                       QString filePath,
                       int indexOfAlphabeticOrder,
                       QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~PlatformM();


private:


};

QML_DECLARE_TYPE(PlatformM)

#endif // PLATFORMM_H
