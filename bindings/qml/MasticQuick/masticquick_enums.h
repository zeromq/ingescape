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

#ifndef _MASTICQUICK_ENUMS_H_
#define _MASTICQUICK_ENUMS_H_

#include <QObject>


#include "masticquick_helpers.h"


//-------------------------------------------------------------------
//
//
//  Enums
//
//
//-------------------------------------------------------------------


/**
  * @brief Enum for IOP types
  */
MASTIC_QML_ENUM(MasticIopType, INVALID = 0, INTEGER, DOUBLE, STRING, BOOLEAN, IMPULSION, DATA)



/**
 * @brief Enum for log levels
 */
MASTIC_QML_ENUM(MasticLogLevel, LOG_TRACE = 0, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL)






/**
 * @brief MasticQuickUtils defines a set of utility functions
 */
class MasticQuickUtils : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Check if Iop type "fromType" can be written to Iop type "toType"
     * @param from
     * @param to
     * @return
     */
    static bool checkIfIopTypesAreCompatible(MasticIopType::Value fromType, MasticIopType::Value toType);

};


#endif // _MASTICQUICK_ENUMS_H_
