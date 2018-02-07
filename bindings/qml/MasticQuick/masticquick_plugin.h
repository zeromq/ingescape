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


#ifndef _MASTIQUICK_LPLUGIN_H_
#define _MASTIQUICK_LPLUGIN_H_

#include <QQmlEngine>
#include <QQmlExtensionPlugin>

#include "MasticQuick.h"


/**
 * @brief The MasticQuickPlugin class defines a QML extension plugin
 */
class MasticQuickPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
   /**
    * @brief Called to register the new QML types
    * @param uri
    */
    void registerTypes(const char *uri = DEFAULT_MASTICQUICK_URI);
};


#endif // _MASTIQUICK_LPLUGIN_H_
