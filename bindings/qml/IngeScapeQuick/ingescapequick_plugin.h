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


#ifndef _MASTIQUICK_LPLUGIN_H_
#define _MASTIQUICK_LPLUGIN_H_

#include <QQmlEngine>
#include <QQmlExtensionPlugin>

#include "IngeScapeQuick.h"


/**
 * @brief The IngeScapeQuickPlugin class defines a QML extension plugin
 */
class IngeScapeQuickPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
   /**
    * @brief Called to register the new QML types
    * @param uri
    */
    void registerTypes(const char *uri = "IngeScapeQuick");
};


#endif // _MASTIQUICK_LPLUGIN_H_
