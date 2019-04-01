/*
 *	IngeScape Measuring
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

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

//import "theme" as Theme
import "popup" as Popup


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

    property ExperimentationsListController controller: null;


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

        color: "#EEAAAA"
    }

    Text {
        id: title

        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }

        text: "Experimentations"
    }

    Button {
        id: btnNewExpe

        anchors {
            top: title.bottom
            topMargin: 20
            horizontalCenter: parent.horizontalCenter
        }

        text: "Nouvelle Expé"

        onClicked: {
            createExperimentationPopup.open();
        }
    }

    ListView {

        anchors {
            top: btnNewExpe.bottom
            topMargin: 20
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        spacing: 10

        model: controller.allExperimentationsGroups

        delegate: Text {
            //height: 40
            //width: 200

            text: model.name
        }
    }


    Popup.CreateExperimentationPopup {
        id: createExperimentationPopup

        //anchors.centerIn: parent

        controller: rootItem.controller
    }
}
