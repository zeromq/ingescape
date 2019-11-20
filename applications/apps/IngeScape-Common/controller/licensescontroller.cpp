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

extern "C" {
#include <ingescape.h>
//#include <ingescape_advanced.h>
#include <ingescape_private.h>
//#include <ingescape_agent.h>
}


/**
 * @brief onLicenseCallback
 * @param limit
 * @param myData
 */
void onLicenseCallback(igs_license_limit_t limit, void *myData)
{
    LicensesController* licensesController = static_cast<LicensesController*>(myData);
    if (licensesController != nullptr)
    {
        switch (limit)
        {
        case IGS_LICENSE_TIMEOUT:
            qCritical("The editor has reached demonstration timeout and is now stopped.");
            licensesController->seterrorMessageWhenLicenseFailed("The editor has reached demonstration timeout and is now stopped.");
            break;

        case IGS_LICENSE_TOO_MANY_AGENTS:
            qCritical("The editor has reached maximum number of agents allowed with your current license and is now stopped.");
            licensesController->seterrorMessageWhenLicenseFailed("The editor has reached maximum number of agents allowed with your current license and is now stopped.");
            break;

        case IGS_LICENSE_TOO_MANY_IOPS:
            qCritical("The editor has reached maximum number of IOPs allowed with your current license and is now stopped.");
            licensesController->seterrorMessageWhenLicenseFailed("The editor has reached maximum number of IOPs allowed with your current license and is now stopped.");
            break;

        default:
            break;
        }

        // Update data
        licensesController->needsUpdate();
        Q_EMIT licensesController->licenseLimitationReached();
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
    _errorMessageWhenLicenseFailed(""),
    _mergedLicense(nullptr)
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

    // Subcribe to updates
    connect(this, &LicensesController::needsUpdate, this, &LicensesController::_onNeedsUpdate);

    // Begin to observe license events (events are triggered only when no valid license is available)
    igs_observeLicense(onLicenseCallback, this);

    // Set the IngeScape license path
    igs_setLicensePath(_licensesPath.toStdString().c_str());

    // Get the data about licenses
    refreshLicensesData();
}


/**
 * @brief Destructor
 */
LicensesController::~LicensesController()
{
    qInfo() << "Delete Licenses Controller";

    // Unsubcribe to updates
    disconnect(this, &LicensesController::needsUpdate, this, &LicensesController::_onNeedsUpdate);

    // Clean-up merged license
    if (_mergedLicense != nullptr)
    {
        LicenseInformationM* temp = _mergedLicense;
        setmergedLicense(nullptr);
        delete temp;
    }

    // Clean-up license details
    _licenseDetailsList.deleteAllItems();
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
        refreshLicensesData();

        // Emit the signal "Licenses Updated"
        Q_EMIT licensesUpdated();
    }
}


/**
 * @brief Delete the given license from the platform and from the filesystem
 * @param licenseInformation
 * @return
 */
bool LicensesController::deleteLicense(LicenseInformationM* licenseInformation)
{
    QDir licenseDirectory(_licensesPath);
    if (licenseDirectory.exists() && licenseDirectory.remove(licenseInformation->fileName()))
    {
        // Success
        refreshLicensesData();
        return true;
    }

    // Failure
    qDebug() << "Unable to delete the license file.";
    return false;
}


/**
 * @brief Copy the license file from the given path the the current license directory
 * then refresh the global license information
 * @param licenseFilePath
 * @return
 */
bool LicensesController::addLicenses(const QList<QUrl>& licenseUrlList)
{
    bool completeSuccess = true;

    // Are all URLs corresponding to local files ?
    for (QUrl licenseUrl : licenseUrlList)
    {
        if (licenseUrl.isLocalFile())
        {
            completeSuccess &= _importLicenseFromFile(QFileInfo(licenseUrl.toLocalFile()));
        }
        else
        {
            completeSuccess = false;
        }
    }

    refreshLicensesData();

    return completeSuccess;
}


/**
 * @brief Open a file dialog to import a license file.
 */
void LicensesController::importLicense()
{
    // "File Dialog" to get the files (paths) to open
    QString platformFilePath = QFileDialog::getOpenFileName(nullptr,
                                                            "Import IGS license",
                                                            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                            "IGS License (*.igslicense)");

    if (!platformFilePath.isEmpty())
    {
        QFileInfo licenseFile(platformFilePath);
        bool importFromLicensePath = licenseFile.absoluteDir() == _licensesPath;

        // Do not import from the license path
        if (!importFromLicensePath && _importLicenseFromFile(licenseFile))
        {
            refreshLicensesData();
        }
    }

}


/**
 * @brief Get the data about licenses
 */
void LicensesController::refreshLicensesData()
{
    // Allows to update data about licenses
    igs_checkLicenseForAgent(nullptr);

    if (internalAgent->license != nullptr)
    {
        if (_mergedLicense != nullptr)
        {
            LicenseInformationM* temp = _mergedLicense;
            setmergedLicense(nullptr);
            delete temp;
        }

        setmergedLicense(new LicenseInformationM(internalAgent->license));
        _licenseDetailsList.deleteAllItems();

        //
        // License details
        //
        if ((internalAgent->license->licenseDetails != nullptr) && (zlist_size(internalAgent->license->licenseDetails) > 0))
        {
            license_t* detail = static_cast<license_t*>(zlist_first(internalAgent->license->licenseDetails));
            while (detail != nullptr)
            {
                LicenseInformationM* licenseDetails = new LicenseInformationM(detail);
                _licenseDetailsList.append(licenseDetails);
                detail = static_cast<license_t*>(zlist_next(internalAgent->license->licenseDetails));
            }
        }
    }
}


/**
 * @brief Import (copy) the given license file to the license directory
 * @param licenseFile
 * @return true on success. false otherwise.
 */
bool LicensesController::_importLicenseFromFile(const QFileInfo& licenseFile)
{
    bool success = false;
    if (licenseFile.exists())
    {
        success = QFile::copy(licenseFile.absoluteFilePath(), QDir(_licensesPath).filePath(licenseFile.fileName()));
        if (!success)
        {
            qDebug() << "Unable to copy" << licenseFile.fileName() << "to the license directory";
        }
    }
    return success;
}


/**
 * @brief Called when our needsUpdate signal is triggered
 */
void LicensesController::_onNeedsUpdate()
{
    refreshLicensesData();
}
