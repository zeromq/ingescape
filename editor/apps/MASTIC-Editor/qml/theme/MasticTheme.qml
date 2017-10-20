/*
 *	MASTIC Editor
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
import QtQuick 2.5
import I2Quick 1.0


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
    readonly property int bottomPanelHeight: 200


    //-----------------------------------------
    //
    // SVG files
    //
    //-----------------------------------------

    // - Main SVG file
    property var svgFileMASTIC : svgFileCacheMASTIC

    I2SvgFileCache {
        id: svgFileCacheMASTIC
        svgFile: "qrc:/resources/SVG/mastic-pictos.svg"
    }


    //-----------------------------------------
    //
    // Colors
    //
    //-----------------------------------------


    readonly property color whiteColor: "#FFFFFF"
    readonly property color blackColor: "#17191E"

    readonly property color redColor: "#E30613"

    readonly property color veryDarkGreyColor: "#282D34"
    readonly property color darkGreyColor: "#3C424F"
    readonly property color blueGreyColor: "#8896AA"
    readonly property color darkBlueGreyColor: "#535A66"




    readonly property color orangeColor: "#F39200"


    //-----------------------------------------
    //
    // Fonts
    //
    //-----------------------------------------

    // Default font family
    readonly property string defaultFontFamily: "Arial"


    property alias labelFontFamilyExtraBold : fontAkrobatExtraBold.name
    property alias labelFontFamilyBlack : fontAkrobatBlack.name

    property alias labelFontFamily : fontAkrobat.name
    property alias textFontFamily : fontBloggerSans.name


    FontLoader {
        id: fontAkrobat
        source: "qrc:/resources/fonts/akrobat/Akrobat.ttf"
    }

    FontLoader {
        id: fontAkrobatBlack
        source: "qrc:/resources/fonts/akrobat/Akrobat-Black.ttf"
    }

    FontLoader {
        id: fontAkrobatExtraBold
        source: "qrc:/resources/fonts/akrobat/Akrobat-ExtraBold.ttf"
    }

    FontLoader {
        id: fontBloggerSans
        source: "qrc:/resources/fonts/blogger-sans/Blogger_Sans.ttf"
    }



    // Heading font (level1)
    readonly property font headingFont: Qt.font({
                                                 family: labelFontFamilyBlack,
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



    //-----------------------------------------
    //
    // Agents list UI elements
    //
    //-----------------------------------------


    // Background
    readonly property color leftPanelBackgroundColor: theme.blackColor
    readonly property color selectedTabsBackgroundColor: theme.veryDarkGreyColor

    // Labels
    readonly property color agentsListLabelColor: theme.whiteColor
    readonly property color agentsListTextColor: theme.blueGreyColor

    readonly property color agentOFFLabelColor: theme.darkBlueGreyColor
    readonly property color agentOFFTextColor: theme.darkGreyColor


    // List
    readonly property color agentsListItemBackgroundColor: theme.veryDarkGreyColor

    // Selected Agent
    readonly property color selectedAgentColor: theme.orangeColor



    //-----------------------------------------
    //
    // Agent Definition editors UI elements
    //
    //-----------------------------------------

    // Background
    readonly property color definitionEditorsBackgroundColor: "#282C88"

    // Labels
    readonly property color definitionEditorsLabelColor: theme.whiteColor



    //-----------------------------------------
    //
    // Scenario UI elements
    //
    //-----------------------------------------


    // Background
    readonly property color scenarioBackgroundColor: "#3C424F"



    //-----------------------------------------
    //
    // Agents mapping UI elements
    //
    //-----------------------------------------

    //
    // Background
    //
    readonly property color agentsMappingBackgroundColor: "#282C34"
    // - grid
    readonly property color agentsMappingGridLineColor: "#323741"
    readonly property color agentsMappingGridSublineColor: "#29313A"


    //
    // Links
    //
    // - Default state
    readonly property int agentsMappingLinkDefaultWidth: 4
    readonly property color agentsMappingLinkDefaultColor: theme.whiteColor
    // - Hover state
    readonly property int agentsMappingLinkHoverFuzzyRadius: 4
    readonly property color agentsMappingLinkHoverFuzzyColor: "firebrick"

    // Press state
    readonly property color agentsMappingLinkPressColor: "firebrick"



}
