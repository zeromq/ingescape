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
     * @param readOnly
     * @param parent
     */
    explicit MasticQuickQmlPropertyMap(bool readOnly, QObject *parent = nullptr);


protected:
    // Flag indicating if our properties are read-only
    bool _readOnly;
};

#endif // _MASTICQUICKQMLPROPERTYMAP_H_
