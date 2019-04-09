/*
 *	IngeScape Assessments
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

pragma Singleton
import QtQuick 2.9
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
    readonly property int bottomPanelHeight: 300
    //readonly property int bottomPanelHeight_OneRow: 100

    // Width of time (with format "hh:mm:ss.zzz")
    readonly property int timeWidth: 80


    //-----------------------------------------
    //
    // SVG files
    //
    //-----------------------------------------

    // - Main SVG file
    /*property var svgFileINGESCAPE : svgFileCacheINGESCAPE

    I2SvgFileCache {
        id: svgFileCacheINGESCAPE
        svgFile: "qrc:/resources/SVG/ingescape-pictos.svg"
    }*/


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
    //readonly property color middleDarkRedColor:"#851618"
    //readonly property color darkRedColor:"#7A110E"

    readonly property color darkBlueGreyColor: "#3C424F"
    //readonly property color blueGreyColor2: "#434B57"
    //readonly property color blueGreyColor : "#525A66"

    //readonly property color lightBlueGreyColor: "#8895AA"

    readonly property color veryDarkGreyColor: "#282D34"
    //readonly property color darkGreyColor2: "#2B3137"
    readonly property color darkGreyColor : "#575757"
    readonly property color greyColor : "#6F6F6F"
    //readonly property color greyColor2 : "#878787"
    //readonly property color lightGreyColor : "#9C9C9C"
    //readonly property color veryLightGreyColor : "#ECECEC"

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



    readonly property color disabledTextColor: "#3C3C3C"


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

}
