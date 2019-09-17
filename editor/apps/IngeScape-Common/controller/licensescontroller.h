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


    /**
     * @brief Open a file dialog to import a license file.
     */
    Q_INVOKABLE void importLicense();


    /**
     * @brief Get the data about licenses
     */
    void refreshLicensesData();


Q_SIGNALS:
    /**
     * @brief Triggered to update licenses data
     */
    void needsUpdate();


    /**
     * @brief Signal emitted when the licenses have been updated
     */
    void licensesUpdated();

    /**
     * @brief Signal emited when the license has reached a limitation (timeour, agents, IOPs)
     */
    void licenseLimitationReached();


protected Q_SLOTS:
    /**
     * @brief Called when our needsUpdate signal is triggered
     */
    void _onNeedsUpdate();


private:


    /**
     * @brief Import (copy) the given license file to the license directory
     * @param licenseFile
     * @return true on success. false otherwise.
     */
    bool _importLicenseFromFile(const QFileInfo& licenseFile);


};

QML_DECLARE_TYPE(LicensesController)

#endif // LICENSESCONTROLLER_H
