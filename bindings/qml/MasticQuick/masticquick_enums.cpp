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

#include "masticquick_enums.h"



/**
 * @brief MasticIopType pretty print
 * @param value
 * @return
 */
QString MasticIopType::enumToString(int value)
{
    return enumToKey(value);
}



/**
 * @brief MasticLogLevel pretty print
 * @param value
 * @return
 */
QString MasticLogLevel::enumToString(int value)
{
    return enumToKey(value);
}
