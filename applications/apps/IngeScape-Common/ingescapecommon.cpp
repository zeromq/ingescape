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
#include <misc/urlutils.h>

#include <settings/ingescapesettings.h>

#include <controller/abstractscenariocontroller.h>
#include <controller/abstracttimeactionslinescenarioviewcontroller.h>
#include <controller/ingescapemodelmanager.h>
#include <controller/ingescapenetworkcontroller.h>
#include <controller/licensescontroller.h>
#include <controller/callhomecontroller.h>

#include <model/scenario/scenariom.h>
#include <model/scenario/timetickm.h>

#include <graphics/dashedbar.h>

#include <platformsupport/osutils.h>
#include <platformsupport/IngescapeApplicationQmlHelper.h>



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
    //---------------
    //
    // Enums
    //
    //---------------
    qmlRegisterSingletonType<ActionConditionTypes>("INGESCAPE", 1, 0, "ActionConditionTypes", &ActionConditionTypes::qmlSingleton);
    qmlRegisterSingletonType<ActionEffectTypes>("INGESCAPE", 1, 0, "ActionEffectTypes", &ActionEffectTypes::qmlSingleton);
    qmlRegisterSingletonType<AgentConditionValues>("INGESCAPE", 1, 0, "AgentConditionValues", &AgentConditionValues::qmlSingleton);
    qmlRegisterSingletonType<AgentEffectValues>("INGESCAPE", 1, 0, "AgentEffectValues", &AgentEffectValues::qmlSingleton);
    qmlRegisterSingletonType<AgentIOPTypes>("INGESCAPE", 1, 0, "AgentIOPTypes", &AgentIOPTypes::qmlSingleton);
    qmlRegisterSingletonType<AgentIOPValueTypes>("INGESCAPE", 1, 0, "AgentIOPValueTypes", &AgentIOPValueTypes::qmlSingleton);
    qmlRegisterSingletonType<AgentIOPValueTypeGroups>("INGESCAPE", 1, 0, "AgentIOPValueTypeGroups", &AgentIOPValueTypeGroups::qmlSingleton);
    qmlRegisterSingletonType<DebugWindowRenderingMode>("INGESCAPE", 1, 0,  "DebugWindowRenderingMode", &DebugWindowRenderingMode::qmlSingleton);
    qmlRegisterSingletonType<LogTypes>("INGESCAPE", 1, 0, "LogTypes", &LogTypes::qmlSingleton);
    qmlRegisterSingletonType<MappingEffectValues>("INGESCAPE", 1, 0, "MappingEffectValues", &MappingEffectValues::qmlSingleton);
    qmlRegisterSingletonType<ValidationDurationTypes>("INGESCAPE", 1, 0, "ValidationDurationTypes", &ValidationDurationTypes::qmlSingleton);
    qmlRegisterSingletonType<ValueComparisonTypes>("INGESCAPE", 1, 0, "ValueComparisonTypes", &ValueComparisonTypes::qmlSingleton);


    //----------------
    //
    // Controllers
    //
    //----------------
    qmlRegisterSingletonType<DebugQuickInspector>("INGESCAPE", 1, 0, "DebugQuickInspector", &DebugQuickInspector::qmlSingleton);

    qmlRegisterUncreatableType<AbstractScenarioController>("INGESCAPE", 1, 0, "AbstractScenarioController", "Internal Class");
    qmlRegisterUncreatableType<AbstractTimeActionslineScenarioViewController>("INGESCAPE", 1, 0, "AbstractTimeActionslineScenarioViewController", "Internal Class");
    qmlRegisterUncreatableType<IngeScapeModelManager>("INGESCAPE", 1, 0, "IngeScapeModelManager", "Internal class");
    qmlRegisterUncreatableType<IngeScapeNetworkController>("INGESCAPE", 1, 0, "IngeScapeNetworkController", "Internal class");
    qmlRegisterUncreatableType<LicensesController>("INGESCAPE", 1, 0, "LicensesController", "Internal class");
    qmlRegisterUncreatableType<CallHomeController>("INGESCAPE", 1, 0, "CallHomeController", "Internal class");


    //----------------
    //
    // Sort and Filter
    //
    //----------------
    qmlRegisterUncreatableType<AbstractTimeRangeFilter>("INGESCAPE", 1, 0, "AbstractTimeRangeFilter", "Internal class");


    //----------------
    //
    // Models
    //
    //----------------
    qmlRegisterUncreatableType<ActionConditionM>("INGESCAPE", 1, 0, "ActionConditionM", "Internal class");
    qmlRegisterUncreatableType<ActionEffectM>("INGESCAPE", 1, 0, "ActionEffectM", "Internal class");
    qmlRegisterUncreatableType<ActionM>("INGESCAPE", 1, 0, "ActionM", "Internal class");
    qmlRegisterUncreatableType<AgentIOPM>("INGESCAPE", 1, 0, "AgentIOPM", "Internal class");
    qmlRegisterUncreatableType<AgentM>("INGESCAPE", 1, 0, "AgentM", "Internal class");
    qmlRegisterUncreatableType<AgentMappingM>("INGESCAPE", 1, 0, "AgentMappingM", "Internal class");
    qmlRegisterUncreatableType<ConditionOnAgentM>("INGESCAPE", 1, 0, "ConditionOnAgentM", "Internal class");
    qmlRegisterUncreatableType<DefinitionM>("INGESCAPE", 1, 0, "DefinitionM", "Internal class");
    qmlRegisterUncreatableType<EffectOnAgentM>("INGESCAPE", 1, 0, "EffectOnAgentM", "Internal class");
    qmlRegisterUncreatableType<Enums>("INGESCAPE", 1, 0, "Enums", "Internal class");
    qmlRegisterUncreatableType<HostM>("INGESCAPE", 1, 0, "HostM", "Internal class");
    qmlRegisterUncreatableType<IOPValueConditionM>("INGESCAPE", 1, 0, "IOPValueConditionM", "Internal class");
    qmlRegisterUncreatableType<IOPValueEffectM>("INGESCAPE", 1, 0, "IOPValueEffectM", "Internal class");
    qmlRegisterUncreatableType<MappingEffectM>("INGESCAPE", 1, 0, "MappingEffectM", "Internal class");
    qmlRegisterUncreatableType<MappingElementM>("INGESCAPE", 1, 0, "MappingElementM", "Internal class");
    qmlRegisterUncreatableType<OutputM>("INGESCAPE", 1, 0, "OutputM", "Internal class");
    qmlRegisterUncreatableType<PublishedValueM>("INGESCAPE", 1, 0, "PublishedValueM", "Internal class");
    qmlRegisterUncreatableType<ScenarioM>("INGESCAPE", 1, 0, "ScenarioM", "Internal class");
    qmlRegisterUncreatableType<TimeTickM>("INGESCAPE", 1, 0, "TimeTickM", "Internal class");
    qmlRegisterUncreatableType<LicenseInformationM>("INGESCAPE", 1, 0, "LicenseInformationM", "Internal class");


    //---------------
    //
    // View Models
    //
    //---------------
    qmlRegisterUncreatableType<ActionConditionVM>("INGESCAPE", 1, 0, "ActionConditionVM", "Internal class");
    qmlRegisterUncreatableType<ActionEffectVM>("INGESCAPE", 1, 0, "ActionEffectVM", "Internal class");
    qmlRegisterUncreatableType<ActionExecutionVM>("INGESCAPE", 1, 0, "ActionExecutionVM", "Internal class");
    qmlRegisterUncreatableType<ActionInPaletteVM>("INGESCAPE", 1, 0, "ActionInPaletteVM", "Internal class");
    qmlRegisterUncreatableType<ActionVM>("INGESCAPE", 1, 0, "ActionVM", "Internal class");
    qmlRegisterUncreatableType<AgentsGroupedByDefinitionVM>("INGESCAPE", 1, 0, "AgentsGroupedByDefinitionVM", "Internal class");
    qmlRegisterUncreatableType<AgentsGroupedByNameVM>("INGESCAPE", 1, 0, "AgentsGroupedByNameVM", "Internal class");
    qmlRegisterUncreatableType<AgentIOPVM>("INGESCAPE", 1, 0, "AgentIOPVM", "Internal class");
    qmlRegisterUncreatableType<InputVM>("INGESCAPE", 1, 0, "InputVM", "Internal class");
    qmlRegisterUncreatableType<OutputVM>("INGESCAPE", 1, 0, "OutputVM", "Internal class");
    qmlRegisterUncreatableType<MappingElementVM>("INGESCAPE", 1, 0, "MappingElementVM", "Internal class");
    qmlRegisterUncreatableType<ParameterVM>("INGESCAPE", 1, 0, "ParameterVM", "Internal class");




    //---------------
    //
    // Views
    //
    //---------------

    qmlRegisterType<DashedBar>("INGESCAPE", 1, 0, "DashedBar");


    //---------------
    //
    // Misc
    //
    //---------------
    qmlRegisterSingletonType<URLUtils>("INGESCAPE", 1, 0, "URLUtils", &URLUtils::qmlSingleton);



    //---------------
    //
    // Platform support
    //
    //---------------
    qmlRegisterSingletonType<OSUtils>("INGESCAPE", 1, 0, "OSUtils", &OSUtils::qmlSingleton);
    qmlRegisterSingletonType<IngescapeApplicationQmlHelper>("INGESCAPE", 1, 0, "IngescapeApplicationQmlHelper", &IngescapeApplicationQmlHelper::qmlSingleton);




    //------------------
    //
    // QML components
    //
    //------------------
    // - Singleton used to enable consistent app styling through predefined colors, fonts, etc.
    qmlRegisterSingletonType(QUrl("qrc:/qml/theme/IngeScapeTheme.qml"), "INGESCAPE", 1, 0, "IngeScapeTheme");

    // - Label-less SVG Button style
    qmlRegisterType(QUrl("qrc:/qml/theme/LabellessSvgButtonStyle.qml"), "INGESCAPE", 1, 0, "LabellessSvgButtonStyle");

    // - Combobox style
    qmlRegisterType(QUrl("qrc:/qml/theme/IngeScapeComboboxStyle.qml"), "INGESCAPE", 1, 0, "IngeScapeComboboxStyle");

    // - Scrollview style
    qmlRegisterType(QUrl("qrc:/qml/theme/IngeScapeScrollViewStyle.qml"), "INGESCAPE", 1, 0, "IngeScapeScrollViewStyle");

    // - Style of button for options in (popup) menu
    qmlRegisterType(QUrl("qrc:/qml/theme/ButtonStyleOfOption.qml"), "INGESCAPE", 1, 0, "ButtonStyleOfOption");

    // - Combobox delegate with tooltip
    qmlRegisterType(QUrl("qrc:/qml/IngeScapeToolTipComboboxDelegate.qml"), "INGESCAPE", 1, 0, "IngeScapeToolTipComboboxDelegate");

    // - Actions List Item
    qmlRegisterType(QUrl("qrc:/qml/ActionsListItem.qml"), "INGESCAPE", 1, 0, "ActionsListItem");

    // - Popups
    qmlRegisterType(QUrl("qrc:/qml/ConfirmationPopup.qml"), "INGESCAPE", 1, 0, "ConfirmationPopup");
    qmlRegisterType(QUrl("qrc:/qml/LicensePopup.qml"), "INGESCAPE", 1, 0, "LicensePopup");
    qmlRegisterType(QUrl("qrc:/qml/SystemInformationPopup.qml"), "INGESCAPE", 1, 0, "SystemInformationPopup");
    qmlRegisterType(QUrl("qrc:/qml/popup/MenuPopup.qml"), "INGESCAPE", 1, 0, "MenuPopup");

    // - Text area with timer to delay processing of edited text while the user is editing it
    qmlRegisterType(QUrl("qrc:/qml/TextAreaWithTimer.qml"), "INGESCAPE", 1, 0, "TextAreaWithTimer");

    // - Network configuration information
    qmlRegisterType(QUrl("qrc:/qml/NetworkConnectionInformationItem.qml"), "INGESCAPE", 1, 0, "NetworkConnectionInformationItem");

    // - Timeline
    qmlRegisterType(QUrl("qrc:/qml/timeline/ActionInTimeLine.qml"), "INGESCAPE", 1, 0, "ActionInTimeLine");
    qmlRegisterType(QUrl("qrc:/qml/timeline/ScenarioTimeLine.qml"), "INGESCAPE", 1, 0, "ScenarioTimeLine");

    // - Widgets
    qmlRegisterType(QUrl("qrc:/qml/widgets/LabellessSvgButton.qml"), "INGESCAPE", 1, 0, "LabellessSvgButton");
    qmlRegisterType(QUrl("qrc:/qml/widgets/LabellessSvgButtonCachedImage.qml"), "INGESCAPE", 1, 0, "LabellessSvgButtonCachedImage");
    qmlRegisterType(QUrl("qrc:/qml/widgets/SvgImage.qml"), "INGESCAPE", 1, 0, "SvgImage");

}
