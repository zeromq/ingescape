import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0
import QtQuick.Window 2.3

import INGESCAPE 1.0

Item {
    id: rootItem

    anchors.fill: parent


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------



    //--------------------------------------------------------
    //
    //
    // Functions
    //
    //
    //--------------------------------------------------------


    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------


    //
    // Center panel
    //
    Item {
        id: centerPanel

        anchors {
            left: leftPanel.right
            right: parent.right
            top: parent.top
            bottom: bottomPanel.top
        }


    }


    //
    // Bottom panel
    //
    Rectangle {
        id: bottomPanel

        anchors {
            left: leftPanel.right
            right: parent.right
            bottom: parent.bottom
        }
        height: 0
        color: "red"
    }


    //
    // Left panel
    //
    Rectangle {
        id: leftPanel

        width: IngeScapeTheme.leftPanelWidth

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        color: IngeScapeTheme.agentsMappingBackgroundColor

        I2CustomRectangle {
            anchors {
                fill: parent
                topMargin: 9
            }
            color: IngeScapeTheme.leftPanelBackgroundColor

            fuzzyRadius: 8
            topRightRadius : 5

            borderWidth: 1
            borderColor: IngeScapeTheme.selectedTabsBackgroundColor

        }
    }



    //
    // Network Configuration (Popup)
    //
    /*NetworkConfiguration {
        id: networkConfigurationPopup

        anchors.centerIn: parent
    }*/


}
