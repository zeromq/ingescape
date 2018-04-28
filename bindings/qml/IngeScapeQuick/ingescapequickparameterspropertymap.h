/*
 *  IngeScape - QML binding
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

#ifndef _INGESCAPEQUICKPARAMETERSPROPERTYMAP_H_
#define _INGESCAPEQUICKPARAMETERSPROPERTYMAP_H_

#include <QObject>
#include <QtQml>
#include <QQmlPropertyMap>


/**
 * @brief The IngeScapeQuickParametersPropertyMap class defines a custom QQmlPropertyMap that is used to access IngeScape parameters
 */
class IngeScapeQuickParametersPropertyMap : public QQmlPropertyMap
{
    Q_OBJECT


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit IngeScapeQuickParametersPropertyMap(QObject *parent = nullptr);


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

QML_DECLARE_TYPE(IngeScapeQuickParametersPropertyMap)

#endif // _INGESCAPEQUICKPARAMETERSPROPERTYMAP_H_
