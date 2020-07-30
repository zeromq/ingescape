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

#include <model/peerm.h>
#include <model/recordm.h>
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
    const char* uri = "INGESCAPE";

    //---------------
    //
    // Enums
    //
    //---------------
    qmlRegisterSingletonType<ActionConditionTypes>(uri, 1, 0, "ActionConditionTypes", &ActionConditionTypes::qmlSingleton);
    qmlRegisterSingletonType<ActionEffectTypes>(uri, 1, 0, "ActionEffectTypes", &ActionEffectTypes::qmlSingleton);
    qmlRegisterSingletonType<AgentConditionValues>(uri, 1, 0, "AgentConditionValues", &AgentConditionValues::qmlSingleton);
    qmlRegisterSingletonType<AgentEffectValues>(uri, 1, 0, "AgentEffectValues", &AgentEffectValues::qmlSingleton);
    qmlRegisterSingletonType<AgentIOPTypes>(uri, 1, 0, "AgentIOPTypes", &AgentIOPTypes::qmlSingleton);
    qmlRegisterSingletonType<AgentIOPValueTypes>(uri, 1, 0, "AgentIOPValueTypes", &AgentIOPValueTypes::qmlSingleton);
    qmlRegisterSingletonType<AgentIOPValueTypeGroups>(uri, 1, 0, "AgentIOPValueTypeGroups", &AgentIOPValueTypeGroups::qmlSingleton);
    qmlRegisterSingletonType<DebugWindowRenderingMode>(uri, 1, 0,  "DebugWindowRenderingMode", &DebugWindowRenderingMode::qmlSingleton);
    qmlRegisterSingletonType<LogTypes>(uri, 1, 0, "LogTypes", &LogTypes::qmlSingleton);
    qmlRegisterSingletonType<MappingEffectValues>(uri, 1, 0, "MappingEffectValues", &MappingEffectValues::qmlSingleton);
    qmlRegisterSingletonType<ValidationDurationTypes>(uri, 1, 0, "ValidationDurationTypes", &ValidationDurationTypes::qmlSingleton);
    qmlRegisterSingletonType<ValueComparisonTypes>(uri, 1, 0, "ValueComparisonTypes", &ValueComparisonTypes::qmlSingleton);


    //----------------
    //
    // Controllers
    //
    //----------------
    qmlRegisterSingletonType<DebugQuickInspector>(uri, 1, 0, "DebugQuickInspector", &DebugQuickInspector::qmlSingleton);
    qmlRegisterSingletonType<IngeScapeModelManager>(uri, 1, 0, "IgsModelManager", &IngeScapeModelManager::qmlSingleton);
    qmlRegisterSingletonType<IngeScapeNetworkController>(uri, 1, 0, "IgsNetworkController", &IngeScapeNetworkController::qmlSingleton);

    qmlRegisterUncreatableType<AbstractScenarioController>(uri, 1, 0, "AbstractScenarioController", "Internal Class");
    qmlRegisterUncreatableType<AbstractTimeActionslineScenarioViewController>(uri, 1, 0, "AbstractTimeActionslineScenarioViewController", "Internal Class");
    qmlRegisterUncreatableType<LicensesController>(uri, 1, 0, "LicensesController", "Internal class");
    qmlRegisterUncreatableType<CallHomeController>(uri, 1, 0, "CallHomeController", "Internal class");


    //----------------
    //
    // Sort and Filter
    //
    //----------------
    qmlRegisterUncreatableType<AbstractTimeRangeFilter>(uri, 1, 0, "AbstractTimeRangeFilter", "Internal class");


    //----------------
    //
    // Models
    //
    //----------------
    qmlRegisterUncreatableType<ActionConditionM>(uri, 1, 0, "ActionConditionM", "Internal class");
    qmlRegisterUncreatableType<ActionEffectM>(uri, 1, 0, "ActionEffectM", "Internal class");
    qmlRegisterUncreatableType<ActionM>(uri, 1, 0, "ActionM", "Internal class");
    qmlRegisterUncreatableType<AgentIOPM>(uri, 1, 0, "AgentIOPM", "Internal class");
    qmlRegisterUncreatableType<AgentM>(uri, 1, 0, "AgentM", "Internal class");
    qmlRegisterUncreatableType<AgentMappingM>(uri, 1, 0, "AgentMappingM", "Internal class");
    qmlRegisterUncreatableType<ConditionOnAgentM>(uri, 1, 0, "ConditionOnAgentM", "Internal class");
    qmlRegisterUncreatableType<DefinitionM>(uri, 1, 0, "DefinitionM", "Internal class");
    qmlRegisterUncreatableType<EffectOnAgentM>(uri, 1, 0, "EffectOnAgentM", "Internal class");
    qmlRegisterUncreatableType<Enums>(uri, 1, 0, "Enums", "Internal class");
    qmlRegisterUncreatableType<HostM>(uri, 1, 0, "HostM", "Internal class");
    qmlRegisterUncreatableType<IOPValueConditionM>(uri, 1, 0, "IOPValueConditionM", "Internal class");
    qmlRegisterUncreatableType<IOPValueEffectM>(uri, 1, 0, "IOPValueEffectM", "Internal class");
    qmlRegisterUncreatableType<MappingEffectM>(uri, 1, 0, "MappingEffectM", "Internal class");
    qmlRegisterUncreatableType<MappingElementM>(uri, 1, 0, "MappingElementM", "Internal class");
    qmlRegisterUncreatableType<OutputM>(uri, 1, 0, "OutputM", "Internal class");
    qmlRegisterUncreatableType<PeerM>(uri, 1, 0, "PeerM", "Internal class");
    qmlRegisterUncreatableType<PublishedValueM>(uri, 1, 0, "PublishedValueM", "Internal class");
    qmlRegisterUncreatableType<RecordM>(uri, 1, 0, "RecordM", "Internal class");
    qmlRegisterUncreatableType<ScenarioM>(uri, 1, 0, "ScenarioM", "Internal class");
    qmlRegisterUncreatableType<TimeTickM>(uri, 1, 0, "TimeTickM", "Internal class");
    qmlRegisterUncreatableType<LicenseInformationM>(uri, 1, 0, "LicenseInformationM", "Internal class");


    //---------------
    //
    // View Models
    //
    //---------------
    qmlRegisterUncreatableType<ActionConditionVM>(uri, 1, 0, "ActionConditionVM", "Internal class");
    qmlRegisterUncreatableType<ActionEffectVM>(uri, 1, 0, "ActionEffectVM", "Internal class");
    qmlRegisterUncreatableType<ActionExecutionVM>(uri, 1, 0, "ActionExecutionVM", "Internal class");
    qmlRegisterUncreatableType<ActionInPaletteVM>(uri, 1, 0, "ActionInPaletteVM", "Internal class");
    qmlRegisterUncreatableType<ActionVM>(uri, 1, 0, "ActionVM", "Internal class");
    qmlRegisterUncreatableType<AgentsGroupedByDefinitionVM>(uri, 1, 0, "AgentsGroupedByDefinitionVM", "Internal class");
    qmlRegisterUncreatableType<AgentsGroupedByNameVM>(uri, 1, 0, "AgentsGroupedByNameVM", "Internal class");
    qmlRegisterUncreatableType<AgentIOPVM>(uri, 1, 0, "AgentIOPVM", "Internal class");
    qmlRegisterUncreatableType<InputVM>(uri, 1, 0, "InputVM", "Internal class");
    qmlRegisterUncreatableType<OutputVM>(uri, 1, 0, "OutputVM", "Internal class");
    qmlRegisterUncreatableType<MappingElementVM>(uri, 1, 0, "MappingElementVM", "Internal class");
    qmlRegisterUncreatableType<ParameterVM>(uri, 1, 0, "ParameterVM", "Internal class");




    //---------------
    //
    // Views
    //
    //---------------

    qmlRegisterType<DashedBar>(uri, 1, 0, "DashedBar");


    //---------------
    //
    // Misc
    //
    //---------------
    qmlRegisterSingletonType<URLUtils>(uri, 1, 0, "URLUtils", &URLUtils::qmlSingleton);



    //---------------
    //
    // Platform support
    //
    //---------------
    qmlRegisterSingletonType<OSUtils>(uri, 1, 0, "OSUtils", &OSUtils::qmlSingleton);
    qmlRegisterSingletonType<IngescapeApplicationQmlHelper>(uri, 1, 0, "IngescapeApplicationQmlHelper", &IngescapeApplicationQmlHelper::qmlSingleton);




    //------------------
    //
    // QML components
    //
    //------------------
    // - Singleton used to enable consistent app styling through predefined colors, fonts, etc.
    qmlRegisterSingletonType(QUrl("qrc:/qml/theme/IngeScapeTheme.qml"), uri, 1, 0, "IngeScapeTheme");

    // - Label-less SVG Button style
    qmlRegisterType(QUrl("qrc:/qml/theme/LabellessSvgButtonStyle.qml"), uri, 1, 0, "LabellessSvgButtonStyle");

    // - Combobox style
    qmlRegisterType(QUrl("qrc:/qml/theme/IngeScapeComboboxStyle.qml"), uri, 1, 0, "IngeScapeComboboxStyle");

    // - Scrollview style
    qmlRegisterType(QUrl("qrc:/qml/theme/IngeScapeScrollViewStyle.qml"), uri, 1, 0, "IngeScapeScrollViewStyle");

    // - Style of button for options in (popup) menu
    qmlRegisterType(QUrl("qrc:/qml/theme/ButtonStyleOfOption.qml"), uri, 1, 0, "ButtonStyleOfOption");

    // - Combobox delegate with tooltip
    qmlRegisterType(QUrl("qrc:/qml/IngeScapeToolTipComboboxDelegate.qml"), uri, 1, 0, "IngeScapeToolTipComboboxDelegate");

    // - Actions List Item
    qmlRegisterType(QUrl("qrc:/qml/ActionsListItem.qml"), uri, 1, 0, "ActionsListItem");

    // - Popups
    qmlRegisterType(QUrl("qrc:/qml/popup/ConfirmationPopup.qml"), uri, 1, 0, "ConfirmationPopup");
    qmlRegisterType(QUrl("qrc:/qml/popup/SystemInformationPopup.qml"), uri, 1, 0, "SystemInformationPopup");
    qmlRegisterType(QUrl("qrc:/qml/popup/MenuPopup.qml"), uri, 1, 0, "MenuPopup");

    // License popups
    qmlRegisterType(QUrl("qrc:/qml/license/LicensePopup.qml"), uri, 1, 0, "LicensePopup");
    qmlRegisterType(QUrl("qrc:/qml/license/LicenseEventPopup.qml"), uri, 1, 0, "LicenseEventPopup");

    // - Text area with timer to delay processing of edited text while the user is editing it
    qmlRegisterType(QUrl("qrc:/qml/TextAreaWithTimer.qml"), uri, 1, 0, "TextAreaWithTimer");

    // - Network configuration information
    qmlRegisterType(QUrl("qrc:/qml/NetworkConnectionInformationItem.qml"), uri, 1, 0, "NetworkConnectionInformationItem");

    // - Timeline
    qmlRegisterType(QUrl("qrc:/qml/timeline/ActionInTimeLine.qml"), uri, 1, 0, "ActionInTimeLine");
    qmlRegisterType(QUrl("qrc:/qml/timeline/ScenarioTimeLine.qml"), uri, 1, 0, "ScenarioTimeLine");

    // - Widgets
    qmlRegisterType(QUrl("qrc:/qml/widgets/LabellessSvgButton.qml"), uri, 1, 0, "LabellessSvgButton");
    qmlRegisterType(QUrl("qrc:/qml/widgets/LabellessSvgButtonCachedImage.qml"), uri, 1, 0, "LabellessSvgButtonCachedImage");
    qmlRegisterType(QUrl("qrc:/qml/widgets/SvgImage.qml"), uri, 1, 0, "SvgImage");



}

