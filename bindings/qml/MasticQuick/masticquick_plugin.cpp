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


#include "masticquick_plugin.h"

#include <qqml.h>



/**
 * @brief Fonction called to register all new types which are provided by this extension plugin
 * @param uri The URI of our extension plugin
 */
void MasticQuickPlugin::registerTypes(const char *uri)
{
    MasticQuick::registerTypes(uri);
}

