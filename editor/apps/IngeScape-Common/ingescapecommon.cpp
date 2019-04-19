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

#include <misc/debugquickinspector.h>
#include <misc/terminationsignalwatcher.h>
#include <settings/ingescapesettings.h>

#include <model/enums.h>
#include <model/agent/agentm.h>


/**
 * @brief Constructor
 */
IngeScapeCommon::IngeScapeCommon()
{
}


/**
 * @brief Register our C++ types and extensions in the QML system
 */
void IngeScapeCommon::registerIngeScapeQmlTypes()
{
    //
    // Enum classes
    //
    qmlRegisterSingletonType<AgentIOPTypes>("INGESCAPE", 1, 0, "AgentIOPTypes", &AgentIOPTypes::qmlSingleton);
    qmlRegisterSingletonType<AgentIOPValueTypes>("INGESCAPE", 1, 0, "AgentIOPValueTypes", &AgentIOPValueTypes::qmlSingleton);
    qmlRegisterSingletonType<AgentIOPValueTypeGroups>("INGESCAPE", 1, 0, "AgentIOPValueTypeGroups", &AgentIOPValueTypeGroups::qmlSingleton);
    qmlRegisterSingletonType<LogTypes>("INGESCAPE", 1, 0, "LogTypes", &LogTypes::qmlSingleton);
    qmlRegisterSingletonType<DebugWindowRenderingMode>("INGESCAPE", 1, 0,  "DebugWindowRenderingMode", &DebugWindowRenderingMode::qmlSingleton);



    //
    // Models
    //
    qmlRegisterUncreatableType<AgentIOPM>("INGESCAPE", 1, 0, "AgentIOPM", "Internal class");
    qmlRegisterUncreatableType<AgentM>("INGESCAPE", 1, 0, "AgentM", "Internal class");
    qmlRegisterUncreatableType<AgentMappingM>("INGESCAPE", 1, 0, "AgentMappingM", "Internal class");
    qmlRegisterUncreatableType<DefinitionM>("INGESCAPE", 1, 0, "DefinitionM", "Internal class");
    qmlRegisterUncreatableType<Enums>("INGESCAPE", 1, 0, "Enums", "Internal class");
    qmlRegisterUncreatableType<MappingElementM>("INGESCAPE", 1, 0, "MappingElementM", "Internal class");
    qmlRegisterUncreatableType<OutputM>("INGESCAPE", 1, 0, "OutputM", "Internal class");


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

