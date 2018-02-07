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

#ifndef _MASTICQUICK_H_
#define _MASTICQUICK_H_

#include "masticquick_global.h"


// URI associated to our QML plugin (used in import)
#define DEFAULT_MASTICQUICK_URI "MasticQuick"


/**
 * @brief The MasticQuick class defines a set of utility functions to register QML extensions
 */
class MASTICQUICK_EXPORT MasticQuick
{
public:
    /**
     * @brief Register new QML types
     * @param uri
     */
    static void registerTypes(const char *uri = DEFAULT_MASTICQUICK_URI);
};


#endif // _MASTICQUICK_H_
