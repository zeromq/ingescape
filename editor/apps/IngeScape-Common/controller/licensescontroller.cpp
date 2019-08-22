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

#include "licensescontroller.h"
#include <misc/ingescapeutils.h>
#include <settings/ingescapesettings.h>
#include <QFileDialog>


/**
 * @brief onLicenseCallback
 * @param limit
 * @param myData
 */
void onLicenseCallback(igs_license_limit_t limit, void *myData)
{
    LicensesController* licensesController = (LicensesController*)myData;
    if (licensesController != nullptr)
    {
        // Emit the signal "License Error Occured"
        Q_EMIT licensesController->licenseErrorOccured("TODO");

        switch (limit)
        {
        case IGS_LICENSE_TIMEOUT:
            qCritical("IngeScape is stopped because demonstration mode timeout has been reached");
            break;

        case IGS_LICENSE_TOO_MANY_AGENTS:
            qCritical("IngeScape is stopped because too many agents are running on the platform compared to what the license allows");
            break;

        case IGS_LICENSE_TOO_MANY_IOPS:
            qCritical("IngeScape is stopped because too many IOPs have been created on the platform compared to what the license allows");
            break;

        default:
            break;
        }
    }
}


//--------------------------------------------------------------
//
// Licenses Controller
//
//--------------------------------------------------------------

/**
 * @brief Constructor
 * @param parent
 */
LicensesController::LicensesController(QObject *parent) : QObject(parent),
    _licensesPath(""),
    _isValidLicense(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);


    //
    // Settings
    //
    IngeScapeSettings &settings = IngeScapeSettings::Instance();

    settings.beginGroup("licenses");

    // Get the default path for "Licenses"
    QString defaultLicensesPath = IngeScapeUtils::getLicensesPath();

    _licensesPath = settings.value("directoryPath", QVariant(defaultLicensesPath)).toString();

    settings.endGroup();

    qInfo() << "New Licenses Controller with licenses path" << _licensesPath;

    // Begin to observe license events (events are triggered only when no valid license is available)
    igs_observeLicense(onLicenseCallback, this);

    // Set the IngeScape license path
    igs_setLicensePath(_licensesPath.toStdString().c_str());

    // Get the data about licenses
    _getLicensesData();

}


/**
 * @brief Destructor
 */
LicensesController::~LicensesController()
{
    qInfo() << "Delete Licenses Controller";

}


/**
 * @brief Select a directory with IngeScape licenses
 * @return
 */
QString LicensesController::selectLicensesDirectory()
{
    // Open a directory dialog box
    return QFileDialog::getExistingDirectory(nullptr,
                                             "Select a directory with IngeScape licenses",
                                             _licensesPath);
}


/**
 * @brief Update the licenses path
 * @param newLicensesPath
 */
void LicensesController::updateLicensesPath(QString newLicensesPath)
{
    // Licenses path has been changed
    if (newLicensesPath != _licensesPath)
    {
        qInfo() << "Licenses path changes from" << _licensesPath << "to" << newLicensesPath;

        // Update property
        setlicensesPath(newLicensesPath);

        // Update settings file
        IngeScapeSettings &settings = IngeScapeSettings::Instance();
        settings.beginGroup("licenses");
        settings.setValue("directoryPath", _licensesPath);
        settings.endGroup();

        // Save new value
        settings.sync();

        // Set the IngeScape license path
        igs_setLicensePath(_licensesPath.toStdString().c_str());

        // Get the data about licenses
        _getLicensesData();

        // Emit the signal "Licenses Updated"
        Q_EMIT licensesUpdated();
    }
}


/**
 * @brief Get the data about licenses
 */
void LicensesController::_getLicensesData()
{
    // Allows to update data about licenses
    igs_checkLicenseForAgent(nullptr);

    if (license != nullptr)
    {
        QDateTime licenseExpirationDate = QDateTime::fromSecsSinceEpoch(license->licenseExpirationDate);
        QDateTime editorExpirationDate = QDateTime::fromSecsSinceEpoch(license->editorExpirationDate);

        if (license->isLicenseValid && license->isEditorLicenseValid)
        {
            qInfo() << "VALID License: id" << QString(license->id) << "order" << QString(license->order) << "customer" << QString(license->customer) << "licenseExpirationDate" << licenseExpirationDate;
            qInfo() << "VALID EDITOR License: editorOwner" << QString(license->editorOwner) << "editorExpirationDate" << editorExpirationDate;
            qDebug() << "Nb MAX Agents" << license->platformNbAgents << "Nb MAX IOPs" << license->platformNbIOPs;

            // Update flag
            setisValidLicense(true);
        }
        else
        {
            if (!license->isLicenseValid) {
                qInfo() << "IN-valid License: id" << QString(license->id) << "order" << QString(license->order) << "customer" << QString(license->customer) << "licenseExpirationDate" << licenseExpirationDate;
            }
            else { //if (!license->isEditorLicenseValid) {
                qInfo() << "IN-valid EDITOR License: editorOwner" << QString(license->editorOwner) << "editorExpirationDate" << editorExpirationDate;
            }
            qDebug() << "Nb MAX Agents" << QString(license->platformNbAgents) << "Nb MAX IOPs" << QString(license->platformNbIOPs);

            // Update flag
            setisValidLicense(false);
        }
    }
}
