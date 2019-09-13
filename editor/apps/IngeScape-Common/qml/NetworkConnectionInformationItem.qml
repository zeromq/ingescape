import QtQuick 2.8

import INGESCAPE 1.0

import I2Quick 1.0


Item {
    id: root

    //--------------------------------------------------------
    //
    // Properties
    //
    //--------------------------------------------------------

    // Flag indicating if we are online
    property bool isOnline: false

    // Current network device
    property var currentNetworkDevice: null

    // Current port
    property int currentPort: 31520

    // Duration of expand/collapse animations in milliseconds (250 ms => default duration of QML animations)
    property int expandCollapseAnimationDuration: 250



    //
    // Configure our item
    //
    width: (root.isOnline) ? 200 : 100
    height: (rootPrivate.isInEditionMode) ? 100 : 50



    //
    // Private properties
    //
    QtObject {
        id: rootPrivate

        // Flag indicating if we can perform expand/collapse animations
        property bool canPerformExpandCollapseAnimations: false

        // Flag indicating if we are in edition mode
        property bool isInEditionMode: false
    }


    //--------------------------------------------------------
    //
    // Behavior
    //
    //--------------------------------------------------------


    Behavior on width {
        enabled: rootPrivate.canPerformExpandCollapseAnimations

        NumberAnimation {
            duration: root.expandCollapseAnimationDuration
        }
    }


    Behavior on height {
        enabled: rootPrivate.canPerformExpandCollapseAnimations

        NumberAnimation {
            duration: root.expandCollapseAnimationDuration
        }
    }


    Component.onCompleted: {
        rootPrivate.canPerformExpandCollapseAnimations = true;
    }


    //--------------------------------------------------------
    //
    // Content
    //
    //--------------------------------------------------------


    I2CustomRectangle {
        id: background

        anchors {
            fill: parent
        }

        bottomLeftRadius: 5

        color: (root.isOnline) ? IngeScapeTheme.veryDarkGreyColor : "#8C1B1F"

        borderColor: (root.isOnline) ? IngeScapeTheme.editorsBackgroundBorderColor : "#FF1D25"
        borderWidth: 1

        fuzzyRadius: 8

        Behavior on color {
            ColorAnimation {
            }
        }

        Behavior on borderColor {
            ColorAnimation {
            }
        }


        Item {
            id: content

            anchors {
                fill: parent
                margins: 10
            }
        }

    }

}
