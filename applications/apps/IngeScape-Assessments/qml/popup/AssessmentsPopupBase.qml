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
 *      Mathieu Soum <soum@ingenuity.io>
 *
 */

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

import "../theme" as Theme

I2PopupBase {
    id: rootPopup

    default property alias data: popupContent.data

    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    isModal: true
    dismissOnOutsideTap: false
    keepRelativePositionToInitialParent: false

    // Popup title
    property alias title: titleText.text

    // Show the title ?
    property var showPopupTitle: true

    // Can user close the popup ?
    property bool canExitPopup : true


    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

    Rectangle {
        id: titleItem
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        visible: showPopupTitle
        enabled: visible

        height: visible ? 52 : 0
        color: IngeScapeAssessmentsTheme.blueButton

        Text {
            id: titleText

            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
                leftMargin: 28
            }

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.labelFontFamily
                weight: Font.Black
                pixelSize: 24
            }
        }

        Button {
            id: btnClose

            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 22
            }

            height: 20
            width: 20

            visible: rootPopup.canExitPopup

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "cancel-edition"
                disabledID: releasedID
            }

            onClicked: {
                close()
            }
        }

        I2SvgItem {
            svgFileCache: IngeScapeAssessmentsTheme.svgFileIngeScapeAssessments

            svgElementId: ""
        }
    }

    Rectangle {
        id: popupBackground

        anchors {
            top: titleItem.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        color: IngeScapeTheme.whiteColor

        Item {
            id: popupContent
            anchors.fill: parent

        }

    }

}
