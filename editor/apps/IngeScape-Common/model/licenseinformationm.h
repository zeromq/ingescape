/*
 *	ENEDIS - Inget-PS
 *
 *  Copyright (c) 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Mathieu Soum <soum@ingenuity.io>
 *
 */

#ifndef LICENSEINFORMATIONM_H
#define LICENSEINFORMATIONM_H

#include <QObject>
#include <QtQml>
#include <I2PropertyHelpers.h>


extern "C" {
#include <ingescape_private.h>
}


/**
 * @brief Holds the various information form a given license_t instance
 * sorted into QML properties so they can be displayed in the editor
 */
class LicenseInformationM : public QObject
{
    Q_OBJECT

    // The licence ID
    I2_QML_PROPERTY(QString, licenseId)

    // List of registered customer
    I2_QML_PROPERTY(QString, customers)

    // List of licences order numbers
    I2_QML_PROPERTY(QString, orderNumbers)

    // Ingescape expiration date
    I2_QML_PROPERTY(QDateTime, expirationDate)

    // Is ingescape license valid ?
    I2_QML_PROPERTY(bool, ingescapeLicenseValidity)

    // Maximum number of agents allowed
    I2_QML_PROPERTY(long, maxNumberOfAgents)

    // Maximum number of IOPs allowed
    I2_QML_PROPERTY(long, maxNumberOfIOPs)

    // List of registered editor owners
    I2_QML_PROPERTY(QString, editorOwners)

    // Editor expiration date
    I2_QML_PROPERTY(QDateTime, editorExpirationDate)

    // Is editor license valid ?
    I2_QML_PROPERTY(bool, editorLicenseValidity)

    // List of features allowed
    I2_QML_PROPERTY(QStringList, features)

    // List of agents allowed
    I2_QML_PROPERTY(QStringList, agents)

    // License file (or directory in case of merged license)
    I2_QML_PROPERTY(QFileInfo, licenseFileInfo)

public:
    LicenseInformationM(const license_t* licenseObject, QObject* parent = nullptr);
};

QDebug operator<<(QDebug debug, const LicenseInformationM& licenseInformation);

QML_DECLARE_TYPE(LicenseInformationM)

#endif // LICENSEINFORMATIONM_H
