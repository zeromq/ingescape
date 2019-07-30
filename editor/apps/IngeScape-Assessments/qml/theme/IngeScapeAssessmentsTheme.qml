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
    readonly property int appMinWidth: 1530
    readonly property int appMinHeight: 768

    // Default size of our application (Window)
    readonly property int appDefaultWidth: 1920
    readonly property int appDefaultHeight: 1080


    //-----------------------------------------
    //
    // SVG files
    //
    //-----------------------------------------

    // - Main SVG file
    property var svgFileIngeScapeAssessments : svgFileCacheIngeScapeAssessments

    I2SvgFileCache {
        id: svgFileCacheIngeScapeAssessments
        svgFile: "qrc:/resources/SVG/ingescape-assessments-pictos-SVG.svg"
    }


    //-----------------------------------------
    //
    // Colors
    //
    //-----------------------------------------

    readonly property color darkBlueHeader: "#38444F"
    readonly property color blueButton: "#007b90"
    readonly property color blueButton_pressed: "#239eb3"
    readonly property color ingescapeTitleDarkGrey: "#666666"
    readonly property color ingescapeTitleMediumGrey: "#999999"
    readonly property color ingescapeTitleLightGrey: "#B3B3B3"



    //-----------------------------------------
    //
    // Fonts
    //
    //-----------------------------------------

}
