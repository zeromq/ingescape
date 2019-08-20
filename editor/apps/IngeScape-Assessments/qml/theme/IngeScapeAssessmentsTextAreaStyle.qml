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
 *      Mathieu Soum        <soum@ingenuity.io>
 *
 */

import QtQuick 2.0
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

I2TextAreaStyle {
    backgroundColor: IngeScapeTheme.veryLightGreyColor
    borderColor: IngeScapeAssessmentsTheme.blueButton
    borderErrorColor: IngeScapeTheme.redColor
    radiusTextBox: 5
    borderWidth: 0;
    borderWidthActive: 2
    textIdleColor: IngeScapeAssessmentsTheme.regularDarkBlueHeader;
    textDisabledColor: IngeScapeAssessmentsTheme.lighterDarkBlueHeader

    padding.top: 10
    padding.bottom: 10
    padding.left: 12
    padding.right: 12


    //
    // Override scroll bar style
    //

    // Handles of scrollbars
    handle: Item {
        implicitWidth: 11
        implicitHeight: 11

        Rectangle {
            color: IngeScapeTheme.lightGreyColor

            anchors {
                fill: parent
                leftMargin: 3
            }

            opacity : 0.8
            radius: 10
        }
    }


    // Background of our scrollbar
    scrollBarBackground: Item {
        implicitWidth: rootStyle.scrollBarSize + rootStyle.verticalScrollbarMargin
        implicitHeight: rootStyle.scrollBarSize + rootStyle.horizontalScrollbarMargin
    }


    // Increment button of our scrollbars
    incrementControl: Item {
        implicitWidth: 1
        implicitHeight: 1
    }


    // Decrement button of our scrollbars
    decrementControl: Item {
        implicitWidth: 1
        implicitHeight: 1
    }

    font {
        pixelSize: 16
        family: IngeScapeTheme.textFontFamily
    }
}
