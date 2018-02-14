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
        console.log("Mastic version is " + Mastic.version);

        // MasticQuick API: verbose mode
        Mastic.isVerbose = true;


        // MasticQuick API: set definition
        Mastic.definitionName = Qt.application.name;
        Mastic.definitionVersion = Qt.application.version;
        Mastic.definitionDescription = "Definition of " + Qt.application.name;


        // MasticQuick API: create inputs
        Mastic.createInputDouble("circleCx", 300);
        Mastic.createInputDouble("circleCy", 200);

        // MasticQuick API: create outputs
        Mastic.createOutputDouble("rectX", myRectangle.x);
        Mastic.createOutputDouble("rectY", myRectangle.y);
        Mastic.createOutputString("currentColor", content.currentColor);
        Mastic.createOutputImpulsion("impulsion");
        Mastic.createOutputString("image", "https://pbs.twimg.com/profile_images/502064538108166144/ih48MCFK_400x400.png");

        // MasticQuick API: create parameters
        Mastic.createParameterString("myColor", "mediumaquamarine");


        // MasticQuick API: create a binding in javascript to update a Mastic output
        Mastic.outputs.currentColor = Qt.binding(function() { return content.currentColor;});


        //
        // MasticQuick API: check for existence and get type of an input
        //
        console.log("Input circleCx exists ? " + Mastic.checkInputExistence("circleCx"));
        var typeOfCircleCx = Mastic.getTypeForInput("circleCx");
        console.log("Type of input circleCx is: " + typeOfCircleCx + " == " + MasticIopType.enumToString(typeOfCircleCx));


        // MasticQuick API: list of inputs
        console.log("List of inputs: " + Mastic.inputsList);

        // MasticQuick API: list of outputs
        console.log("List of outputs:");
        var outputsList = Mastic.outputsList;
        for(var index = 0; index < outputsList.length; index++)
        {
            var outputName = outputsList[index];
            console.log(
                        " - Output "+ outputName
                        + ", type = " + MasticIopType.enumToString( Mastic.getTypeForOutput(outputName) )
                        + ", value = " + Mastic.outputs[outputName]
                        );
        }

        // MasticQuick API: list of parameters
         console.log("List of parameters: " + Mastic.parametersList);


        // MasticQuick API: get the list of network devices
        var networkDevices = Mastic.getNetdevicesList();
        console.log("Network devices: "+networkDevices);


        // MasticQuick API: log something
        Mastic.log(MasticLogLevel.LOG_TRACE, "Hello world\n");
        Mastic.info("Our agent will be started\n");


        // MasticQuick API: start our Mastic agent
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

        // Color of our current item
        property color currentColor: "white"



        // Controls
        Rectangle {
            id: controlsBackground

            anchors {
                top: parent.top
                topMargin: -controlsBackground.radius
                left: parent.left
                right: parent.right
            }

            height: controlsColumn.height + controlsColumn.y * 2

            radius: 10

            color: "lightgrey"

            border {
                width: 2
                color: "black"
            }

            Column {
                id: controlsColumn

                anchors {
                    left: parent.left
                    leftMargin: 10
                    right: parent.right
                    rightMargin: 10
                }

                y: 10

                spacing: 10

                Item {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: 50

                    Text {
                        id: currentColorLabel

                        anchors {
                            top: parent.top
                            bottom: parent.bottom
                        }

                        text: qsTr("Current color")

                        verticalAlignment: Text.AlignVCenter

                        font {
                            pixelSize: 14
                        }
                    }

                    Row {
                        anchors {
                            left: currentColorLabel.right
                            leftMargin: 20
                             verticalCenter: parent.verticalCenter
                        }

                        spacing: 20

                        Repeater {
                            model: ["white", "lightgreen", "burlywood", "palevioletred", "salmon", "darkorange", "deepskyblue"]

                            delegate: Rectangle {
                                width: 40
                                height: 40

                                radius: 5

                                color: modelData

                                border {
                                    width: Qt.colorEqual(content.currentColor, modelData) ? 4 : 0
                                    color: "black"
                                }

                                Behavior on border.width {
                                    NumberAnimation {}
                                }

                                MouseArea {
                                    anchors.fill: parent

                                    onClicked: {
                                        content.currentColor = modelData;
                                    }
                                }
                            }
                        }
                    }

                }


                Row {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                    }

                    height: childrenRect.height

                    spacing: 20

                    Button {
                        // MasticQuick API: check if our agent is muted
                        text: Mastic.isMuted ? qsTr("Unmute") : qsTr("Mute")

                        onClicked: {
                            // MastickQuick API: mute/unmute our agent
                            Mastic.isMuted = !Mastic.isMuted;
                        }
                    }

                    Button {
                        // MasticQuick API: check if our agent is verbose
                        text: Mastic.isVerbose ? qsTr("Disable verbose mode") : qsTr("Enable verbose mode")

                        onClicked: {
                            // MastickQuick API: switch verbose mode
                            Mastic.isVerbose = !Mastic.isVerbose;
                        }
                    }

                    Button {
                        text: qsTr("Send 'impulsion'")

                        onClicked: {
                            // MasticQuick API: update a Mastic output with type impulsion

                            // Option 1: set a new value each time
                            //Mastic.outputs.impulsion = new Date();

                            // Option 2: use writeOutputAsImpulsion
                            Mastic.writeOutputAsImpulsion("impulsion");
                        }
                    }
                }
            }
        }


        // Pseudo canvas
        Item {
            id: canvas

            anchors {
                left: parent.left
                right: parent.right
                top: controlsBackground.bottom
                bottom: parent.bottom
            }



            Rectangle {
                id: myRectangle

                x: 20
                y: 20

                width: 100
                height: 100
                radius: 10

                color: "mediumaquamarine"

                Text {
                    anchors.fill: parent

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    text: qsTr("Drag me")

                    color: "black"

                    wrapMode: Text.WordWrap

                    font {
                        pixelSize: 14
                    }
                }

                MouseArea {
                    anchors.fill: parent

                    drag.target: myRectangle
                }

                // MasticQuick API: update a Mastic output via a Binding item
                Binding {
                    target: Mastic.outputs
                    property: "rectX"
                    value: myRectangle.x
                }


                // MasticQuick API: update a Mastic output via a Binding item
                Binding {
                    target: Mastic.outputs
                    property: "rectY"
                    value: myRectangle.y
                }
            }


            Item {
                id: myCircle

                // MasticQuick API: simple bindings to subscribe to Mastic inputs
                x: Mastic.inputs.circleCx
                y: Mastic.inputs.circleCy

                Rectangle {
                    anchors.centerIn: parent

                    width: 50
                    height: width
                    radius: width/2

                    color: "tomato"

                    border {
                        width: 3
                        color: "black"
                    }
                }
            }
        }
    }
}
