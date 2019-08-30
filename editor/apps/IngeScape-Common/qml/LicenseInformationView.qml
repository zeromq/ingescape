/*
 *	IngeScape Editor
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Mathieu Soum     <soum@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

Column {
    id: rootItem
    property LicensesController controller: null

    spacing: 8
    Row {
        spacing: 10

        Text {
            text: qsTr("License validity:")

            anchors {
                verticalCenter: parent.verticalCenter
            }

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
        }

        Rectangle {
            id: flag
            width: 20
            height: 20
            radius: 10
            color: rootItem.controller && rootItem.controller.isLicenseValid ? "green" : "red"
        }
    }

    Text {
        text: rootItem.controller ? qsTr("Customer: %1").arg(rootItem.controller.licenseCustomer) : ""

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight : Font.Medium
            pixelSize : 16
        }
    }

    Text {
        text: rootItem.controller ? qsTr("Order: %1").arg(rootItem.controller.licenseOrder) : ""

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight : Font.Medium
            pixelSize : 16
        }
    }

    Text {
        text: rootItem.controller ? qsTr("Expiration date: %1").arg(rootItem.controller.licenseExpirationDate.toLocaleString(Qt.locale(), "dd/MM/yyyy"))
                                  : ""

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight : Font.Medium
            pixelSize : 16
        }
    }

    // Vertical space
    Item {
        anchors {
            left: parent.left
            right: parent.right
        }
        height: 10
    }

    Row {
        spacing: 10

        Text {
            text: qsTr("Editor license validity:")

            anchors {
                verticalCenter: parent.verticalCenter
            }

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
        }

        Rectangle {
            id: flagEditor
            width: 20
            height: 20
            radius: 10
            color: rootItem.controller && rootItem.controller.isEditorLicenseValid ? "green" : "red"
        }
    }

    Text {
        text: rootItem.controller ? qsTr("Editor owner: %1").arg(rootItem.controller.editorOwner) : ""

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight : Font.Medium
            pixelSize : 16
        }
    }

    Text {
        text: rootItem.controller ? qsTr("Editor expiration date: %1").arg(rootItem.controller.editorExpirationDate.toLocaleString(Qt.locale(), "dd/MM/yyyy"))
                                  : ""

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight : Font.Medium
            pixelSize : 16
        }
    }

    // Vertical space
    Item {
        anchors {
            left: parent.left
            right: parent.right
        }
        height: 10
    }

    Text {
        text: rootItem.controller ? qsTr("Maximum number of agents: %1").arg(rootItem.controller.maxNbOfAgents) : ""

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            //weight: Font.Medium
            pixelSize: 16
        }
    }
    Text {
        text: rootItem.controller ? qsTr("Maximum number of agents inputs/outputs/parameters: %1").arg(rootItem.controller.maxNbOfIOPs) : ""

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            //weight: Font.Medium
            pixelSize: 16
        }
    }

    // Vertical space
    Item {
        anchors {
            left: parent.left
            right: parent.right
        }
        height: 10
    }

    Text {
        text: rootItem.controller ? qsTr("Features:") : ""

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight: Font.Medium
            pixelSize: 16
        }
    }

    Repeater {
        model: rootItem.controller ? rootItem.controller.featureNames : null

        Text {
            text: " - " + modelData

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight: Font.Medium
                pixelSize: 16
            }
        }
    }

    // Vertical space
    Item {
        anchors {
            left: parent.left
            right: parent.right
        }
        height: 10
    }

    Text {
        text: rootItem.controller ? qsTr("Agents:") : ""

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight: Font.Medium
            pixelSize: 16
        }
    }

    Repeater {
        model: rootItem.controller ? rootItem.controller.agentNames : null

        Text {
            text: " - " + modelData

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight: Font.Medium
                pixelSize: 16
            }
        }
    }
}
