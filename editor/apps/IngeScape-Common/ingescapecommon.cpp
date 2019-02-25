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

#include "ingescapecommon.h"

#include <misc/terminationsignalwatcher.h>
#include <settings/ingescapesettings.h>


/**
 * @brief Constructor
 */
IngeScapeCommon::IngeScapeCommon()
{
}


/**
 * @brief Register our C++ types and extensions in the QML system
 */
void IngeScapeCommon::registerPGIMQmlTypes()
{
    // Our custom QML namespace
    //const char *uri = "IngeScapeCommon";


    //
    // Enum classes
    //
    //qmlRegisterSingletonType<AiguilleStates>(uri, 1, 0, "AiguilleStates", &AiguilleStates::qmlSingleton);
    //qmlRegisterSingletonType<AlarmTypes>(uri, 1, 0, "AlarmTypes", &AlarmTypes::qmlSingleton);


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


    //
    // QML components
    //
    //qmlRegisterType(QUrl("qrc:/qml/CircuitsElementsPanel.qml"), uri, 1, 0, "CircuitsElementsPanel");
    //qmlRegisterType(QUrl("qrc:/qml/Clock.qml"), uri, 1, 0, "Clock");


}

