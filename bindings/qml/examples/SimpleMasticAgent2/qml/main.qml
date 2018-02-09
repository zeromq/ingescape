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
        Mastic.createInputDouble("circleCx", 20);
        Mastic.createInputDouble("circleCy", 20);
        Mastic.createInputString("canvasColor", canvas.color);
        Mastic.createInputImpulsion("impulsion");

        // MasticQuick API: create outputs
        Mastic.createOutputDouble("x", 0);
        Mastic.createOutputDouble("y", 0);

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

                // Slider X
                Item {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: 40

                    Text {
                        id: sliderXvalue

                        anchors {
                            verticalCenter: parent.verticalCenter
                            right: parent.right
                        }

                        width: 70

                        text: "X = " + sliderX.value.toLocaleString(Qt.locale(), 'f', 2)

                        font {
                            pixelSize: 14
                        }
                    }

                    Slider {
                        id: sliderX

                        anchors {
                            left: parent.left
                            verticalCenter: parent.verticalCenter
                            right: sliderXvalue.left
                            rightMargin: 10
                        }

                        minimumValue: 0
                        maximumValue: 400

                        onValueChanged: {
                            // MasticQuick API: update a Mastic ouput via javascript
                            Mastic.outputs.x = value;
                        }
                    }
                }


                // Slider Y
                Item {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: 40

                    Text {
                        id: sliderYvalue

                        anchors {
                            verticalCenter: parent.verticalCenter
                            right: parent.right
                        }

                        width: sliderXvalue.width

                        text: "Y = " + sliderY.value.toLocaleString(Qt.locale(), 'f', 2)

                        font {
                            pixelSize: 14
                        }
                    }

                    Slider {
                        id: sliderY

                        anchors {
                            left: parent.left
                            verticalCenter: parent.verticalCenter
                            right: sliderYvalue.left
                            rightMargin: 10
                        }

                        minimumValue: 0
                        maximumValue: 400
                    }

                    // MasticQuick API: update a Mastic output via a Binding item
                    Binding {
                        target: Mastic.outputs
                        property: "y"
                        value: sliderY.value
                    }
                }
            }
        }


        // Pseudo canvas
        Rectangle {
            id: canvas

            anchors {
                left: parent.left
                right: parent.right
                top: controlsBackground.bottom
                bottom: parent.bottom
            }

            // MasticQuick API: simple binding to subscribe to a Mastic input
            color: Mastic.inputs.canvasColor

            Behavior on color {
                ColorAnimation {}
            }


            Item {
                id: myCircle

                // MasticQuick API: simple bindings to subscribe to Mastic inputs
                x: Mastic.inputs.circleCx
                y: Mastic.inputs.circleCy

                Rectangle {
                    id: myCircleUI

                    anchors.centerIn: parent

                    width: 40
                    height: width
                    radius: width/2

                    color: "gold"

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


        // MasticQuick API: use a Connections item to subscribe to Mastic input changes
        Connections {
            target: Mastic.inputs

            onCanvasColorChanged: {
                console.log("Input canvasColor has changed - new value is " + Mastic.inputs.canvasColor);
            }

            onCircleXChanged: {
                console.log("Input circleX has changed - new value is " + Mastic.inputs.circleX);
            }

            onCircleYChanged: {
                console.log("Input circleY has changed - new value is " + Mastic.inputs.circleY);
            }

            onImpulsionChanged: {
                console.log("Input impulsion has changed - new value is " + Mastic.inputs.impulsion);
                myCirclePulseAnimation.start();
            }
        }


        // MasticQuick API: subscribe to forceStop
        Connections {
            target: Mastic

            onForcedStop: {
                console.log("Forced stop");
            }
        }
    }
}
