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

#ifndef _MASTICQUICKPARAMETERSPROPERTYMAP_H_
#define _MASTICQUICKPARAMETERSPROPERTYMAP_H_

#include <QObject>
#include <QtQml>
#include <QQmlPropertyMap>


/**
 * @brief The MasticQuickParametersPropertyMap class defines a custom QQmlPropertyMap that is used to access Mastic parameters
 */
class MasticQuickParametersPropertyMap : public QQmlPropertyMap
{
    Q_OBJECT


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit MasticQuickParametersPropertyMap(QObject *parent = nullptr);


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

};

QML_DECLARE_TYPE(MasticQuickParametersPropertyMap)

#endif // _MASTICQUICKPARAMETERSPROPERTYMAP_H_
