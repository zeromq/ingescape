/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef LICENSESCONTROLLER_H
#define LICENSESCONTROLLER_H

#include <QObject>

#include <I2PropertyHelpers.h>

/**
 * @brief The LicensesController class defines the controller to manage IngeScape licenses
 */
class LicensesController : public QObject
{
    Q_OBJECT

    // Path to the directory with IngeScape licenses
    I2_QML_PROPERTY_READONLY(QString, licensesPath)

    // Flag indicating if the license is valid for our editor
    I2_QML_PROPERTY_READONLY(bool, isValidLicense)


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit LicensesController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~LicensesController();


    /**
     * @brief Select a directory with IngeScape licenses
     * @return
     */
    Q_INVOKABLE QString selectLicensesDirectory();


    /**
     * @brief Update the licenses path
     * @param newLicensesPath
     */
    Q_INVOKABLE void updateLicensesPath(QString newLicensesPath);


Q_SIGNALS:

    /**
     * @brief Signal emitted when the licenses have been updated
     */
    void licensesUpdated();


};

QML_DECLARE_TYPE(LicensesController)

#endif // LICENSESCONTROLLER_H
