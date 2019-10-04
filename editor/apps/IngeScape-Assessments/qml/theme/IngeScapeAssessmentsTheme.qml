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
        svgFile: "qrc:/resources/SVG/ingescape_assessments-pictos.svg"
    }


    //-----------------------------------------
    //
    // Colors
    //
    //-----------------------------------------

    readonly property color lighterDarkBlueHeader: "#707D91"
    readonly property color regularDarkBlueHeader: "#434B57"
    readonly property color darkerDarkBlueHeader: "#2D333B"

    readonly property color blueButton: "#0F8598"
    readonly property color blueButton_pressed: "#13AEC7"
    readonly property color blueButton_disabled: "#CBCBCA"
    readonly property color blueButton_rollover: "#0A5A66"

    readonly property color ingescapeTitleDarkGrey: "#666666"
    readonly property color ingescapeTitleMediumGrey: "#999999"
    readonly property color ingescapeTitleLightGrey: "#B3B3B3"




    //-----------------------------------------
    //
    // Fonts
    //
    //-----------------------------------------

}
