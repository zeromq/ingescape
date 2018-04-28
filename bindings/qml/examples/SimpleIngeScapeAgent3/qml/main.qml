import QtQuick 2.8
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Qt.labs.settings 1.0

import IngeScapeQuick 1.0


ApplicationWindow {
    id: root



    //----------------------------------
    //
    // Settings
    //
    //----------------------------------

    // IngeScape settings
    Settings {
        category: "IngeScape"

        // Network device and port used by our IngeScape agent
        property alias networkDevice: root.ingescapeNetworkDevice
        property alias port: root.ingescapePort
    }




    //----------------------------------
    //
    // Properties
    //
    //----------------------------------


    // Network device used to communicate via IngeScape
    property string ingescapeNetworkDevice: "en0"


    // Port used to communiate via IngeScape
    property int ingescapePort: 5670


    //
    // Configure our window
    //

    visible: true

    title: qsTr("%1 - v%2").arg(Qt.application.name).arg(Qt.application.version)

    minimumWidth: 320
    minimumHeight: 240

    width: 800
    height: 600


    //----------------------------------
    //
    // Menu
    //
    //----------------------------------

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")

            MenuItem {
                text: qsTr("E&xit")

                onTriggered: {
                    Qt.quit();
                }
            }
        }
    }



    //----------------------------------
    //
    // Behavior
    //
    //----------------------------------

    Component.onCompleted: {
        // IngeScapeQuick API: ingescape version
        console.log("IngeScape version is " + IngeScape.versionString);

        // IngeScapeQuick API: verbose mode
        IngeScape.isVerbose = true;

        // IngeScapeQuick API: set definition
        IngeScape.definitionName = Qt.application.name;
        IngeScape.definitionVersion = Qt.application.version;
        IngeScape.definitionDescription = "Definition of " + Qt.application.name;

        // IngeScapeQuick API: request outputs from mapped agents
        IngeScape.requestOutputsFromMappedAgents = true;

        // IngeScapeQuick API: list of inputs
        console.log("List of intputs:");
        var inputsList = IngeScape.inputsList;
        for(var indexI = 0; indexI < inputsList.length; indexI++)
        {
            var inputName = inputsList[indexI];
            console.log(
                        "  - input '"+ inputName
                        + "': type = " + IngeScapeIopType.enumToString( IngeScape.getTypeForInput(inputName) )
                        + ", value = " + IngeScape.inputs[inputName]
                        );
        }

        // IngeScapeQuick API: list of outputs
        console.log("List of outputs:");
        var outputsList = IngeScape.outputsList;
        for(var indexO = 0; indexO < outputsList.length; indexO++)
        {
            var outputName = outputsList[indexO];
            console.log(
                        "  - output '"+ outputName
                        + "': type = " + IngeScapeIopType.enumToString( IngeScape.getTypeForOutput(outputName) )
                        + ", value = " + IngeScape.outputs[outputName]
                        );
        }

        // IngeScapeQuick API: start our IngeScape agent
        console.log("Starting IngeScape on device " + root.ingescapeNetworkDevice + " with port " + root.ingescapePort);
        IngeScape.startWithDevice(root.ingescapeNetworkDevice, root.ingescapePort);
    }






    //----------------------------------
    //
    // Content
    //
    //----------------------------------


    Item {
        id: content

        anchors.fill: parent


        //
        // Text
        //
        Text {
            id: myText

            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                top: parent.verticalCenter
                margins: 10
            }

            text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed non risus. Suspendisse lectus tortor, dignissim sit amet, adipiscing nec, ultricies sed, dolor. Cras elementum ultrices diam. Maecenas ligula massa, varius a, semper congue, euismod non, mi. Proin porttitor, orci nec nonummy molestie, enim est eleifend mi, non fermentum diam nisl sit amet erat. Duis semper. Duis arcu massa, scelerisque vitae, consequat in, pretium a, enim. Pellentesque congue. Ut in risus volutpat libero pharetra tempor. Cras vestibulum bibendum augue. Praesent egestas leo in pede. Praesent blandit odio eu enim. Pellentesque sed dui ut augue blandit sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Aliquam nibh. Mauris ac mauris sed pede pellentesque fermentum. Maecenas adipiscing ante non diam sodales hendrerit"

            wrapMode: Text.WordWrap

            font {
                pixelSize: 12
            }

            // IngeScapeQuick API: bind all proprerties to IngeScape inputs
            IngeScapeInputBinding {
                target: myText

                properties: IngeScapeBinding.AllProperties

                inputsPrefix: "T_"
            }

            // IngeScapeQuick API: export all properties and signals as IngeScape outputs
            IngeScapeOutputBinding {
                target: myText

                properties: IngeScapeBinding.AllProperties
                signalHandlers: IngeScapeBinding.AllSignalHandlers

                outputsPrefix: "T_"
            }

            Component.onCompleted: console.log("text completed")
        }





        //
        // Custom item
        //
        MyItem {
            id: myItem

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: parent.verticalCenter
                margins: 10
            }


            // IngeScapeQuick API: bind our 'wheelRunning' property to a IngeScape input
            IngeScapeInputBinding on wheelRunning {
            }

            // IngeScapeQuick API: bind our 'wheelRunning' property to a IngeScape output
            IngeScapeOutputBinding on wheelRunning {
            }


            // IngeScapeQuick API: bind our 'wheelSource' property to a IngeScape input
            IngeScapeInputBinding on wheelSource {
            }
        }
    }
}
