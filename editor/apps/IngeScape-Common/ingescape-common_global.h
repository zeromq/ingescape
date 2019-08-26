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

#ifndef INGESCAPECOMMON_GLOBAL_H
#define INGESCAPECOMMON_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(INGESCAPECOMMON_LIBRARY)
#  define INGESCAPECOMMONSHARED_EXPORT Q_DECL_EXPORT
#elseif defined(INGESCAPECOMMON_FROM_PRI)
#  define INGESCAPECOMMONSHARED_EXPORT
#else
#  define INGESCAPECOMMONSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // INGESCAPECOMMON_GLOBAL_H
