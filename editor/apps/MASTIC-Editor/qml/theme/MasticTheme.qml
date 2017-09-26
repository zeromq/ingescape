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

    // - Standard font family
    //readonly property string fontFamily: "Source Sans Pro"
    //readonly property string lightFontStyleName: "Light"


    /*FontLoader {
        id:sourceSansProBold
        source: "qrc:/Resources/fonts/source-sans-pro/SourceSansPro-Bold.ttf"
    }

    FontLoader {
        id:sourceSansProBoldItalic
        source: "qrc:/Resources/fonts/source-sans-pro/SourceSansPro-BoldItalic.ttf"
    }

    FontLoader {
        id:sourceSansProItalic
        source: "qrc:/Resources/fonts/source-sans-pro/SourceSansPro-Italic.ttf"
    }

    FontLoader {
        id:sourceSansProRegular
        source: "qrc:/Resources/fonts/source-sans-pro/SourceSansPro-Regular.ttf"
    }

    FontLoader {
        id:sourceSansProLight
        source: "qrc:/Resources/fonts/source-sans-pro/SourceSansPro-Light.ttf"
    }

    FontLoader {
        id:sourceSansProLightItalic
        source: "qrc:/Resources/fonts/source-sans-pro/SourceSansPro-LightItalic.ttf"
    }*/
}
