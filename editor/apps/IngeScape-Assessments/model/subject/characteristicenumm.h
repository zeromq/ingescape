/*
 *	IngeScape Assessments
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

#ifndef CHARACTERISTICENUMM_H
#define CHARACTERISTICENUMM_H

#include <QObject>
#include <I2PropertyHelpers.h>


/**
 * @brief The CharacteristicEnumM class defines a model of enumeration for a characteristic
 * Allows to manage the list of enumerations for a characteristic
 */
class CharacteristicEnumM : public QObject
{
    Q_OBJECT

    // Name of our characteristic enumeration
    I2_QML_PROPERTY(QString, name)

    // List of possible values
    I2_QML_PROPERTY(QStringList, values)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit CharacteristicEnumM(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~CharacteristicEnumM();


Q_SIGNALS:


public Q_SLOTS:


private:


};

QML_DECLARE_TYPE(CharacteristicEnumM)

#endif // CHARACTERISTICENUMM_H
