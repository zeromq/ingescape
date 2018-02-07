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

#ifndef _MASTICQUICKQMLPROPERTYMAP_H_
#define _MASTICQUICKQMLPROPERTYMAP_H_

#include <QObject>
#include <QQmlPropertyMap>


/**
 * @brief The MasticQuickQmlPropertyMap class defines a custom QQmlPropertyMap that can be read-only if needed
 */
class MasticQuickQmlPropertyMap : public QQmlPropertyMap
{
    Q_OBJECT

public:
    /**
     * @brief Default constructor
     *
     * @param propertiesAreReadOnlyForQml
     * @param parent
     */
    explicit MasticQuickQmlPropertyMap(bool propertiesAreReadOnlyForQml, QObject *parent = nullptr);


protected:
    /**
     * @brief Method provided to intercept updates to a property from QML
     *
     * @param key          Name of the property
     * @param newQmlValue  The new value provided by QML
     *
     * @return the value that will really be stored in our property
     */
    QVariant updateValue(const QString &key, const QVariant &newQmlValue) Q_DECL_OVERRIDE;


protected:
    // Flag indicating if our properties are read-only for QML
    // i.e. if we prevent QML from updating them or not
    bool _propertiesAreReadOnlyForQml;
};

#endif // _MASTICQUICKQMLPROPERTYMAP_H_
