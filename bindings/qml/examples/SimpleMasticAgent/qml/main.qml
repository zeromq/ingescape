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

    minimumWidth: 640
    minimumHeight: 480

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
        Mastic.isVerbose = false;


        // MasticQuick API: set definition
        Mastic.definitionName = Qt.application.name;
        Mastic.definitionVersion = Qt.application.version;
        Mastic.definitionDescription = "Definition of " + Qt.application.name;


        // MasticQuick API: create inputs
        Mastic.createInputDouble("circleCx", 300);
        Mastic.createInputDouble("circleCy", 200);
        Mastic.createInputImpulsion("impulsion");

        // MasticQuick API: create outputs
        Mastic.createOutputDouble("rectX", myRectangle.x);
        Mastic.createOutputDouble("rectY", myRectangle.y);
        Mastic.createOutputString("currentColor", content.currentColor);
        Mastic.createOutputString("currentImage", content.currentImage);
        Mastic.createOutputString("currentFont", content.currentFont);
        Mastic.createOutputImpulsion("impulsion");

        // MasticQuick API: create parameters
        Mastic.createParameterString("myColor", "mediumaquamarine");


        // MasticQuick API: create a binding in javascript to update Mastic outputs
        Mastic.outputs.currentColor = Qt.binding(function() { return content.currentColor;});
        Mastic.outputs.currentImage = Qt.binding(function() { return content.currentImage;});
        Mastic.outputs.currentFont = Qt.binding(function() { return content.currentFont;});


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

        // Current color
        property color currentColor: "white"

        // Current image
        property string currentImage: "https://pbs.twimg.com/profile_images/502064538108166144/ih48MCFK_400x400.png"

        // Current font
        property font currentFont: Qt.font({family: "Helvetica", pixelSize: 12});



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
                            model: ["white", "lightgreen", "burlywood", "palevioletred", "salmon", "darkorange", "deepskyblue", "black"]

                            delegate: Rectangle {
                                width: 40
                                height: 40

                                radius: 5

                                color: modelData

                                border {
                                    width: Qt.colorEqual(content.currentColor, modelData) ? 4 : 0
                                    color: Qt.colorEqual("black", modelData) ? "white" : "black"
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


                Item {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: 50

                    Text {
                        id: currentFontLabel

                        anchors {
                            top: parent.top
                            bottom: parent.bottom
                        }

                        text: qsTr("Current font")

                        verticalAlignment: Text.AlignVCenter

                        font {
                            pixelSize: 14
                        }
                    }

                    Row {
                        anchors {
                            left: currentFontLabel.right
                            leftMargin: 20
                             verticalCenter: parent.verticalCenter
                        }

                        spacing: 20

                        Repeater {
                            model: [
                                    Qt.font({family: "Helvetica", pixelSize: 12}),
                                    Qt.font({family: "Helvetica", pixelSize: 14}),
                                    Qt.font({family: "Helvetica", pixelSize: 24, bold: true}),
                                    Qt.font({family: "Wingdings", pixelSize: 18, bold: true})
                                    ]

                            delegate: Rectangle {
                                width: 120
                                height: 40

                                radius: 5

                                color: "white"

                                border {
                                    width: (content.currentFont === modelData) ? 4 : 0
                                    color: "black"
                                }

                                Behavior on border.width {
                                    NumberAnimation {}
                                }

                                Text {
                                    anchors {
                                        fill: parent
                                        margins: 4
                                    }

                                    text: modelData.family + "\n" +
                                          modelData.pixelSize + "px, bold:" + modelData.bold

                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter


                                    font {
                                        pixelSize: 12
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent

                                    onClicked: {
                                        content.currentFont = modelData;
                                    }
                                }
                            }
                        }
                    }

                }




                Item {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: 80

                    Text {
                        id: currentImageLabel

                        anchors {
                            top: parent.top
                            bottom: parent.bottom
                        }

                        text: qsTr("Current image")

                        verticalAlignment: Text.AlignVCenter

                        font {
                            pixelSize: 14
                        }
                    }

                    Row {
                        anchors {
                            left: currentImageLabel.right
                            leftMargin: 20
                             verticalCenter: parent.verticalCenter
                        }

                        spacing: 20

                        Repeater {
                            model: [
                                     "https://pbs.twimg.com/profile_images/502064538108166144/ih48MCFK_400x400.png",
                                     "https://i2.wp.com/ingenuity.io/wp-content/uploads/2017/10/logo-I2-site-05.png",
                                     "https://i0.wp.com/ingenuity.io/wp-content/uploads/2017/08/offre2-01-1.png"
                                   ]

                            delegate: Rectangle {
                                width: 90
                                height: 70

                                Image {
                                    id: image

                                    anchors {
                                        fill: parent
                                        margins: 4
                                    }

                                    fillMode: Image.PreserveAspectFit
                                    horizontalAlignment: Image.AlignHCenter
                                    verticalAlignment: Image.AlignVCenter

                                    source: modelData
                                }

                                Rectangle {
                                    anchors.fill: parent

                                    color: "transparent"

                                    border {
                                        width: (content.currentImage === modelData) ? 4 : 1
                                        color: "black"
                                    }

                                    Behavior on border.width {
                                        NumberAnimation {}
                                    }

                                    MouseArea {
                                        anchors.fill: parent

                                        onClicked: {
                                            content.currentImage = modelData;
                                        }
                                    }
                                }
                            }
                        }
                    }

                }

                Rectangle {
                    anchors {
                        left: parent.left
                        right: parent.right
                        leftMargin: 10
                        rightMargin: 10
                    }

                    height: 2

                    color: "black"
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
                        // MasticQuick API: check if our agent logs in a file
                        text: Mastic.logInFile ? qsTr("Don't log in file") : qsTr("Log in file")

                        onClicked: {
                            // MastickQuick API: switch our logInFile flag
                            Mastic.logInFile = !Mastic.logInFile;
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

                    Button {
                        id: buttonPressMe

                        text: qsTr("Press me")

                        // MasticQuick API: export our 'pressed' property
                        MasticOutputBinding {
                            target: buttonPressMe

                            properties: "pressed"

                            outputsPrefix: "button_"
                        }
                    }
                }
            }
        }


        //
        // Pseudo canvas
        //
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
                    id: myCircleUI

                    anchors.centerIn: parent

                    width: 50
                    height: width
                    radius: width/2

                    color: "tomato"

                    border {
                        width: 3
                        color: "black"
                    }

                    SequentialAnimation {
                        id: myCirclePulseAnimation

                        running: false

                        NumberAnimation { target: myCircleUI; property: "scale"; to: 3; }
                        NumberAnimation { target: myCircleUI; property: "scale"; to: 1; }
                     }
                }
            }
        }


        // MasticQuick API: subscribe to forceStop and observeOutput
        Connections {
            target: Mastic

            onForcedStop: {
                console.log("Forced stop");
            }

            onObserveOutput: {
                console.log("Mastic output " + name +" has changed - new value is " + value);
            }
        }


        // MasticQuick API: use a Connections item to subscribe to Mastic input changes
        Connections {
            target: Mastic.inputs

            onImpulsionChanged: {
                myCirclePulseAnimation.start();
            }
        }
    }
}
