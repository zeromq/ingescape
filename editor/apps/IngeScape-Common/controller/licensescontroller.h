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
#include "model/licenseinformationm.h"


/**
 * @brief The LicensesController class defines the controller to manage IngeScape licenses
 */
class LicensesController : public QObject
{
    Q_OBJECT

    // Path to the directory with IngeScape licenses
    I2_QML_PROPERTY_READONLY(QString, licensesPath)

    // Error message when the IngeScape license is not valid
    I2_QML_PROPERTY_READONLY(QString, errorMessageWhenLicenseFailed)

    // Flag indicating if the IngeScape license is valid
    I2_QML_PROPERTY_READONLY(bool, isLicenseValid)

    //
    I2_CPP_NOSIGNAL_PROPERTY(QString, licenseId)

    //
    I2_QML_PROPERTY_READONLY(QString, licenseCustomer)

    //
    I2_QML_PROPERTY_READONLY(QString, licenseOrder)

    //
    I2_QML_PROPERTY_READONLY(QDate, licenseExpirationDate)

    // Flag indicating if the license of our editor is valid
    I2_QML_PROPERTY_READONLY(bool, isEditorLicenseValid)

    //
    I2_QML_PROPERTY_READONLY(QString, editorOwner)

    //
    I2_QML_PROPERTY_READONLY(QDate, editorExpirationDate)

    // Max number of agents in the platform
    I2_QML_PROPERTY_READONLY(int, maxNbOfAgents)

    // Max number of agents I/O/P in the platform
    I2_QML_PROPERTY_READONLY(int, maxNbOfIOPs)

    // Names of each feature
    I2_QML_PROPERTY(QStringList, featureNames)

    // Names of each agent
    I2_QML_PROPERTY(QStringList, agentNames)

    // FIXME TODO: list of licenseM

    // Merged license information
    I2_QML_PROPERTY(LicenseInformationM*, mergedLicense)

    // List of detailed licenses (one for each file)
    I2_QOBJECT_LISTMODEL(LicenseInformationM, licenseDetailsList)


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


    /**
     * @brief Delete the given license from the platform and from the filesystem
     * @param licenseInformation
     * @return
     */
    Q_INVOKABLE bool deleteLicense(LicenseInformationM* licenseInformation);


    /**
     * @brief Copy the license file from the given path the the current license directory
     * then refresh the global license information
     * @param licenseFilePath
     * @return
     */
    Q_INVOKABLE bool addLicenses(const QList<QUrl>& licenseUrlList);


Q_SIGNALS:

    /**
     * @brief Signal emitted when the licenses have been updated
     */
    void licensesUpdated();


private:

    /**
     * @brief Get the data about licenses
     */
    void _getLicensesData();


};

QML_DECLARE_TYPE(LicensesController)

#endif // LICENSESCONTROLLER_H
