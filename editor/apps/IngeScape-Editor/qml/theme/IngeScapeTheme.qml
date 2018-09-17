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
//import INGESCAPE 1.0

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

    // Height of our bottom panel
    readonly property int bottomPanelHeight: 300
    readonly property int bottomPanelHeight_OneRow: 100

    // Width of time (with format "hh:mm:ss.zzz")
    readonly property int timeWidth: 80


    //-----------------------------------------
    //
    // SVG files
    //
    //-----------------------------------------

    // - Main SVG file
    property var svgFileINGESCAPE : svgFileCacheINGESCAPE

    I2SvgFileCache {
        id: svgFileCacheINGESCAPE
        svgFile: "qrc:/resources/SVG/ingescape-pictos.svg"
    }


    //-----------------------------------------
    //
    // Colors
    //
    //-----------------------------------------
    readonly property color whiteColor: "#FFFFFF"
    readonly property color blackColor: "#17191E"

    readonly property color loadingScreen_darkBlue: "#07101B"
    readonly property color loadingScreen_lightBlue: "#35404D"

    readonly property color redColor: "#E30513"
    readonly property color middleDarkRedColor:"#851618"
    readonly property color darkRedColor:"#7A110E"

    readonly property color darkBlueGreyColor: "#3C424F"
    readonly property color blueGreyColor2: "#434B57"
    readonly property color blueGreyColor : "#525A66"

    readonly property color lightBlueGreyColor: "#8895AA"

    readonly property color veryDarkGreyColor: "#282D34"
    readonly property color darkGreyColor2: "#2B3137"
    readonly property color darkGreyColor : "#575757"
    readonly property color greyColor : "#6F6F6F"
    readonly property color greyColor2 : "#878787"
    readonly property color lightGreyColor : "#9C9C9C"
    readonly property color veryLightGreyColor : "#ECECEC"

    readonly property color orangeColor: "#F07F0A"


    //-----------------------------------------
    //
    // Fonts
    //
    //-----------------------------------------

    // Default font family
    readonly property string defaultFontFamily: "Arial"


    property alias labelFontFamily: fontAkrobat.name
    property alias textFontFamily: fontBloggerSans.name
    property alias loadingFontFamily: fontTitillium.name

    FontLoader {
        id: fontAkrobat
        source: "qrc:/resources/fonts/akrobat/Akrobat.ttf"
    }

    FontLoader {
        source: "qrc:/resources/fonts/akrobat/Akrobat-Black.ttf"
    }

    FontLoader {
        source: "qrc:/resources/fonts/akrobat/Akrobat-ExtraBold.ttf"
    }

    FontLoader {
        id: fontBloggerSans
        source: "qrc:/resources/fonts/blogger-sans/Blogger_Sans.ttf"
    }

    FontLoader {
        source: "qrc:/resources/fonts/blogger-sans/Blogger_Sans-Medium.ttf"
    }

    FontLoader {
        source: "qrc:/resources/fonts/blogger-sans/Blogger_Sans-Light.ttf"
    }

    FontLoader {
        id: fontTitillium
        source: "qrc:/resources/fonts/titillium/Titillium-Regular.otf"
    }


    // Heading font (level1)
    readonly property font headingFont: Qt.font({
                                                    family: labelFontFamily,
                                                    pixelSize: 20,
                                                    weight: Font.Black
                                                });


    // Heading font (level2)
    readonly property font heading2Font: Qt.font({
                                                     family: textFontFamily,
                                                     pixelSize: 16
                                                 });

    // Normal font
    readonly property font normalFont: Qt.font({
                                                   family: textFontFamily,
                                                   pixelSize: 14
                                               });



    //-----------------------------------------
    //
    // Window
    //
    //-----------------------------------------

    // Background
    readonly property color windowBackgroundColor: "#E4E4E4"

    // Left panel lists Background
    readonly property color leftPanelBackgroundColor: theme.blackColor
    readonly property color selectedTabsBackgroundColor: theme.veryDarkGreyColor

    // Editors Background
    readonly property color editorsBackgroundColor: veryDarkGreyColor
    readonly property color editorsBackgroundBorderColor: greyColor


    //-----------------------------------------
    //
    // Agents list UI elements
    //
    //-----------------------------------------
    // Labels
    readonly property color agentsListLabelColor: theme.whiteColor
    readonly property color agentsListLabel2Color: theme.whiteColor
    readonly property color agentsListPressedLabel2Color: theme.lightGreyColor
    readonly property color agentsListTextColor: theme.lightBlueGreyColor

    readonly property color agentOFFLabelColor: theme.lightGreyColor
    readonly property color agentOFFLabel2Color: theme.lightGreyColor
    readonly property color agentOFFPressedLabel2Color: theme.greyColor
    readonly property color agentOFFTextColor: theme.blueGreyColor


    // List
    readonly property color agentsListItemBackgroundColor: theme.veryDarkGreyColor
    readonly property color agentsListItemRollOverBackgroundColor: theme.darkGreyColor2

    // Selected Agent
    readonly property color selectedAgentColor: theme.orangeColor



    //-----------------------------------------
    //
    // Agent Definition editors UI elements
    //
    //-----------------------------------------

    // Labels
    readonly property color definitionEditorsLabelColor: theme.whiteColor
    readonly property color definitionEditorsAgentDescriptionColor: theme.lightGreyColor



    //-----------------------------------------
    //
    // Scenario UI elements
    //
    //-----------------------------------------


    // Background
    readonly property color scenarioBackgroundColor: theme.veryDarkGreyColor


    // List Actions
    readonly property color actionsListItemBackgroundColor: theme.veryDarkGreyColor
    readonly property color actionsListItemRollOverBackgroundColor: theme.darkGreyColor2

    readonly property color actionsListLabelColor: theme.whiteColor
    readonly property color actionsListPressedLabelColor: theme.greyColor


    // Lines height in timeline
    property int lineInTimeLineHeight : 37


    //-----------------------------------------
    //
    // Agents mapping UI elements
    //
    //-----------------------------------------

    //
    // Background
    //
    readonly property color agentsMappingBackgroundColor: theme.blackColor
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
    readonly property color agentsMappingLinkDefaultColor: theme.whiteColor
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
            return (conditions === true) ? theme.whiteColor : theme.darkGreyColor

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
    readonly property color agentsONNameMappingColor : theme.whiteColor
    readonly property color agentsOFFNameMappingColor : theme.greyColor

    readonly property color agentsONInputsOutputsMappingColor : theme.whiteColor
    readonly property color agentsOFFInputsOutputsMappingColor : theme.lightGreyColor
}
