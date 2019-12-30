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
 *      Mathieu Soum       <soum@ingenuity.io>
 *
 */

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0


AssessmentsPopupBase {
    id: rootPopup

    height: 330
    width: 626

    anchors.centerIn: parent

    title: "NEW SESSION"


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property ExperimentationController experimentationController: null;

    property ExperimentationM experimentation: null

    property date currentDate: new Date();


    //--------------------------------
    //
    //
    // Slots
    //
    //
    //--------------------------------

    onOpened: {
        rootPopup.currentDate = new Date();
    }


    //--------------------------------
    //
    //
    // Functions
    //
    //
    //--------------------------------

    //
    // Reset all user inputs and close the popup
    //
    function resetInputsAndClosePopup() {
        console.log("QML: Reset all user inputs and close popup");

        // Reset all user inputs
        //txtRecordName.text = "";
        comboSubjects.selectedIndex = -1;
        comboTasks.selectedIndex = -1;

        // Close the popup
        rootPopup.close();
    }


    //--------------------------------
    //
    // Content
    //
    //--------------------------------


    /*Item {
        id: nameRow

        anchors {
            top: parent.top
            topMargin: 30
            left: parent.left
            leftMargin: 28
            right: parent.right
            rightMargin: 28
        }

        height: 30

        Text {
            id: nameLabel
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }

            height: 30

            text: qsTr("Name :")

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        Text {
            id: taskInstanceName

            anchors {
                left: parent.left
                leftMargin: 94
                right: parent.right
                verticalCenter: parent.verticalCenter
            }

            height: 30

            // the subject and the task are defined
            text: if (comboSubjects.selectedItem && comboTasks.selectedItem) {
                      String("Session-%1-%2-%3").arg(comboSubjects.selectedItem.displayedId).arg(comboTasks.selectedItem.name).arg(rootPopup.currentDate.toLocaleString(Qt.locale(), "yyyyMMdd-hhmmss"));
                  }
            // Only the subject is defined
                  else if (comboSubjects.selectedItem) {
                      String("Session-%1-???-%2").arg(comboSubjects.selectedItem.displayedId).arg(rootPopup.currentDate.toLocaleString(Qt.locale(), "yyyyMMdd-hhmmss"));
                  }
            // Only the task is defined
                  else if (comboTasks.selectedItem) {
                      String("Session-???-%1-%2").arg(comboTasks.selectedItem.name).arg(rootPopup.currentDate.toLocaleString(Qt.locale(), "yyyyMMdd-hhmmss"));
                  }
                  else {
                      String("Session-???-???-%1").arg(rootPopup.currentDate.toLocaleString(Qt.locale(), "yyyyMMdd-hhmmss"));
                  }

            verticalAlignment: Text.AlignVCenter

            elide: Text.ElideRight
            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }
    }*/

    Item {
        id: subjectRow

        anchors {
            //top: nameRow.bottom
            top: parent.top
            topMargin: 30
            left: parent.left
            leftMargin: 28
            right: parent.right
            rightMargin: 28
        }

        height: 30

        Text {
            id: subjectLabel
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }

            height: 30

            text: qsTr("Subject :")

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        I2ComboboxItemModel {
            id: comboSubjects

            anchors {
                left: parent.left
                leftMargin: 94
                right: parent.right
                verticalCenter: parent.verticalCenter
            }

            height: 30

            model: rootItem.experimentation ? rootItem.experimentation.allSubjects : null

            enabled: rootItem.experimentation ? (rootItem.experimentation.allSubjects.count > 0)
                                              : false

            style: IngeScapeAssessmentsComboboxStyle {
                frameVisible: false
                listBackgroundColorIdle: IngeScapeTheme.veryLightGreyColor
                backgroundColorIdle: IngeScapeTheme.veryLightGreyColor
            }
            scrollViewStyle: IngeScapeAssessmentsScrollViewStyle {
                scrollBarSize: 4
                verticalScrollbarMargin: 2
            }

            function modelToString(_model) {
                if (_model) {
                    return _model.displayedId;
                }
                else {
                    return "";
                }
            }
        }
    }

    Item {
        id: taskRow

        anchors {
            top: subjectRow.bottom
            topMargin: 30
            left: parent.left
            leftMargin: 28
            right: parent.right
            rightMargin: 28
        }

        height: 30

        Text {
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }

            height: 30

            text: qsTr("Protocol :")

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        I2ComboboxItemModel {
            id: comboTasks

            anchors {
                left: parent.left
                leftMargin: 94
                right: parent.right
                verticalCenter: parent.verticalCenter
            }

            height: 30

            enabled: rootItem.experimentation && (rootItem.experimentation.allProtocols.count > 0)

            model: rootItem.experimentation ? rootItem.experimentation.allProtocols : null

            style: IngeScapeAssessmentsComboboxStyle {
                frameVisible: false
                listBackgroundColorIdle: IngeScapeTheme.veryLightGreyColor
                backgroundColorIdle: IngeScapeTheme.veryLightGreyColor
            }
            scrollViewStyle: IngeScapeAssessmentsScrollViewStyle {
                scrollBarSize: 4
                verticalScrollbarMargin: 2
            }

            function modelToString(_model) {
                if (_model) {
                    return _model.name;
                }
                else {
                    return "";
                }
            }
        }
    }


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
                // Reset all user inputs and close the popup
                rootPopup.resetInputsAndClosePopup();
            }
        }

        Button {
            id: okButton

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            anchors {
                verticalCenter: parent.verticalCenter
            }

            height: boundingBox.height
            width: boundingBox.width

            activeFocusOnPress: true

            //enabled: (taskInstanceName.text.length > 0) && (typeof comboSubjects.selectedItem !== 'undefined') && (typeof comboTasks.selectedItem !== 'undefined')
            enabled: (typeof comboSubjects.selectedItem !== 'undefined') && (typeof comboTasks.selectedItem !== 'undefined')

            style: IngeScapeAssessmentsButtonStyle {
                text: "OK"
            }

            onClicked: {
                if (experimentationController && comboSubjects.selectedItem && comboTasks.selectedItem)
                {
                    //console.log("QML: create new session with protocol " + comboTasks.selectedItem + " and subject " + comboSubjects.selectedItem);

                    // Create a new session for a subject and a protocol
                    experimentationController.createNewSessionForSubjectAndProtocol(comboSubjects.selectedItem, comboTasks.selectedItem);
                }

                // Reset all user inputs and close the popup
                rootPopup.resetInputsAndClosePopup();
            }
        }
    }
}
