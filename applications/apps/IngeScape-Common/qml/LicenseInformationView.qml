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
    property LicenseInformationM licenseInformation: null

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
            color: rootItem.licenseInformation && rootItem.licenseInformation.ingescapeLicenseValidity ? "green" : "red"
        }
    }

    Text {
        text: rootItem.licenseInformation ? qsTr("Customer: %1").arg(rootItem.licenseInformation.customers) : ""

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight : Font.Medium
            pixelSize : 16
        }
    }

    Text {
        text: rootItem.licenseInformation ? qsTr("Order: %1").arg(rootItem.licenseInformation.orderNumbers) : ""

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight : Font.Medium
            pixelSize : 16
        }
    }

    Text {
        text: rootItem.licenseInformation ? qsTr("Expiration date: %1").arg(rootItem.licenseInformation.expirationDate.toLocaleString(Qt.locale(), "dd/MM/yyyy"))
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
            color: rootItem.licenseInformation && rootItem.licenseInformation.editorLicenseValidity ? "green" : "red"
        }
    }

    Text {
        text: rootItem.licenseInformation ? qsTr("Editor owner: %1").arg(rootItem.licenseInformation.editorOwners) : ""

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight : Font.Medium
            pixelSize : 16
        }
    }

    Text {
        text: rootItem.licenseInformation ? qsTr("Editor expiration date: %1").arg(rootItem.licenseInformation.editorExpirationDate.toLocaleString(Qt.locale(), "dd/MM/yyyy"))
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
        text: rootItem.licenseInformation ? qsTr("Maximum number of agents: %1").arg(rootItem.licenseInformation.maxNumberOfAgents) : ""

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            //weight: Font.Medium
            pixelSize: 16
        }
    }
    Text {
        text: rootItem.licenseInformation ? qsTr("Maximum number of agents inputs/outputs/parameters: %1").arg(rootItem.licenseInformation.maxNumberOfIOPs) : ""

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
        text: rootItem.licenseInformation ? qsTr("Features:") : ""

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight: Font.Medium
            pixelSize: 16
        }
    }

    Repeater {
        model: rootItem.licenseInformation ? rootItem.licenseInformation.features : null

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
        text: rootItem.licenseInformation ? qsTr("Agents:") : ""

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight: Font.Medium
            pixelSize: 16
        }
    }

    Repeater {
        model: rootItem.licenseInformation ? rootItem.licenseInformation.agents : null

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
