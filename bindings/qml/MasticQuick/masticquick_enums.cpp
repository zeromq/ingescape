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



/**
 * @brief Check if Iop type "fromType" can be written to Iop type "toType"
 * @param from
 * @param to
 * @return
 */
bool MasticQuickUtils::checkIfIopTypesAreCompatible(MasticIopType::Value fromType, MasticIopType::Value toType)
{
    bool result = false;

    // Check if we have the same type
    if (fromType == toType)
    {
        result = true;
    }
    else if ((fromType != MasticIopType::INVALID) && (toType != MasticIopType::INVALID))
    {
        switch (fromType)
        {
            case MasticIopType::INTEGER:
                {
                    // All except DATA
                    result = (toType != MasticIopType::DATA);
                }
                break;

            case MasticIopType::DOUBLE:
                {
                    // All except DATA
                    result = (toType != MasticIopType::DATA);
                }
                break;

            case MasticIopType::STRING:
                {
                    // STRING and IMPULSION only
                    result = ((toType == MasticIopType::STRING) || (toType == MasticIopType::IMPULSION));
                }
                break;

            case MasticIopType::BOOLEAN:
                {
                    // All except DATA
                    result = (toType != MasticIopType::DATA);
                }
                break;

            case MasticIopType::IMPULSION:
                {
                    // All except DATA
                    result = (toType != MasticIopType::DATA);
                }
                break;

            case MasticIopType::DATA:
                {
                    // All
                    result = true;
                }
                break;

            default:
                break;
        }
    }

    return result;
}
