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

#ifndef _INGESCAPEQUICK_ENUMS_H_
#define _INGESCAPEQUICK_ENUMS_H_

#include <QObject>


#include "ingescapequick_helpers.h"


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
INGESCAPE_QML_ENUM(IngeScapeIopType, INVALID = 0, INTEGER, DOUBLE, STRING, BOOLEAN, IMPULSION, DATA)



/**
 * @brief Enum for log levels
 */
INGESCAPE_QML_ENUM(IngeScapeLogLevel, LOG_TRACE = 0, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL)






/**
 * @brief IngeScapeQuickUtils defines a set of utility functions
 */
class IngeScapeQuickUtils : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Check if Iop type "fromType" can be written to Iop type "toType"
     * @param from
     * @param to
     * @return
     */
    static bool checkIfIopTypesAreCompatible(IngeScapeIopType::Value fromType, IngeScapeIopType::Value toType);

};


#endif // _INGESCAPEQUICK_ENUMS_H_
