/*
 *	I2Quick
 *
 *  Copyright (c) 2016-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Alexandre Lemort <lemort@ingenuity.io>
 *
 */


#ifndef _I2QUICK_H_
#define _I2QUICK_H_

#include <QQmlEngine>


// To export symbols
#include "i2quick_global.h"



//----------
//
// Base
//
//----------


// To define properties with macros
#include "I2PropertyHelpers.h"

// Advanced types (list, hashtable, enums)
#include "base/I2CustomItemListModel.h"
#include "base/I2CustomItemListSortFilterProxyModel.h"
#include "base/I2EnumListModel.h"
#include "base/I2SortFilterProxyModel.h"


#include "base/I2SignalUtils.h"
#include "base/I2StringUtils.h"
#include "base/I2Utils.h"


//----------
//
// Graphics
//
//----------

// To create images with a specific SVG item
#include "graphics/I2SvgImageProvider.h"

// Set of helpers
#include "graphics/I2GraphicsHelpers.h"
#include "graphics/I2SvgUtils.h"
#include "graphics/I2TriangulationHelpers.h"

// File cache used by our SVG bitmap renderer
#include "graphics/I2SvgFileCache.h"

// Parser and file cache used by our SVG OpenGL renderer
#include "graphics/I2SimpleSvgParser.h"
#include "graphics/I2SvgVectorFileCache.h"


//----------
//
// Helpers
//
//----------



//----------
//
// Syntax highlighters
//
//----------



//----------
//
// System
//
//----------


#include "system/I2LogsManager.h"






// URI associated to our QML plugin (used in import)
#define DEFAULT_I2QUICK_URI "I2Quick"




/**
 * @brief The I2Quick class defines a set of utility funcitons to register QML extensions
 */
class I2QUICK_EXPORT I2Quick
{
public:
    /**
     * @brief Register new QML types
     * @param uri
     */
    static void registerTypes(const char *uri = DEFAULT_I2QUICK_URI);


    /**
     * @brief Initialize a given QML engine
     * @param engine
     * @param uri
     */
    static void initializeEngine(QQmlEngine *engine, const char *uri = DEFAULT_I2QUICK_URI);
};




#endif // _I2QUICK_H_
