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


Item {
    id: rootItem

    //anchors.fill: parent


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property SubjectsController controller: null;

    property ExperimentationM experimentation: controller ? controller.currentExperimentation : null;

    property int indexPreviousSelectedSubject: -1;
    property int indexSubjectCurrentlyEditing: -1;

    property real characteristicValueColumnWidth: 228


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Close Subjects view
    signal closeSubjectsView();



    //--------------------------------
    //
    //
    // Functions
    //
    //
    //--------------------------------

    /**
     * Edit a subject at a row index
     */
    function editSubjectAtRowIndex(rowIndex) {

        //console.log("QML: Edit the subject at the row index " + rowIndex);

        // Set the index
        rootItem.indexSubjectCurrentlyEditing = rowIndex;
    }


    /**
     * Stop the current edition of a subject
     */
    function stopCurrentEditionOfSubject() {
        if (rootItem.indexSubjectCurrentlyEditing > -1)
        {
            //console.log("QML: Stop the current edition of the subject at the row index " + rootItem.indexSubjectCurrentlyEditing);

            // Reset the index
            rootItem.indexSubjectCurrentlyEditing = -1;
        }
    }



    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

    Rectangle {
        id: background
        anchors.fill: parent
        color: IngeScapeTheme.veryLightGreyColor
    }

    Button {
        id: btnClose

        anchors {
            top: parent.top
            topMargin: 21
            right: parent.right
            rightMargin: 21
        }

        height: 40
        width: 40

        text: "X"

        onClicked: {
            // Emit the signal "closeSubjectsView"
            rootItem.closeSubjectsView();
        }
    }

    // Characteristics
    Item {
        id: characteristicsItem
        anchors {
            top: parent.top
            topMargin: 24
            left: parent.left
            leftMargin: 26
            bottom: parent.bottom
            bottomMargin: 24
        }

        width: 360

        Text {
            id: titleCharacteristics

            anchors {
                verticalCenter: btnNewCharacteristic.verticalCenter
                left: parent.left
            }

            text: qsTr("CHARACTERISTICS")

            height: parent.height
            verticalAlignment: Text.AlignVCenter

            color: IngeScapeTheme.blackColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 20
            }
        }

        Button {
            id: btnNewCharacteristic

            anchors {
                top: parent.top
                right: parent.right
            }

            height: 40
            width: 134

            onClicked: {
                // Open the popup
                createCharacteristicPopup.open();
            }

            //FIXME correct font
            style: IngeScapeAssessmentsButtonStyle {
                text: qsTr("ADD NEW")
            }
        }

        Rectangle {
            id: characteristicsList
            anchors {
                top: btnNewCharacteristic.bottom
                topMargin: 14
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            color: IngeScapeTheme.whiteColor

            Column {
                id: characteristicsColumn
                anchors.fill: parent
                spacing: 0

                Repeater {
                    model: rootItem.experimentation ? rootItem.experimentation.allCharacteristics : null

                    delegate: Characteristic {

                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        modelM: model.QtObject

                        //
                        // Slots
                        //
                        onDeleteCharacteristic: {
                            if (rootItem.controller) {
                                rootItem.controller.deleteCharacteristic(model.QtObject);
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            id: bottomShadow
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            height: 4
            gradient: Gradient {
                GradientStop { position: 0.0; color: IngeScapeTheme.whiteColor; }
                GradientStop { position: 1.0; color: IngeScapeTheme.darkGreyColor; }
            }
        }
    }

    // Subjects
    Item {
        anchors {
            top: parent.top
            topMargin: 80
            left: characteristicsItem.right
            leftMargin: 26
            right: parent.right
            rightMargin: 26
        }

        Text {
            id: titleSubjects

            anchors {
                verticalCenter: btnDownloadSubjects.verticalCenter
                left: parent.left
            }

            text: qsTr("SUBJECTS")

            height: parent.height
            verticalAlignment: Text.AlignVCenter

            color: IngeScapeTheme.blackColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 20
            }
        }

        Button {
            id: btnDownloadSubjects

            anchors {
                top: parent.top
                right: btnNewSubject.left
                rightMargin: 12
            }

            height: 40
            width: 182

            onClicked: {
                console.log("Not implemented yet")
            }

            //FIXME correct font
            style: IngeScapeAssessmentsButtonStyle {
                text: qsTr("DOWNLOAD LIST")
            }
        }

        Button {
            id: btnNewSubject

            anchors {
                top: parent.top
                right: parent.right
            }

            height: 40
            width: 182

            onClicked: {
                // Open the popup
                if (rootItem.controller)
                {
                    rootItem.controller.createNewSubject()
                }
            }

            //FIXME correct font
            style: IngeScapeAssessmentsButtonStyle {
                text: qsTr("NEW SUBJECT")
            }
        }

        IngeScapeAssessmentsListHeader {
            id: subjectListHeader
            anchors {
                top: btnDownloadSubjects.bottom
                topMargin: 12
                left: parent.left
                right: parent.right
            }

            Row {
                anchors {
                    top: parent.top
                    left: parent.left
                    leftMargin: 15
                    right: parent.right
                    bottom: parent.bottom
                }

                Repeater {
                    model: rootItem.experimentation ? rootItem.experimentation.allCharacteristics : null

                    Text {
                        anchors {
                            verticalCenter: parent.verticalCenter
                        }

                        width: rootItem.characteristicValueColumnWidth

                        text: model ? model.name : ""
                        color: IngeScapeTheme.whiteColor
                        font {
                            family: IngeScapeTheme.labelFontFamily
                            pixelSize: 16
                            weight: Font.Black
                        }
                    }
                }
            }
        }

        Rectangle {
            anchors {
                top: subjectListHeader.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            color: IngeScapeTheme.whiteColor

            Column {
                id: subjectsColumn
                anchors.fill: parent
                spacing: 0

                Repeater {
                    model: rootItem.experimentation ? rootItem.experimentation.allSubjects : null

                    delegate: Rectangle {
                        id: subjectDelegate
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        height: 40

                        property var subject: model ? model.QtObject : null
                        property bool isMouseHovering: itemMouseArea.containsMouse || editSubjectButton.containsMouse || deleteSubjectButton.containsMouse

                        color: subjectDelegate.isMouseHovering ? IngeScapeTheme.veryLightGreyColor : IngeScapeTheme.whiteColor

                        MouseArea {
                            id: itemMouseArea
                            anchors.fill: parent
                            hoverEnabled: true
                        }

                        Row {
                            anchors {
                                fill: parent
                                leftMargin: 15
                            }

                            Repeater {
                                model: rootItem.experimentation ? rootItem.experimentation.allCharacteristics : null

                                delegate: Text {
                                    id: startDate

                                    text: subjectDelegate.subject && subjectDelegate.subject.mapCharacteristicValues ? subjectDelegate.subject.mapCharacteristicValues[model.name] : ""
                                    width: rootItem.characteristicValueColumnWidth
                                    height: subjectDelegate.height

                                    verticalAlignment: Text.AlignVCenter

                                    color: IngeScapeTheme.blackColor
                                    font {
                                        family: IngeScapeTheme.textFontFamily
                                        //weight: Font.Medium
                                        pixelSize: 14
                                    }
                                }
                            }
                        }

                        Row {
                            spacing: 12

                            anchors {
                                right: parent.right
                                rightMargin: 18
                                verticalCenter: parent.verticalCenter
                            }

                            Button {
                                id: deleteSubjectButton
                                height: 30
                                width: 40

                                property bool containsMouse: __behavior.containsMouse

                                opacity: subjectDelegate.isMouseHovering ? 1 : 0

                                style: IngeScapeAssessmentsButtonStyle {
                                    text: "D"
                                }

                                onClicked: {
                                    console.log("Not implemented yet")
                                }
                            }

                            Button {
                                id: editSubjectButton
                                height: 30
                                width: 40

                                property bool containsMouse: __behavior.containsMouse

                                opacity: subjectDelegate.isMouseHovering ? 1 : 0

                                style: IngeScapeAssessmentsButtonStyle {
                                    text: "E"
                                }

                                onClicked: {
                                    console.log("Not implemented yet")
                                }
                            }
                        }

                        Rectangle {
                            id: bottomSeparator
                            anchors {
                                left: parent.left
                                right: parent.right
                                bottom: parent.bottom
                            }
                            height: 2
                            color: IngeScapeTheme.veryLightGreyColor
                        }
                    }
                }
            }
        }
    }

    //
    // Subjects Panel
    //
//    Rectangle {
//        id: subjectsPanel

//        anchors {
//            left: characteristicsItem.right
//            right: parent.right
//            top: header.bottom
//            topMargin: 30
//            bottom: parent.bottom
//        }

//        color: "transparent"
//        border {
//            color: IngeScapeTheme.darkGreyColor
//            width: 1
//        }


//        TableView {
//            id: tableSubjects

//            anchors {
//                fill: parent
//                margins: 10
//            }

//            rowDelegate: Rectangle {
//                width: childrenRect.width
//                height: styleData.selected ? 30 : 20

//                color: styleData.selected ? "lightblue"
//                                          : (styleData.alternate ? "lightgray" : "white")
//            }

//            headerDelegate: Rectangle {
//                height: 30
//                width: parent.width

//                color: "darkgray"

//                Text {
//                    id: txtColumnHeader

//                    anchors {
//                        fill: parent
//                        leftMargin: (styleData.column === 0) ? 110 : 5
//                    }
//                    verticalAlignment: Text.AlignVCenter
//                    horizontalAlignment: styleData.textAlignment

//                    text: styleData.value
//                    elide: Text.ElideRight
//                    color: IngeScapeTheme.blackColor
//                }

//                Rectangle {
//                    id: leftSeparator

//                    visible: (styleData.column === 0)

//                    anchors {
//                        left: parent.left
//                        leftMargin: 105
//                        top: parent.top
//                        bottom: parent.bottom
//                    }
//                    width: 1
//                    color: "silver"
//                }

//                Rectangle {
//                    id: rightSeparator

//                    anchors {
//                        right: parent.right
//                        top: parent.top
//                        bottom: parent.bottom
//                    }
//                    width: 1
//                    color: "silver"
//                }
//            }

//            model: rootItem.experimentation ? rootItem.experimentation.allSubjects : null

//            Instantiator {
//                id: columnsInstantiator

//                model: rootItem.experimentation ? rootItem.experimentation.allCharacteristics : null

//                delegate: TableViewColumn {
//                    id: column

//                    property CharacteristicM characteristic: model.QtObject

//                    role: model.name
//                    title: model.name

//                    width: (index === 0) ? 250 : 150

//                    delegate: CharacteristicValueEditor {
//                        id: characteristicValueEditor

//                        characteristic: column.characteristic

//                        characteristicValue: {
//                            // FIXME rendering based on litteral string is a source of error
//                            model ? (column.role === "ID" ? model.displayedId : model.mapCharacteristicValues[column.role]) : ""
//                        }

//                        isSelected: styleData.selected

//                        isCurrentlyEditing: (rootItem.indexSubjectCurrentlyEditing === styleData.row)


//                        //
//                        // Slots
//                        //

//                        onEditSubject: {
//                            // Edit the subject at the row index
//                            rootItem.editSubjectAtRowIndex(styleData.row);
//                        }

//                        onStopEditionOfSubject: {
//                            // Stop the current edition of the subject
//                            rootItem.stopCurrentEditionOfSubject();
//                        }

//                        onCharacteristicValueUpdated: {
//                            if (model)
//                            {
//                                //console.log("QML: on Characteristic Value Updated " + value);

//                                // Update the value (in C++)
//                                model.mapCharacteristicValues[column.role] = value;
//                            }
//                        }

//                        onDeleteSubject: {

//                            if (rootItem.controller && model)
//                            {
//                                //console.log("QML: Delete Subject " + model.name);

//                                // Stop the current edition of the subject
//                                stopCurrentEditionOfSubject();

//                                // Delete the subject
//                                rootItem.controller.deleteSubject(model.QtObject);

//                                // Clear the selection
//                                tableSubjects.selection.clear();
//                            }
//                        }
//                    }
//                }

//                onObjectAdded: {
//                    //console.log("onObjectAdded " + index);
//                    //tableSubjects.insertColumn(index, object);
//                    tableSubjects.addColumn(object);
//                }
//                onObjectRemoved: {
//                    //console.log("onObjectRemoved " + index);
//                    tableSubjects.removeColumn(index);
//                }
//            }

//            onDoubleClicked: {
//                //console.log("onDoubleClicked " + row);

//                // Edit the subject at the row index
//                editSubjectAtRowIndex(row);
//            }


//            // NOT Called
//            //onSelectionChanged: {
//            //}
//            // --> Workaround
//            Connections {
//                target: tableSubjects.selection

//                onSelectionChanged: {
//                    //console.log("onSelectionChanged:");

//                    if ((rootItem.indexPreviousSelectedSubject === rootItem.indexSubjectCurrentlyEditing) && (rootItem.indexSubjectCurrentlyEditing > -1))
//                    {
//                        // Stop the current edition of the subject
//                        stopCurrentEditionOfSubject();
//                    }

//                    // Reset
//                    rootItem.indexPreviousSelectedSubject = -1;

//                    tableSubjects.selection.forEach(function(rowIndex) {
//                        //console.log("row " + rowIndex + " is selected");

//                        // Update
//                        rootItem.indexPreviousSelectedSubject = rowIndex;
//                    })
//                }
//            }
//        }
//    }


    //
    // Create Characteristic Popup
    //
    Popup.CreateCharacteristicPopup {
        id: createCharacteristicPopup

        //anchors.centerIn: parent

        controller: rootItem.controller
    }

}
