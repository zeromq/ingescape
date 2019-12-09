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
 *      Chloé Roumieu    <roumieu@ingenuity.io>
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

Item {
    id: rootItem

    //--------------------------------------------------------
    //
    // Properties
    //
    //--------------------------------------------------------

    // Controller
    property LicenseInformationM licenseInformation: null

    // Subtitles style
    property string subtitlesFont : IngeScapeTheme.textFontFamily
    property real subtitlesCapitalization : Font.AllUppercase
    property real subtitlesWeight : Font.Medium
    property real subtitlesPixelSize : 18
    property color subtitlesColor : IngeScapeTheme.whiteColor

    // Properties style
    property string propertiesFont : IngeScapeTheme.textFontFamily
    property real propertiesWeight : Font.Medium
    property real propertiesPixelSize : 14
    property color propertiesNameColor : IngeScapeTheme.lightGreyColor
    property color propertiesValueColor : IngeScapeTheme.whiteColor

    // Simplify rendering of license information view
    // NB : Pass to false to have a more simplified view of license informations
    property bool simplifiedWiew: true


    height: childrenRect.height + childrenRect.y


    //--------------------------------------------------------
    //
    // Content
    //
    //--------------------------------------------------------

    //
    // Editor part of license informations
    //

    // Picto "Editor"
    SvgImage {
        id: editorPicto

        anchors {
            top : parent.top
            left: parent.left
        }

        height : visible ? editorPicto.svgHeight : parent.anchors.topMargin * 2 // x2 : to put editorText at right place
        width : visible ? editorPicto.svgWidth : 0

        visible: rootItem.simplifiedWiew

        svgElementId : rootItem.licenseInformation && rootItem.licenseInformation.editorLicenseValidity ? "license-editor-ok" : "license-editor-fail"
    }

    // Subtitle "Editor"
    Text {
        id : editorText

        anchors {
            verticalCenter: editorPicto.verticalCenter
            left: editorPicto.right
            leftMargin: editorPicto.visible ? 8 : 0
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

    // Editor informations
    Column {
        id : editorInfosColumn

        anchors {
            top: editorText.bottom
            topMargin: 8
            left: editorText.left
            right: parent.right
        }

        spacing: 8

        // OWNERS
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

        // EXPIRATION DATE
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


    //
    // Platform part of license informations
    //

    // Picto "Platform"
    SvgImage {
        id: platformPicto

        anchors {
            top : editorInfosColumn.bottom
            topMargin : 22
            right: editorPicto.right
        }

        height : visible ? platformPicto.svgHeight : 0
        width : visible ? platformPicto.svgWidth : 0

        visible: rootItem.simplifiedWiew

        svgElementId : rootItem.licenseInformation && rootItem.licenseInformation.ingescapeLicenseValidity ? "platform-ok" : "platform-fail"
    }

    // Subtitle "Platform"
    Text {
        id : platformText

        anchors {
            verticalCenter: platformPicto.verticalCenter
            left: platformPicto.right
            leftMargin: platformPicto.visible ? 8 : 0
        }

        text: rootItem.simplifiedWiew ? qsTr("Platform") : qsTr("License")

        color: rootItem.subtitlesColor

        font {
            family: rootItem.subtitlesFont
            capitalization: rootItem.subtitlesCapitalization
            weight : rootItem.subtitlesWeight
            pixelSize : rootItem.subtitlesPixelSize
        }
    }

    // Platform informations
    Column {
        id : platformInfosColumn

        anchors {
            top: platformText.bottom
            topMargin: 8
            left: platformText.left
            right: parent.right
        }

        spacing: 8

        // CUSTOMERS
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

        // ORDERS
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

        // EXPIRATION DATE
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

                text: qsTr("Editor expiration date: ")

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

        // MAX AGENTS (if view is simplify : !rootItem.simplifiedWiew)
        Item {
            id : agentsInfosSimplify

            anchors {
                left : parent.left
                right: parent.right
            }

            height: childrenRect.height

            visible: !rootItem.simplifiedWiew

            Text {
                id: agentsInfosSimplifyText

                anchors.left: agentsInfosSimplify.left

                text: qsTr("Max AGENTS: ")

                color: rootItem.propertiesNameColor

                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }
            }

            Text {
                anchors {
                    left: agentsInfosSimplifyText.right
                    right: parent.right
                }

                text: rootItem.licenseInformation ? rootItem.licenseInformation.maxNumberOfAgents.toString() : ""

                elide: Text.ElideRight

                color: rootItem.propertiesValueColor

                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }
            }
        }

        // MAX IOPS (if view is simplify : !rootItem.simplifiedWiew)
        Item {
            id : iopsInfosSimplify

            anchors {
                left : parent.left
                right: parent.right
            }

            height: childrenRect.height

            visible: !rootItem.simplifiedWiew

            Text {
                id: iopsInfosSimplifyText

                anchors.left: iopsInfosSimplify.left

                text: qsTr("Max IOP: ")

                color: rootItem.propertiesNameColor

                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }
            }

            Text {
                anchors {
                    left: iopsInfosSimplifyText.right
                    right: parent.right
                }

                text: rootItem.licenseInformation ? rootItem.licenseInformation.maxNumberOfIOPs.toString() : ""

                elide: Text.ElideRight

                color: rootItem.propertiesValueColor

                font {
                    family: rootItem.propertiesFont
                    weight : rootItem.propertiesWeight
                    pixelSize : rootItem.propertiesPixelSize
                }
            }
        }

        // MAX AGENTS AND IOPS (when view is not simplify : rootItem.simplifiedWiew)
        Row {
            anchors {
                left : parent.left
                right: parent.right
            }

            visible: rootItem.simplifiedWiew

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

                    text: rootItem.licenseInformation ? rootItem.licenseInformation.maxNumberOfIOPs.toString() : ""

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

        // FEATURES
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

        // AGENTS
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
}
