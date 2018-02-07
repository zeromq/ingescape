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

#include "masticquickqmlpropertymap.h"

#include <QDebug>


/**
 * @brief Default constructor
 *
 * @param readOnly
 * @param propertiesAreReadOnlyForQml
 */
MasticQuickQmlPropertyMap::MasticQuickQmlPropertyMap(bool propertiesAreReadOnlyForQml, QObject *parent) : QQmlPropertyMap(parent),
    _propertiesAreReadOnlyForQml(propertiesAreReadOnlyForQml)
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
QVariant MasticQuickQmlPropertyMap::updateValue(const QString &key, const QVariant &newQmlValue)
{
    qDebug() << "UPDATE FROM QML" << key << "=>" << newQmlValue;

    return newQmlValue;
}
