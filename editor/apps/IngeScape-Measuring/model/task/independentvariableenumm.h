/*
 *	IngeScape Measuring
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

#ifndef INDEPENDENTVARIABLEENUMM_H
#define INDEPENDENTVARIABLEENUMM_H

#include <QObject>
#include <I2PropertyHelpers.h>


/**
 * @brief The IndependentVariableEnumM class defines a model of enumeration for an independent variable
 * Allows to manage the list of enumerations for an independent variable
 */
class IndependentVariableEnumM : public QObject
{
    Q_OBJECT

    // Name of our independent variable enumeration
    I2_QML_PROPERTY(QString, name)

    // List of possible values
    I2_QML_PROPERTY(QStringList, values)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit IndependentVariableEnumM(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~IndependentVariableEnumM();


Q_SIGNALS:


public Q_SLOTS:


private:


};

QML_DECLARE_TYPE(IndependentVariableEnumM)

#endif // INDEPENDENTVARIABLEENUMM_H
