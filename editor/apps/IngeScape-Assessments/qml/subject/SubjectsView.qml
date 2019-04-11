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
import QtQml 2.12

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

    property IngeScapeModelManager modelManager: null;

    property ExperimentationM experimentation: modelManager ? modelManager.currentExperimentation : null;

    property int indexSubjectCurrentlyEditing: -1;


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Close Subjects view
    signal closeSubjectsView();



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

        color: "#FF333366"
    }

    Button {
        id: btnClose

        anchors {
            right: parent.right
            top: parent.top
        }

        text: "X"

        onClicked: {
            console.log("QML: close Subjects view");

            // Emit the signal "closeSubjectsView"
            rootItem.closeSubjectsView();
        }
    }

    Row {
        id: header

        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }

        spacing: 20

        Text {
            id: title

            text: "Subjects"

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 20
            }
        }

        Button {
            text: "New Subject"

            onClicked: {
                //console.log("QML: New Subject");

                if (rootItem.controller) {
                    rootItem.controller.createNewSubject();
                }
            }
        }
    }


    //
    // Characteristics Panel
    //
    Rectangle {
        id: characteristicsPanel

        anchors {
            left: parent.left
            top: header.bottom
            topMargin: 30
            bottom: parent.bottom
        }
        width: 350

        color: "#44222222"

        Row {
            id: characteristicsHeader

            anchors {
                left: parent.left
                top: parent.top
            }

            spacing: 20

            Text {
                text: "Characteristics"

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 16
                }
            }

            Button {
                text: "New Characteristic"

                onClicked: {
                    // Open the popup
                    createCharacteristicPopup.open();
                }
            }
        }

        Column {
            anchors {
                top: characteristicsHeader.bottom
                topMargin: 20
                left: parent.left
                leftMargin: 5
                right: parent.right
                rightMargin: 5
            }

            Repeater {
                model: rootItem.experimentation ? rootItem.experimentation.allCharacteristics : null

                delegate: Characteristic {

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


    //
    // Subjects Panel
    //
    Rectangle {
        id: subjectsPanel

        anchors {
            left: characteristicsPanel.right
            right: parent.right
            top: header.bottom
            topMargin: 30
            bottom: parent.bottom
        }

        color: "#44AAAAAA"


        TableView {
            id: tableSubjects

            anchors {
                fill: parent
                margins: 10
            }

            rowDelegate: Rectangle {
                width: childrenRect.width
                height: 30

                color: styleData.selected ? "lightblue"
                                          : (styleData.alternate ? "lightgray" : "white")
            }

            model: rootItem.experimentation ? rootItem.experimentation.allSubjects : null

            TableViewColumn {
                role: "name"
                title: "Name"

                width: 350

                delegate: Item {

                    anchors.fill: parent

                    Row {
                        id: rowSubjectOptions

                        anchors {
                            top: parent.top
                            bottom: parent.bottom
                        }

                        spacing: 5

                        visible: styleData.selected

                        Button {
                            id: btnEdit

                            anchors {
                                top: parent.top
                            }
                            width: 70
                            height: 30

                            checkable: true

                            checked: (rootItem.indexSubjectCurrentlyEditing === styleData.row)

                            text: checked ? "Validate" : "Edit"

                            onClicked: {

                                if (rootItem.indexSubjectCurrentlyEditing === styleData.row)
                                {
                                    console.log("QML: Validate Subject " + styleData.value + " (" + styleData.row + ")");

                                    // Reset the index
                                    rootItem.indexSubjectCurrentlyEditing = -1;
                                }
                                else
                                {
                                    console.log("QML: Edit Subject " + styleData.value + " (" + styleData.row + ")");

                                    // Set the index
                                    rootItem.indexSubjectCurrentlyEditing = styleData.row;
                                }
                            }
                        }

                        Button {
                            id: btnDelete

                            anchors {
                                top: parent.top
                            }
                            width: 70
                            height: 30

                            text: "Delete"

                            onClicked: {
                                if (rootItem.controller && rootItem.experimentation)
                                {
                                    var subject = rootItem.experimentation.allSubjects.get(styleData.row);

                                    if (subject)
                                    {
                                        //console.log("QML: Delete Subject " + subject.name);

                                        rootItem.controller.deleteSubject(subject);

                                        // Reset the index
                                        rootItem.indexSubjectCurrentlyEditing = -1;

                                        // Clear the selection
                                        tableSubjects.selection.clear();
                                    }
                                }



                                /*if (rootItem.modelM)
                                {
                                    //console.log("QML: Delete Subject " + rootItem.modelM.uid);

                                    // Emit the signal "Delete Subject"
                                    rootItem.deleteSubject();
                                }*/

                                /*if (rootItem.controller) {
                                    rootItem.controller.deleteSubject(model.QtObject);
                                }*/
                            }
                        }
                    }

                    Rectangle {

                        width: 200
                        height: parent.height

                        anchors {
                            left: styleData.selected ? rowSubjectOptions.right : parent.left
                            top: parent.top
                            bottom: parent.bottom
                        }

                        color: "transparent"
                        border {
                            color: "black"
                            width: 1
                        }

                        Text {
                            text: styleData.value
                        }
                    }
                }
            }

            Instantiator {
                id: columnsInstantiator

                model: rootItem.experimentation ? rootItem.experimentation.allCharacteristics : null

                delegate: TableViewColumn {
                    id: column

                    property CharacteristicM characteristic: model.QtObject

                    role: model.name
                    title: model.name

                    width: 150

                    delegate: CharacteristicValueEditor {
                        id: characteristicValueEditor

                        characteristic: column.characteristic
                        //characteristicValueType: column.characteristic ? column.characteristic.valueType : CharacteristicValueTypes.UNKNOWN;

                        characteristicValue: model.propertyMap[column.role]

                        isCurrentlyEditing: (rootItem.indexSubjectCurrentlyEditing === styleData.row)

                        onCharacteristicValueChanged: {
                            // QQmlDMAbstractItemModelData
                            console.log("QML: Subjects View on Characteristic Value Changed " + characteristicValue);
                            model.propertyMap[column.role] = characteristicValue;
                        }
                    }
                }

                onObjectAdded: {
                    console.log("onObjectAdded " + index)
                    tableSubjects.insertColumn(index, object);
                }
                onObjectRemoved: {
                    console.log("onObjectRemoved " + index)
                    tableSubjects.removeColumn(index);
                }
            }

            onSelectionChanged: {
                console.log("onSelectionChanged " + tableSubjects.selection);

                if (rootItem.indexSubjectCurrentlyEditing > -1)
                {
                    console.log("TODO " + rootItem.indexSubjectCurrentlyEditing);
                }
            }
            onDoubleClicked: {
                console.log("onDoubleClicked ");
            }
        }

        /*Rectangle {
            id: tableHeader

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            height: 30

            color: "#88222222"
            border {
                color: "black"
                width: 1
            }

            Row {
                anchors.fill: parent

                spacing: 0

                Repeater {
                    model: rootItem.experimentation ? rootItem.experimentation.allCharacteristics : null

                    delegate: Rectangle {
                        id: headerColumn

                        anchors {
                            top: parent.top
                            bottom: parent.bottom
                        }
                        width: 150

                        color: "transparent"
                        border {
                            color: "black"
                            width: 1
                        }

                        Text {
                            anchors.centerIn: parent

                            text: model.name

                            color: IngeScapeTheme.whiteColor
                            font {
                                family: IngeScapeTheme.textFontFamily
                                weight : Font.Medium
                                pixelSize : 12
                            }
                        }
                    }
                }
            }
        }*/

        /*Column {
            anchors {
                left: parent.left
                right: parent.right
                top: tableHeader.bottom
                bottom: parent.bottom
            }

            Repeater {
                model: rootItem.experimentation ? rootItem.experimentation.allSubjects : null

                delegate: Subject {

                    modelM: model.QtObject
                    allCharacteristics: rootItem.experimentation.allCharacteristics


                    //
                    // Slots
                    //
                    onDeleteSubject: {
                        if (rootItem.controller) {
                            rootItem.controller.deleteSubject(model.QtObject);
                        }
                    }
                }
            }
        }*/
    }


    //
    // Create Characteristic Popup
    //
    Popup.CreateCharacteristicPopup {
        id: createCharacteristicPopup

        //anchors.centerIn: parent

        controller: rootItem.controller
    }

}
