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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import MASTIC 1.0

// agentsmapping sub-directory
import "agentsmapping" as AgentsMapping
import "theme" as Theme

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

    Item {
        id: content

        anchors.fill: parent


        //
        // Node graph view
        //
        AgentsMapping.NodeGraphView {
            id: nodeGraphView

            anchors.fill: parent

            controller: rootItem.controller
        }




        //
        // Mapping Activation
        //
        Button {
            id: activeMappingBtn

            anchors {
                bottom: parent.bottom
                left: parent.left
                leftMargin: 23
                bottomMargin: 20
            }

            checkable: true

            style: Theme.LabellessSvgButtonStyle {
                fileCache: MasticTheme.svgFileMASTIC

                pressedID: releasedID + "-pressed"
                releasedID: MasticEditorC.modelManager.isActivatedMapping? "mappingActivation-activated" : "mappingActivation"
                disabledID : releasedID
            }

            onClicked: {
                MasticEditorC.modelManager.isActivatedMapping = checked;
            }

            Binding {
                target : activeMappingBtn
                property : "checked"
                value : MasticEditorC.modelManager.isActivatedMapping
            }
        }

        Text {
            anchors {
                verticalCenter: activeMappingBtn.verticalCenter
                left : activeMappingBtn.right
                leftMargin: 15
            }

            visible : rootItem.controller && rootItem.controller.isEmptyMapping && !MasticEditorC.modelManager.isActivatedMapping
            text : "Click on the button to synchronize with the environement and get the currently active mapping.\nOr create/load a mapping and click on the button to apply it to the environment."

            color : MasticTheme.darkBlueGreyColor
            font {
                family : MasticTheme.textFontFamily
                weight : Font.Light
                pixelSize: 17
            }
        }



    }
}
