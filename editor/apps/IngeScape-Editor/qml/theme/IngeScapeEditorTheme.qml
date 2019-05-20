/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *      Justine Limoges    <limoges@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

pragma Singleton
import QtQuick 2.8
import I2Quick 1.0
import INGESCAPE 1.0


/**
  * Singleton that provides access to standard colors, fonts, etc.
  */
Item {
    id: theme


    //-----------------------------------------
    //
    // Sizes of UI elements
    //
    //-----------------------------------------

    // Minimum size of our application (Window)
    readonly property int appMinWidth: 1024
    readonly property int appMinHeight: 768

    // Default size of our application (Window)
    readonly property int appDefaultWidth: 1920
    readonly property int appDefaultHeight: 1080

    // Width of our left panel
    readonly property int leftPanelWidth: 320


    //-----------------------------------------
    //
    // SVG files
    //
    //-----------------------------------------

    // - Main SVG file
    property var svgFileIngeScapeEditor: svgFileCacheIngeScapeEditor

    I2SvgFileCache {
        id: svgFileCacheIngeScapeEditor
        svgFile: "qrc:/resources/SVG/ingescape-editor.svg"
    }


    //-----------------------------------------
    //
    // Colors
    //
    //-----------------------------------------



    //-----------------------------------------
    //
    // Fonts
    //
    //-----------------------------------------



    //-----------------------------------------
    //
    // Window
    //
    //-----------------------------------------

    // Left panel lists Background
    readonly property color selectedTabsBackgroundColor: IngeScapeTheme.veryDarkGreyColor



    //-----------------------------------------
    //
    // Agents list UI elements
    //
    //-----------------------------------------
    // Labels
    readonly property color agentsListLabelColor: IngeScapeTheme.whiteColor
    readonly property color agentsListLabel2Color: IngeScapeTheme.whiteColor
    readonly property color agentsListPressedLabel2Color: IngeScapeTheme.lightGreyColor
    readonly property color agentsListTextColor: IngeScapeTheme.lightBlueGreyColor

    readonly property color agentOFFLabelColor: IngeScapeTheme.lightGreyColor
    readonly property color agentOFFLabel2Color: IngeScapeTheme.lightGreyColor
    readonly property color agentOFFPressedLabel2Color: IngeScapeTheme.greyColor
    readonly property color agentOFFTextColor: IngeScapeTheme.blueGreyColor


    // List
    readonly property color agentsListItemBackgroundColor: IngeScapeTheme.veryDarkGreyColor
    readonly property color agentsListItemRollOverBackgroundColor: IngeScapeTheme.darkGreyColor2



    //-----------------------------------------
    //
    // Agent Definition editors UI elements
    //
    //-----------------------------------------

    // Labels
    readonly property color definitionEditorsLabelColor: IngeScapeTheme.whiteColor
    readonly property color definitionEditorsAgentDescriptionColor: IngeScapeTheme.lightGreyColor


    //-----------------------------------------
    //
    // Agents mapping UI elements
    //
    //-----------------------------------------

    //
    // Background
    //
    readonly property color agentsMappingBackgroundColor: IngeScapeTheme.blackColor
    // - grid
    readonly property color agentsMappingGridLineColor: "#3E414B"
    readonly property color agentsMappingGridSublineColor: "#232830"

    //
    // Links
    //
    // - stroke-width
    readonly property int agentsMappingLinkDefaultWidth: 3
    readonly property int agentsMappingBrinDefaultWidth: 6

    // - stroke-dasharray
    readonly property string agentsMappingLinkVirtualStrokeDashArray: "5, 5"
    readonly property string agentsMappingBrinVirtualStrokeDashArray: "10, 10"


    // - Default state
    readonly property color agentsMappingLinkDefaultColor: IngeScapeTheme.whiteColor

    // - Hover state
    readonly property int agentsMappingLinkHoverFuzzyRadius: 4
    readonly property color agentsMappingLinkHoverFuzzyColor: "firebrick"

    // Press state
    readonly property color agentsMappingLinkPressColor: "firebrick"


    // Input/Output type colors
    readonly property color yellowColor: "#ECB52E"
    readonly property color darkYellowColor: "#936F20"

    readonly property color redColor2: "#E33E36"
    readonly property color darkRedColor2: "#672B28"

    readonly property color purpleColor: "#854D97"
    readonly property color darkPurpleColor: "#483059"

    readonly property color greenColor: "#00AE8B"
    readonly property color darkGreenColor: "#1E564F"


    function colorOfIOPTypeWithConditions(iOPType, conditions) {
        switch (iOPType)
        {
        case 0: //AgentIOPValueTypeGroups.NUMBER:
            return (conditions === true) ? theme.yellowColor : theme.darkYellowColor

        case 1: //AgentIOPValueTypeGroups.STRING:
            return (conditions === true) ? theme.greenColor : theme.darkGreenColor

        case 2: //AgentIOPValueTypeGroups.IMPULSION:
           return (conditions === true) ? theme.purpleColor : theme.darkPurpleColor

        case 3: //AgentIOPValueTypeGroups.DATA:
            return (conditions === true) ? theme.redColor2 : theme.darkRedColor2

        case 4: //AgentIOPValueTypeGroups.MIXED:
            return (conditions === true) ? IngeScapeTheme.whiteColor : IngeScapeTheme.darkGreyColor

        case 5: //AgentIOPValueTypeGroups.UNKNOWN:
            return "#000000";

        default:
            return "#000000";
        }
    }


    function colorOfLogType(logType)
    {
        switch (logType)
        {
        case 0: //LogTypes.IGS_LOG_TRACE:
            // Light Blue
            return "#ADD8E6";

        case 1: //LogTypes.IGS_LOG_DEBUG:
            // Cyan
            return "#00FFFF";

        case 2: //LogTypes.IGS_LOG_INFO:
            // Green
            return "#00FF00";

        case 3: //LogTypes.IGS_LOG_WARNING:
            // Yellow
            return "#FFFF00";

        case 4: //LogTypes.IGS_LOG_ERROR:
            // Red
            return "#FF0000";

        case 5: //LogTypes.IGS_LOG_FATAL:
            // Magenta
            return "#FF00FF";

        default:
            return "#FFFFFF";
        }
    }


    //
    // Agents
    //
    readonly property color agentsONNameMappingColor: IngeScapeTheme.whiteColor
    readonly property color agentsOFFNameMappingColor: IngeScapeTheme.greyColor

    readonly property color agentsONInputsOutputsMappingColor : IngeScapeTheme.whiteColor
    readonly property color agentsOFFInputsOutputsMappingColor : IngeScapeTheme.lightGreyColor
}
