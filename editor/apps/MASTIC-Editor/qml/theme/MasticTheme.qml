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
    readonly property int leftPanelWidth: 400

    // Height of our bottom panel
    readonly property int bottomPanelHeight: 200


    //-----------------------------------------
    //
    // SVG files
    //
    //-----------------------------------------

    // - Main SVG file
    /*property var svgFileMASTIC : svgFileCacheMASTIC

    I2SvgFileCache {
        id: svgFileCacheMASTIC
        svgFile: "qrc:/resources/svg/MASTIC.svg"
    }*/


    //-----------------------------------------
    //
    // Colors
    //
    //-----------------------------------------


    readonly property color whiteColor: "#FFFFFF"
    readonly property color blackColor: "#000000"



    //-----------------------------------------
    //
    // Fonts
    //
    //-----------------------------------------

    // Default font family
    readonly property string defaultFontFamily: "Arial"


    // Normal font
    readonly property font normalFont: Qt.font({
                                                family: defaultFontFamily,
                                                pixelSize: 14
                                                });


    // Heading font (level1)
    readonly property font headingFont: Qt.font({
                                                 family: defaultFontFamily,
                                                 pixelSize: 50,
                                                 weight: Font.Bold
                                                 });


    // Heading font (level2)
    readonly property font heading2Font: Qt.font({
                                                 family: defaultFontFamily,
                                                 pixelSize: 20,
                                                 weight: Font.Bold
                                                 });




    /*
    FontLoader {
        id:sourceSansProRegular
        source: "qrc:/Resources/fonts/source-sans-pro/SourceSansPro-Regular.ttf"
    }
    */



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
    readonly property color agentsListBackgroundColor: "#282C34"


    // Header
    readonly property color agentsListHeaderBackgroundColor: "#3C424F"

    // Labels
    readonly property color agentsListLabelColor: theme.whiteColor

    // List
    readonly property color agentsListItemBackgroundColor: "#3C424F"


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
