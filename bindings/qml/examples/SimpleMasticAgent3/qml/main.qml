import QtQuick 2.8
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Qt.labs.settings 1.0

import MasticQuick 1.0


ApplicationWindow {
    id: root



    //----------------------------------
    //
    // Settings
    //
    //----------------------------------

    // Mastic settings
    Settings {
        category: "Mastic"

        // Network device and port used by our Mastic agent
        property alias networkDevice: root.masticNetworkDevice
        property alias port: root.masticPort
    }




    //----------------------------------
    //
    // Properties
    //
    //----------------------------------


    // Network device used to communicate via Mastic
    property string masticNetworkDevice: "en0"


    // Port used to communiate via Mastic
    property int masticPort: 5670


    //
    // Configure our window
    //

    visible: true

    title: qsTr("%1 - v%2").arg(Qt.application.name).arg(Qt.application.version)

    minimumWidth: 320
    minimumHeight: 240

    width: 800
    height: 600

    color: "white"

    // NB: we must override the style of our ApplicationWindow if we want a white background!
    // Because someone at Qt thinks that white means no background color and thus default system color
    // (i.e. light gray)
    // property QtObject styleData: QtObject {
    //    readonly property bool hasColor: window.color != "#ffffff"
    // }
    //
    style: ApplicationWindowStyle {
        background: Item {
        }
    }


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
        // MasticQuick API: mastic version
        console.log("Mastic version is " + Mastic.versionString);

        // MasticQuick API: verbose mode
        Mastic.isVerbose = true;

        // MasticQuick API: set definition
        Mastic.definitionName = Qt.application.name;
        Mastic.definitionVersion = Qt.application.version;
        Mastic.definitionDescription = "Definition of " + Qt.application.name;

        // MasticQuick API: request outputs from mapped agents
        Mastic.requestOutputsFromMappedAgents = true;


        // MasticQuick API: list of inputs
        console.log("List of intputs:");
        var inputsList = Mastic.inputsList;
        for(var indexI = 0; indexI < inputsList.length; indexI++)
        {
            var inputName = inputsList[indexI];
            console.log(
                        "  - input '"+ inputName
                        + "': type = " + MasticIopType.enumToString( Mastic.getTypeForInput(inputName) )
                        + ", value = " + Mastic.inputs[inputName]
                        );
        }

        // MasticQuick API: list of outputs
        console.log("List of outputs:");
        var outputsList = Mastic.outputsList;
        for(var indexO = 0; indexO < outputsList.length; indexO++)
        {
            var outputName = outputsList[indexO];
            console.log(
                        "  - output '"+ outputName
                        + "': type = " + MasticIopType.enumToString( Mastic.getTypeForOutput(outputName) )
                        + ", value = " + Mastic.outputs[outputName]
                        );
        }

        // MasticQuick API: start our Mastic agent
        console.log("Starting Mastic on device " + root.masticNetworkDevice + " with port " + root.masticPort);
        Mastic.startWithDevice(root.masticNetworkDevice, root.masticPort);
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

            // MasticQuick API: bind all proprerties to Mastic inputs
            MasticInputBinding {
                target: myText

                properties: MasticBinding.AllProperties

                inputsPrefix: "T_"
            }

            // MasticQuick API: export all properties and signals as Mastic outputs
            MasticOutputBinding {
                target: myText

                properties: MasticBinding.AllProperties
                signalHandlers: MasticBinding.AllSignalHandlers

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


            // MasticQuick API: bind our 'wheelRunning' property to a Mastic input
            MasticInputBinding on wheelRunning {
            }

            // MasticQuick API: bind our 'wheelRunning' property to a Mastic output
            MasticOutputBinding on wheelRunning {
            }


            // MasticQuick API: bind our 'wheelSource' property to a Mastic input
            MasticInputBinding on wheelSource {
            }
        }
    }
}
