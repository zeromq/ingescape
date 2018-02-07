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


/**
 * @brief Default constructor
 *
 * @param readOnly
 * @param parent
 */
MasticQuickQmlPropertyMap::MasticQuickQmlPropertyMap(bool readOnly, QObject *parent) : QQmlPropertyMap(parent),
    _readOnly(readOnly)
{
}
