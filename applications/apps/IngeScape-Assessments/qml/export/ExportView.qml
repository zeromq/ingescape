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

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

//import "theme" as Theme
import "../popup" as Popup


Popup.AssessmentsPopupBase {
    id: rootPopup

    height: 280
    width: 700

    anchors.centerIn: parent

    title: "Export selected sessions"

    canExitPopup : true


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    //property ExportController controller: null;

    //property ExperimentationM experimentation: controller ? controller.currentExperimentation : null;


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Signal raised when user confirm that he wants to export all agents outputs
    signal exportAllOutputs();

    // Signal raised when user confirm that he wants to export only dependent variables
    signal exportOnlyDependentVariables();



    //--------------------------------
    //
    //
    // Slots
    //
    //
    //--------------------------------

    onOpened: {
        checkAllOutputs.checked = false;
        checkOnlyVD.checked = false;
    }


    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

    //
    // Body of our popup
    //
    Column {
        anchors {
            left: parent.left
            leftMargin: 30
            top: parent.top
            topMargin: 30
        }
        spacing: 15

        Text {
            id: title

            anchors {
                left: parent.left
            }
            height: 20

            text: qsTr("You are about to export selected session(s). You can:")

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        ExclusiveGroup {
            id: exclusiveGroup
        }

        CheckBox {
            id: checkAllOutputs

            anchors {
                left: parent.left
            }
            height: 25

            checked: false
            exclusiveGroup: exclusiveGroup
            activeFocusOnPress: true;

            style: CheckBoxStyle {
                label: Text {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        verticalCenterOffset: 1
                    }

                    color: IngeScapeAssessmentsTheme.regularDarkBlueHeader

                    text: "Export all agents outputs of your protocol(s)"
                    elide: Text.ElideRight

                    font {
                        family: IngeScapeTheme.textFontFamily
                        pixelSize: 16
                    }
                }

                indicator: Rectangle {
                    width: 14
                    height: 14

                    color: IngeScapeTheme.veryDarkGreyColor

                    radius: 8

                    Rectangle {
                        anchors {
                            fill: parent
                            margins: 4
                        }

                        visible: (control.checkedState === Qt.Checked)

                        color: IngeScapeTheme.whiteColor

                        radius: 8
                    }
                }
            }
        }

        CheckBox {
            id: checkOnlyVD

            anchors {
                left: parent.left
            }
            height: 25

            checked: false
            exclusiveGroup: exclusiveGroup
            activeFocusOnPress: true;

            style: CheckBoxStyle {
                label: Text {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        verticalCenterOffset: 1
                    }

                    color: IngeScapeAssessmentsTheme.regularDarkBlueHeader

                    text: "Export only agents outputs that correspond to a dependent variable of your protocol(s)"
                    elide: Text.ElideRight

                    font {
                        family: IngeScapeTheme.textFontFamily
                        pixelSize: 16
                    }
                }

                indicator: Rectangle {
                    width: 14
                    height: 14

                    color: IngeScapeTheme.veryDarkGreyColor

                    radius: 8

                    Rectangle {
                        anchors {
                            fill: parent
                            margins: 4
                        }

                        visible: (control.checkedState === Qt.Checked)

                        color: IngeScapeTheme.whiteColor

                        radius: 8
                    }
                }
            }
        }
    }

    //
    // Footer of our popup
    //
    Row {
        anchors {
            right: parent.right
            rightMargin: 28
            bottom : parent.bottom
            bottomMargin: 28
        }
        spacing : 15

        Button {
            id: cancelButton

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            anchors {
                verticalCenter: parent.verticalCenter
            }

            height: boundingBox.height
            width: boundingBox.width


            visible: true

            activeFocusOnPress: true

            style: ButtonStyle {
                background: Rectangle {
                    anchors.fill: parent
                    radius: 5
                    color: control.pressed ? IngeScapeTheme.lightGreyColor : (control.hovered ? IngeScapeTheme.veryLightGreyColor : "transparent")
                }

                label: Text {
                    text: "Cancel"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    color: IngeScapeAssessmentsTheme.regularDarkBlueHeader

                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                    }
                }
            }

            onClicked: {
                // Close the popup
                rootPopup.close();
            }
        }

        Button {
            id: okButton

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            anchors {
                verticalCenter: parent.verticalCenter
            }

            height: boundingBox.height
            width: 150

            activeFocusOnPress: true

            enabled: checkAllOutputs.checked || checkOnlyVD.checked

            style: IngeScapeAssessmentsButtonStyle {
                text: "Export"
            }

            onClicked: {
                // Close the popup
                rootPopup.close();

                // Raise appropriate signal
                if (checkAllOutputs.checked)
                {
                    rootPopup.exportAllOutputs();
                }
                else if (checkOnlyVD.checked)
                {
                    rootPopup.exportOnlyDependentVariables();
                }
            }
        }
    }
}
