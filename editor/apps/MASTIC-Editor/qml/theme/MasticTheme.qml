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

    // Greys
    readonly property color greyWindowBgColor: "#E4E4E4"
    readonly property color greySeparationColor: "#4D4D4D"
    readonly property color greyBottomPanelBgColor: "#CCCCCC"
    readonly property color greyDarkTextColor: "#808080"


    // Blues
    readonly property color blueRightPanelLightColor: "#42729E"
    readonly property color blueRightPanelDarkColor: "#2E4D6B"
    readonly property color blueSelectedColor: "#2222FF"


    // Other
    readonly property color redColor: "#FF0000"



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
                                                pixelSize: 12
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
    readonly property color agentsMappingLinkDefaultColor: "#ffffff"
    // - Hover state
    readonly property int agentsMappingLinkHoverFuzzyRadius: 4
    readonly property color agentsMappingLinkHoverFuzzyColor: "firebrick"

    // Press state
    readonly property color agentsMappingLinkPressColor: "firebrick"



}
