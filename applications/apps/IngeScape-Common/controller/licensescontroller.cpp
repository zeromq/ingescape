/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2020 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Chloé Roumieu      <roumieu@ingenuity.io>
 */

#include "licensescontroller.h"
#include <misc/ingescapeutils.h>
#include <settings/ingescapesettings.h>
#include <QFileDialog>

extern "C" {
#include "ingescape_private.h"
}


/**
 * @brief onLicenseCallback
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
 */
LicensesController::LicensesController(QObject *parent) : QObject(parent),
    _licensesPath(""),
    _errorMessageWhenLicenseFailed(""),
    _mergedLicense(nullptr),
    _isLicenseValidForAgentNeeded(true),
    _licenseForAgentNeeded(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Get license path from user's .ini file
    IngeScapeSettings &settings = IngeScapeSettings::Instance();
    settings.beginGroup("licenses");
    QString defaultLicensesPath = IngeScapeUtils::getLicensesPath(); // Get the default path for "Licenses"
    _licensesPath = settings.value("directoryPath", QVariant(defaultLicensesPath)).toString();
    settings.endGroup();

    // Subcribe to updates
    connect(this, &LicensesController::needsUpdate, this, &LicensesController::_onNeedsUpdate);

    // Begin to observe license events (events are triggered only when no valid license is available)
    igs_observeLicense(onLicenseCallback, this);

    // Set the IngeScape license path
    igs_setLicensePath(_licensesPath.toStdString().c_str());

    // Get the data about licenses
    refreshLicensesData();

    qInfo() << "New Licenses Controller with licenses path" << _licensesPath;
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

    if (_licenseForAgentNeeded != nullptr)
    {
        _cleanupLicenseForAgentNeeded();
    }
}


/**
 * @brief Select a directory with IngeScape licenses
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
 */
void LicensesController::updateLicensesPath(QString newLicensesPath)
{
    // Licenses path has been changed
    if (newLicensesPath != _licensesPath)
    {
        qInfo() << "Licenses path changes from" << _licensesPath << "to" << newLicensesPath;

        // Reset error message
        seterrorMessageWhenLicenseFailed("");

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
 */
bool LicensesController::deleteLicense(LicenseInformationM* licenseInformation)
{
    bool success(false);
    if (licenseInformation != nullptr)
    {
        QDir licenseDirectory(_licensesPath);
        if (licenseDirectory.exists() && licenseDirectory.remove(licenseInformation->fileName()))
        {
            // Success
            refreshLicensesData();
            success = true;
        }
        else
        {
            // Failure
            qDebug() << "Unable to delete the license file.";
        }
    }
    return success;
}


/**
 * @brief Copy the license file from the given path the the current license directory
 * then refresh the global license information
 */
bool LicensesController::addLicenses(const QList<QUrl>& licenseUrlList)
{
    bool completeSuccess = true;

    // Are all URLs corresponding to local files ?
    for (QUrl licenseUrl : licenseUrlList)
    {
        if (licenseUrl.isLocalFile())
        {
            seterrorMessageWhenLicenseFailed("");
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
            seterrorMessageWhenLicenseFailed("");
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
    license_readLicense(coreContext);

    if (coreContext->license != nullptr)
    {
        if (_mergedLicense != nullptr)
        {
            LicenseInformationM* temp = _mergedLicense;
            setmergedLicense(nullptr);
            delete temp;
        }

        setmergedLicense(new LicenseInformationM(coreContext->license));
        _licenseDetailsList.deleteAllItems();

        //
        // License details
        //
        if ((coreContext->license->licenseDetails != nullptr) && (zlist_size(coreContext->license->licenseDetails) > 0))
        {
            igs_license_t* detail = static_cast<igs_license_t*>(zlist_first(coreContext->license->licenseDetails));
            while (detail != nullptr)
            {
                LicenseInformationM* licenseDetails = new LicenseInformationM(detail);
                _licenseDetailsList.append(licenseDetails);
                detail = static_cast<igs_license_t*>(zlist_next(coreContext->license->licenseDetails));
            }
        }

        // Check if user's license has a valid license for agent needed
        if (_licenseForAgentNeeded != nullptr) {
            _checkLicenseForAgentNeeded();
        }
    }
}

/**
 * @brief Set necessary license for agent
 * @param agentName : agent name that is necessary
 * @param agentID : agent's ID that must be present in the license
 */
void LicensesController::setNecessaryLicenseForAgent(const char * agentName, const char * agentID) {
    if (_licenseForAgentNeeded != nullptr) {
        _cleanupLicenseForAgentNeeded();
    }

    // Update name and id of license for agent
    _licenseForAgentNeeded = new igs_license_for_agent_t();
    _licenseForAgentNeeded->agentName = strdup(agentName);
    _licenseForAgentNeeded->agentId = strdup(agentID);

    // Check if user's license has a valid license for agent needed
    _checkLicenseForAgentNeeded();
}


/**
 * @brief Import (copy) the given license file to the license directory
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
 * @brief Clean up _licenseForAgentNeeded property
 */
void LicensesController::_cleanupLicenseForAgentNeeded()
{
    if (_licenseForAgentNeeded != nullptr) {
        // Clean-up previous licenseForAgent_t
        if (_licenseForAgentNeeded->agentName != nullptr)
        {
            free(_licenseForAgentNeeded->agentName);
            _licenseForAgentNeeded->agentName = nullptr;
        }

        if (_licenseForAgentNeeded->agentId != nullptr)
        {
            free(_licenseForAgentNeeded->agentId);
            _licenseForAgentNeeded->agentId = nullptr;
        }

        delete _licenseForAgentNeeded;
    }
}


/**
 * @brief Check license for agent needed
*/
bool LicensesController::_checkLicenseForAgentNeeded() {
    bool agentHandleByLicense = true;
    if ((_licenseForAgentNeeded != nullptr) && (!igs_checkLicenseForAgent(_licenseForAgentNeeded->agentId)))
    {
        seterrorMessageWhenLicenseFailed("No valid license for agent id " + QString(_licenseForAgentNeeded->agentId));
        agentHandleByLicense = false;
    }
    else
    {
        seterrorMessageWhenLicenseFailed("");
        agentHandleByLicense = true;
    }
    setisLicenseValidForAgentNeeded(agentHandleByLicense);
    return agentHandleByLicense;
}


/**
 * @brief Called when our needsUpdate signal is triggered
 */
void LicensesController::_onNeedsUpdate()
{
    refreshLicensesData();
}
