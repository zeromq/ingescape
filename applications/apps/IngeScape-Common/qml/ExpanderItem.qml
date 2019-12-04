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
 *      Chloé Roumieu    <roumieu@ingenuity.io>
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0
import INGESCAPE 1.0


Item {
    id: root

    height: childrenRect.height


    //---------------------------
    //
    // Properties
    //
    //---------------------------

    // Redefine our default property to add extra children to our 'popupContentRoot' item
    // NB: We use the 'data' property instead of the 'children' property to allow any type
    //     of content and not only visual items (data is a list<Object> AND children is a list<Item>)
    default property alias data: contentRoot.data

    // To know if our expander item is opened
    property bool checked: false

    // Height of our content when our expander is opened
    property int contentHeightWhenOpened: 100

    // Height of our header
    property alias headerHeight: header.height

    // Text of our header
    property alias headerText: headerLabel.text

    // Flag used to check if our isReduced property has changed since item completion
    // NB: this flag is used has a guard to prevent resize animations at startup
    property bool _canPerformResizeAnimations: false

    // Style of separators
    property color separatorsColor : IngeScapeTheme.lightGreyColor

    // Style of header
    property color headerCheckedColor: IngeScapeTheme.darkBlueGreyColor
    property alias headerTextColor: headerLabel.color

    // Left margin of header label
    property real leftMarginTextHeader: 23

    // To set picto before label in the header
    property alias pictoHeader : pictoHeader.svgElementId


    //---------------------------
    //
    // Behavior
    //
    //---------------------------

    onContentHeightWhenOpenedChanged: {
        if (root.checked)
        {
            root._canPerformResizeAnimations = false;
        }
    }

    onCheckedChanged: {
        root._canPerformResizeAnimations = true;
    }


    Component.onCompleted: {
        root._canPerformResizeAnimations = true;
    }


    //---------------------------
    //
    // Content
    //
    //---------------------------

    //
    // Header of our expander
    //
    Rectangle {
        id: header

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        height: 25

        border {
            color: root.separatorsColor
            width: (root.checked) ? 1 : 0 // Borders are visible only when item is opened
        }

        color:  (root.checked) ? root.headerCheckedColor : "transparent"

        // Bottom separator when borders are not visible
        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            height: 1

            color: root.separatorsColor

            visible: !root.checked
        }

        // Label
        Item {
            id: labelHeader

            anchors.fill: parent

            Item {
                id: containerPicto

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    right: headerLabel.left
                }

                SvgImage {
                    id: pictoHeader

                    anchors {
                        verticalCenter : containerPicto.verticalCenter
                        horizontalCenter : containerPicto.horizontalCenter
                    }

                    visible: svgElementId !== ""

                    svgElementId : ""
                }
            }

            Text {
                id: headerLabel

                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    left: parent.left
                    leftMargin: root.leftMarginTextHeader
                    rightMargin: 23
                }

                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 14
                }

                verticalAlignment: Text.AlignVCenter

                color: IngeScapeTheme.whiteColor
            }
        }

        // Icon
        SvgImage {
            id: collapseIndicator

            anchors {
                right: parent.right
                rightMargin: 14
                verticalCenter: parent.verticalCenter
            }

            svgElementId : "dropdown"

            rotation: root.checked ? 90 : 0

            // Animate rotation of our indicator
            Behavior on rotation {
                enabled: root._canPerformResizeAnimations
                NumberAnimation {}
            }
        }

        // Mouse area
        MouseArea {
            id: headerMouseArea

            anchors.fill: parent

            onClicked: {
                root.checked = !root.checked;
            }
        }
    }


    //
    // Content of our expander
    //
    Item {
        id: contentRoot

        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
        }

        height: (root.checked) ? root.contentHeightWhenOpened : 0

        clip: true

        // Hide our content when needed to avoid useless rendering operations
        visible: (height > 0)

        // Animate resize
        Behavior on height {
            enabled: root._canPerformResizeAnimations
            NumberAnimation {}
        }

        // Left separator
        Rectangle {
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }

            width: 1

            color: root.separatorsColor
        }

        // Right separator
        Rectangle {
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }

            width: 1

            color: root.separatorsColor
        }

        // Bottom separator
        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            height: 1

            color: root.separatorsColor
        }
    }
}
