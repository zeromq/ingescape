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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "ingescapecommon.h"

#include <misc/terminationsignalwatcher.h>
#include <settings/ingescapesettings.h>

#include "misc/debugquickinspector.h"


/**
 * @brief Constructor
 */
IngeScapeCommon::IngeScapeCommon()
{
}


/**
 * @brief Register our C++ types and extensions in the QML system
 */
void IngeScapeCommon::registerQmlTypes()
{
    //
    // Enum classes
    //
    qmlRegisterSingletonType<DebugWindowRenderingMode>("INGESCAPE", 1, 0,  "DebugWindowRenderingMode", &DebugWindowRenderingMode::qmlSingleton);



    //
    // Models
    //
    //qmlRegisterUncreatableType<AiguilleM>(uri, 1, 0, "AiguilleM", "Internal class");
    //qmlRegisterUncreatableType<AlarmInstanceM>(uri, 1, 0, "AlarmInstanceM", "Internal class");


    //
    // Controllers
    //
    //qmlRegisterUncreatableType<CircuitsElementsController>(uri, 1, 0, "CircuitsElementsController", "Internal class");
    //qmlRegisterUncreatableType<EquipmentEditorController>(uri, 1, 0, "EquipmentEditorController", "Internal class");
     qmlRegisterSingletonType<DebugQuickInspector>("INGESCAPE", 1, 0, "DebugQuickInspector", &DebugQuickInspector::qmlSingleton);



    //
    // QML components
    //
    //qmlRegisterType(QUrl("qrc:/qml/CircuitsElementsPanel.qml"), uri, 1, 0, "CircuitsElementsPanel");
    //qmlRegisterType(QUrl("qrc:/qml/Clock.qml"), uri, 1, 0, "Clock");

}

