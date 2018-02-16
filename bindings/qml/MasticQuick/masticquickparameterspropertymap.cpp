/*
 *  Mastic - QML binding
 *
 *  Copyright (c) 2018 Ingenuity i/o. All rights reserved.
 *
 *  See license terms for the rights and conditions
 *  defined by copyright holders.
 *
 *
 *  Contributors:
 *      Alexandre Lemort <lemort@ingenuity.io>
 *
 */

#include "masticquickparameterspropertymap.h"

#include <QDebug>

extern "C" {
#include <mastic.h>
}


/**
 * @brief Default constructor
 * @param parent
 */
MasticQuickParametersPropertyMap::MasticQuickParametersPropertyMap(QObject *parent) : QQmlPropertyMap(this, parent)
{
}


/**
 * @brief Method provided to intercept updates to a property from QML
 *
 * @param key          Name of the property
 * @param newQmlValue  The new value provided by QML
 *
 * @return the value that will really be stored in our property
 */
QVariant MasticQuickParametersPropertyMap::updateValue(const QString &key, const QVariant &newQmlValue)
{
    // Check if this parameyter exists
    if (mtic_checkParameterExistence(key.toStdString().c_str()))
    {
        // We can update our property
        return newQmlValue;
    }
    else
    {
        // This output does not exist. It may be a typo
        qWarning() << "MasticQuick warning: can not update valye from QML - " << QString("Mastic.parameters.%1").arg(key) << "is invalid because parameter"
                   << key << "does not exist. It may be a typo or this parameter is not yet created or has been removed";

        // Return an empty QVariant
        return QVariant();
    }
}
