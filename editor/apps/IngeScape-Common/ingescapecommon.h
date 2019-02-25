/*
 *	IngeScape Common
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef INGESCAPECOMMON_H
#define INGESCAPECOMMON_H

#include "ingescape-common_global.h"

class INGESCAPECOMMONSHARED_EXPORT IngeScapeCommon
{

public:
    IngeScapeCommon();


    /**
     * @brief Register our C++ types and extensions in the QML system
     */
    static void registerPGIMQmlTypes();

};

#endif // INGESCAPECOMMON_H
