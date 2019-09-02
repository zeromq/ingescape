/*
 *	IngeScape Common
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Mathieu Soum     <soum@ingenuity.io>
 *
 */

#include "licenseinformationm.h"

/**
 * @brief Basic constructor.
 * Analyse the given license_t pointer to populate the properties.
 * @param licenseObject
 * @param parent
 */
LicenseInformationM::LicenseInformationM(const license_t* licenseObject, QObject* parent)
    : QObject (parent)
    , _licenseId("")
    , _customers("")
    , _orderNumbers("")
    , _expirationDate(QDateTime())
    , _ingescapeLicenseValidity(false)
    , _maxNumberOfAgents(0)
    , _maxNumberOfIOPs(0)
    , _editorOwners("")
    , _editorExpirationDate(QDateTime())
    , _editorLicenseValidity(false)
    , _features({})
    , _agents({})
    , _fileName("")
{
    if (licenseObject != nullptr)
    {
        setlicenseId(licenseObject->id);
        setcustomers(licenseObject->customer);
        setorderNumbers(licenseObject->order);
        setexpirationDate(QDateTime::fromTime_t(static_cast<uint>(licenseObject->licenseExpirationDate)));
        setingescapeLicenseValidity(licenseObject->isLicenseValid);
        setmaxNumberOfAgents(licenseObject->platformNbAgents);
        setmaxNumberOfIOPs(licenseObject->platformNbIOPs);
        seteditorOwners(licenseObject->editorOwner);
        seteditorExpirationDate(QDateTime::fromTime_t(static_cast<uint>(licenseObject->editorExpirationDate)));
        seteditorLicenseValidity(licenseObject->isEditorLicenseValid);
        setfileName(QFileInfo(licenseObject->fileName).fileName());

        // Extract features allowed
        QStringList features;
        zlist_t *featureNames = zhash_keys(license->features);
        if (featureNames != nullptr)
        {
            char* key = static_cast<char*>(zlist_first(featureNames));
            while (key != nullptr)
            {
                features.append(QString(key));
                key = static_cast<char*>(zlist_next(featureNames));
            }
        }
        setfeatures(features);

        // Extract agents allowed
        QStringList agents;
        zlist_t *agentNames = zhash_keys(license->agents);
        if (agentNames != nullptr)
        {
            char* key = static_cast<char*>(zlist_first(agentNames));
            while (key != nullptr)
            {
                agents.append(QString(key));
                key = static_cast<char*>(zlist_next(agentNames));
            }
        }
        setagents(agents);
    }
}


/**
 * @brief Overload QDebug::operator<< to display license information directly in the logs.
 * @param debug
 * @param licenseInformation
 * @return
 */
QDebug operator<<(QDebug debug, const LicenseInformationM& licenseInformation)
{
    QDebugStateSaver saver(debug);

    debug << "License file name:" << licenseInformation.fileName() << "\n";
    debug << "Licence ID:" << licenseInformation.licenseId() << "\n";
    debug << "Customer:" << licenseInformation.customers() << "\n";
    debug << "Order n°:" << licenseInformation.orderNumbers() << "\n";
    debug << "IngeScape\n";
    debug << " - Is license valid?" << (licenseInformation.ingescapeLicenseValidity() ? "yes" : "no") << "\n";
    debug << " - Expiration date:" << licenseInformation.expirationDate().toString() << "\n";
    debug << " - Max number of agents:" << licenseInformation.maxNumberOfAgents() << "\n";
    debug << " - Max number of IOPs:" << licenseInformation.maxNumberOfIOPs() << "\n";
    debug << "Editor\n";
    debug << " - Is editor license valid?:" << (licenseInformation.editorLicenseValidity() ? "yes" : "no") << "\n";
    debug << " - Editor owner:" << licenseInformation.editorOwners() << "\n";
    debug << " - Editor expiration date:" << licenseInformation.editorExpirationDate().toString() << "\n";
    debug << " - Editor's features allowed [" << licenseInformation.features().size() << "]:\n";
    for (const QString& feature : licenseInformation.features())
    {
        debug << "    -" << feature << "\n";
    }
    debug << "Agents allowed [" << licenseInformation.agents().size() << "]:\n";
    for (const QString& agent : licenseInformation.agents())
    {
        debug << "    -" << agent << "\n";
    }

    return debug;
}
