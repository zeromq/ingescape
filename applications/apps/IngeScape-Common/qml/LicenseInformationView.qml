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

Item {
    id: rootItem
    property LicenseInformationM licenseInformation: null

    property string subtitlesFont : IngeScapeTheme.textFontFamily
    property real subtitlesCapitalization : Font.AllUppercase
    property real subtitlesWeight : Font.Medium
    property real subtitlesPixelSize : 18
    property color subtitlesColor : IngeScapeTheme.whiteColor

    property string propertiesFont : IngeScapeTheme.textFontFamily
    property real propertiesWeight : Font.Medium
    property real propertiesPixelSize : 14
    property color propertiesNameColor : IngeScapeTheme.lightGreyColor
    property color propertiesValueColor : IngeScapeTheme.whiteColor


    height: childrenRect.height

    // Editor license informations
    SvgImage {
        id: editorPicto

        anchors {
            top : parent.top
            left: parent.left
        }

        svgElementId : rootItem.licenseInformation && rootItem.licenseInformation.editorLicenseValidity ? "license-editor-ok" : "license-editor-fail"
    }

    Text {
        id : editorText

        anchors {
            verticalCenter: editorPicto.verticalCenter
            left: editorPicto.right
            leftMargin: 8
        }

        text: qsTr("Editor")

        color: rootItem.subtitlesColor
        font {
            family: rootItem.subtitlesFont
            capitalization: rootItem.subtitlesCapitalization
            weight : rootItem.subtitlesWeight
            pixelSize : rootItem.subtitlesPixelSize
        }
    }

    Column {
        id : editorInfosColumn

        anchors {
            top: editorPicto.bottom
            topMargin: 8
            left: editorText.left
            right: parent.right
        }

        spacing: 8

        Item {
            id: ownerInfo

            anchors {
                left : parent.left
                right: parent.right
            }

            height: childrenRect.height

            Text {
                id: ownertext

                anchors.left: ownerInfo.left

                text: qsTr("Owners: ")

                color: rootItem.propertiesNameColor
                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }

            }

            Text {
                anchors {
                    left: ownertext.right
                    right: parent.right
                }

                text: rootItem.licenseInformation ? rootItem.licenseInformation.editorOwners : ""

                elide: Text.ElideRight

                color: rootItem.propertiesValueColor
                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }
            }
        }

        Item {
            id : expirationDateInfo

            anchors {
                left : parent.left
                right: parent.right
            }

            height: childrenRect.height

            Text {
                id: expirationDatetext

                anchors.left: expirationDateInfo.left

                text: qsTr("Expiration date: ")

                color: rootItem.propertiesNameColor
                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }

            }

            Text {
                anchors {
                    left: expirationDatetext.right
                    right: parent.right
                }

                text: rootItem.licenseInformation ? rootItem.licenseInformation.editorExpirationDate.toLocaleString(Qt.locale(), "dd/MM/yyyy") : ""

                elide: Text.ElideRight

                color: rootItem.propertiesValueColor
                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }
            }
        }
    }


    // Platform license informations
    SvgImage {
        id: platformPicto

        anchors {
            top : editorInfosColumn.bottom
            topMargin : 22
            right: editorPicto.right
        }

        svgElementId : rootItem.licenseInformation && rootItem.licenseInformation.ingescapeLicenseValidity ? "platform-ok" : "platform-fail"
    }

    Text {
        id : platformText

        anchors {
            verticalCenter: platformPicto.verticalCenter
            left: platformPicto.right
            leftMargin: 8
        }

        text: qsTr("Platform")

        color: rootItem.subtitlesColor
        font {
            family: rootItem.subtitlesFont
            capitalization: rootItem.subtitlesCapitalization
            weight : rootItem.subtitlesWeight
            pixelSize : rootItem.subtitlesPixelSize
        }
    }

    Column {
        id : platformInfosColumn

        anchors {
            top: platformPicto.bottom
            topMargin: 8
            left: platformText.left
            right: parent.right
        }

        spacing: 8

        Item {
            id: customerInfo

            anchors {
                left : parent.left
                right: parent.right
            }

            height: childrenRect.height

            Text {
                id: customertext

                anchors.left: customerInfo.left

                text: qsTr("Customers: ")

                color: rootItem.propertiesNameColor
                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }

            }

            Text {
                anchors {
                    left: customertext.right
                    right: parent.right
                }

                text: rootItem.licenseInformation ? rootItem.licenseInformation.customers : ""

                elide: Text.ElideRight

                color: rootItem.propertiesValueColor
                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }
            }
        }

        Item {
            id : orderInfo

            anchors {
                left : parent.left
                right: parent.right
            }

            height: childrenRect.height

            Text {
                id: orderText

                anchors.left: orderInfo.left

                text: qsTr("Orders: ")

                color: rootItem.propertiesNameColor
                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }

            }

            Text {
                anchors {
                    left: orderText.right
                    right: parent.right
                }

                text: rootItem.licenseInformation ? rootItem.licenseInformation.orderNumbers : ""

                elide: Text.ElideRight

                color: rootItem.propertiesValueColor
                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }

            }
        }

        Item {
            id : editorExpirationDateInfo

            anchors {
                left : parent.left
                right: parent.right
            }

            height: childrenRect.height

            Text {
                id: editorExpirationDateText

                anchors.left: editorExpirationDateInfo.left

                text: qsTr("Editor expiration date: ") // TODO pourquoi "Editor" ???

                color: rootItem.propertiesNameColor
                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }

            }

            Text {
                anchors {
                    left: editorExpirationDateText.right
                    right: parent.right
                }

                text: rootItem.licenseInformation ? rootItem.licenseInformation.expirationDate.toLocaleString(Qt.locale(), "dd/MM/yyyy") : ""

                elide: Text.ElideRight

                color: rootItem.propertiesValueColor
                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }
            }
        }

        Row {
            anchors {
                left : parent.left
                right: parent.right
            }

            height: 64

            spacing: 20

            Item {
                id: agentsInfos

                anchors.verticalCenter: parent.verticalCenter

                height: childrenRect.height
                width: childrenRect.width

                Text {
                    id: agentsValue

                    anchors.horizontalCenter: maxAgentsText.horizontalCenter

                    text: rootItem.licenseInformation ? rootItem.licenseInformation.maxNumberOfAgents.toString() : ""

                    color: rootItem.propertiesValueColor
                    font {
                        family: rootItem.propertiesFont
                        weight : rootItem.propertiesWeight
                        pixelSize : 20
                    }
                }

                Text {
                    id: maxAgentsText
                    anchors {
                        left: parent.left
                        top: agentsValue.bottom
                        topMargin: 8
                    }

                    text: qsTr("Max AGENTS")

                    elide: Text.ElideRight

                    color: rootItem.propertiesNameColor
                    font {
                        family: rootItem.propertiesFont
                        weight : rootItem.propertiesWeight
                        pixelSize : rootItem.propertiesPixelSize
                    }
                }
            }

            Rectangle {
                id: separator

                anchors.verticalCenter: parent.verticalCenter

                height : agentsInfos.height
                width: 1
                color: IngeScapeTheme.lightGreyColor
            }

            Item {
                id: iopsInfos

                anchors.verticalCenter: parent.verticalCenter

                height: childrenRect.height
                width: childrenRect.width

                Text {
                    id: iopsValue

                    anchors.horizontalCenter: maxIOPsText.horizontalCenter

                    text: rootItem.licenseInformation ? rootItem.licenseInformation.maxNumberOfAgents.toString() : ""

                    color: rootItem.propertiesValueColor
                    font {
                        family: rootItem.propertiesFont
                        weight : rootItem.propertiesWeight
                        pixelSize : 20
                    }
                }

                Text {
                    id: maxIOPsText
                    anchors {
                        left: parent.left
                        top: iopsValue.bottom
                        topMargin: 8
                    }

                    text: qsTr("Max IOP")

                    elide: Text.ElideRight

                    color: rootItem.propertiesNameColor
                    font {
                        family: rootItem.propertiesFont
                        weight : rootItem.propertiesWeight
                        pixelSize : rootItem.propertiesPixelSize
                    }
                }
            }


        }

        Item {
            id : featuresInfo

            anchors {
                left : parent.left
                right: parent.right
            }

            height: childrenRect.height

            Text {
                id: featuresText

                anchors.left: featuresInfo.left

                text: qsTr("Features: ")

                color: rootItem.propertiesNameColor
                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }

            }

            Text {
                anchors {
                    left: featuresText.right
                    right: parent.right
                }

                text: rootItem.licenseInformation ? rootItem.licenseInformation.features.join(", ") : ""

                elide: Text.ElideRight

                color: rootItem.propertiesValueColor
                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }
            }
        }

        Item {
            id : agentsInfo

            anchors {
                left : parent.left
                right: parent.right
            }

            height: childrenRect.height

            Text {
                id: agentsText

                anchors.left: agentsInfo.left

                text: qsTr("Agents: ")

                color: rootItem.propertiesNameColor
                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }

            }

            Text {
                anchors {
                    left: agentsText.right
                    right: parent.right
                }

                text: rootItem.licenseInformation ? rootItem.licenseInformation.agents.join(", ") : ""

                elide: Text.ElideRight

                color: rootItem.propertiesValueColor
                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }
            }
        }
    }



//    Row {
//        spacing: 10

//        Text {
//            text: qsTr("Editor license validity:")

//            anchors {
//                verticalCenter: parent.verticalCenter
//            }

//            color: IngeScapeTheme.whiteColor
//            font {
//                family: IngeScapeTheme.textFontFamily
//                weight : Font.Medium
//                pixelSize : 16
//            }
//        }

//        Rectangle {
//            id: flagEditor
//            width: 20
//            height: 20
//            radius: 10
//            color: rootItem.licenseInformation && rootItem.licenseInformation.editorLicenseValidity ? "green" : "red"
//        }
//    }

//    Text {
//        text: rootItem.licenseInformation ? qsTr("Maximum number of agents: %1").arg(rootItem.licenseInformation.maxNumberOfAgents) : ""

//        color: IngeScapeTheme.whiteColor
//        font {
//            family: IngeScapeTheme.textFontFamily
//            //weight: Font.Medium
//            pixelSize: 16
//        }
//    }
//    Text {
//        text: rootItem.licenseInformation ? qsTr("Maximum number of agents inputs/outputs/parameters: %1").arg(rootItem.licenseInformation.maxNumberOfIOPs) : ""

//        color: IngeScapeTheme.whiteColor
//        font {
//            family: IngeScapeTheme.textFontFamily
//            //weight: Font.Medium
//            pixelSize: 16
//        }
//    }

//    Text {
//        text: rootItem.licenseInformation ? qsTr("Agents:") : ""

//        color: IngeScapeTheme.whiteColor
//        font {
//            family: IngeScapeTheme.textFontFamily
//            weight: Font.Medium
//            pixelSize: 16
//        }
//    }

//    Repeater {
//        model: rootItem.licenseInformation ? rootItem.licenseInformation.agents : null

//        Text {
//            text: " - " + modelData

//            color: IngeScapeTheme.whiteColor
//            font {
//                family: IngeScapeTheme.textFontFamily
//                //weight: Font.Medium
//                pixelSize: 16
//            }
//        }
//    }
}
