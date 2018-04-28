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

#include "ingescapequick_enums.h"



/**
 * @brief IngeScapeIopType pretty print
 * @param value
 * @return
 */
QString IngeScapeIopType::enumToString(int value)
{
    return enumToKey(value);
}



/**
 * @brief IngeScapeLogLevel pretty print
 * @param value
 * @return
 */
QString IngeScapeLogLevel::enumToString(int value)
{
    return enumToKey(value);
}



/**
 * @brief Check if Iop type "fromType" can be written to Iop type "toType"
 * @param from
 * @param to
 * @return
 */
bool IngeScapeQuickUtils::checkIfIopTypesAreCompatible(IngeScapeIopType::Value fromType, IngeScapeIopType::Value toType)
{
    bool result = false;

    // Check if we have the same type
    if (fromType == toType)
    {
        result = true;
    }
    else if ((fromType != IngeScapeIopType::INVALID) && (toType != IngeScapeIopType::INVALID))
    {
        switch (fromType)
        {
            case IngeScapeIopType::INTEGER:
                {
                    // All except DATA
                    result = (toType != IngeScapeIopType::DATA);
                }
                break;

            case IngeScapeIopType::DOUBLE:
                {
                    // All except DATA
                    result = (toType != IngeScapeIopType::DATA);
                }
                break;

            case IngeScapeIopType::STRING:
                {
                    // STRING and IMPULSION only
                    result = ((toType == IngeScapeIopType::STRING) || (toType == IngeScapeIopType::IMPULSION));
                }
                break;

            case IngeScapeIopType::BOOLEAN:
                {
                    // All except DATA
                    result = (toType != IngeScapeIopType::DATA);
                }
                break;

            case IngeScapeIopType::IMPULSION:
                {
                    // All except DATA
                    result = (toType != IngeScapeIopType::DATA);
                }
                break;

            case IngeScapeIopType::DATA:
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
