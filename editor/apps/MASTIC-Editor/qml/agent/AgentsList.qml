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

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import MASTIC 1.0

Item {
    id: rootItem
    anchors.fill: parent

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property var controller : null;


    //-----------------------------------------
    //
    // Functions
    //
    //-----------------------------------------


    //--------------------------------
    //
    // Content
    //
    //--------------------------------


    Row {
        id: headerRow1
        height: 25
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        Text {
            id: txtSearch
            text: qsTr("Rechercher...")
        }

        Text {
            id: txtFilter
            text: qsTr("Filtrer...")
        }
    }

    Row {
        id: headerRow2
        height: 25
        anchors {
            top: headerRow1.bottom
            left: parent.left
            right: parent.right
        }

        Button {
            id: btnAddAgent
            text: qsTr("Nouvel Agent")
            onClicked: {
                console.log("Nouvel Agent")
                // TODO
            }
        }

        Text {
            text: qsTr("Importer...")
        }
        Text {
            text: qsTr("Exporter...")
        }
    }

    ListView {
        id: agentsList
        anchors {
            top: headerRow2.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        model: MasticEditorC.modelManager.allAgentsVM

        spacing: 10

        delegate: agentInList
    }


    Component {
        id: agentInList

        Rectangle {
            width: MasticTheme.leftPanelWidth
            height: 120
            color: "yellow"

            Column {
                width: MasticTheme.leftPanelWidth

                Text {
                    text: model.modelM.name
                    height: 25
                }

                Text {
                    text: model.modelM.description
                    height: 25
                }

                Text {
                    text: model.modelM.version
                    height: 25
                }
            }

            Button {
                id: btnDefinition
                anchors {
                    top: parent.top
                    right: parent.right
                }

                text: "Définition"

                onClicked: {
                    console.log("Open the definition of " + model.modelM.name)

                    //model.openEditor();
                }
            }
        }
    }
}
