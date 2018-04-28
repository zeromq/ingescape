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
        // IngeScapeQuick API: ingescape version
        console.log("IngeScape version is " + IngeScape.version + " (" + IngeScape.versionString + ")");

        // IngeScapeQuick API: verbose mode
        IngeScape.isVerbose = false;

        // IngeScapeQuick API: set definition
        IngeScape.definitionName = Qt.application.name;
        IngeScape.definitionVersion = Qt.application.version;
        IngeScape.definitionDescription = "Definition of " + Qt.application.name;

        // IngeScapeQuick API: create inputs
        IngeScape.createInputDouble("circleCx", 20);
        IngeScape.createInputDouble("circleCy", 20);
        IngeScape.createInputString("canvasColor", canvas.color);
        IngeScape.createInputImpulsion("impulsion");


        // IngeScapeQuick API: create outputs
        IngeScape.createOutputDouble("x", 0);
        IngeScape.createOutputDouble("y", 0);

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
                            // IngeScapeQuick API: update a IngeScape ouput via javascript
                            IngeScape.outputs.x = value;
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

                    // IngeScapeQuick API: update a IngeScape output via a Binding item
                    Binding {
                        target: IngeScape.outputs
                        property: "y"
                        value: sliderY.value
                    }
                }
            }
        }


        //
        // Pseudo canvas
        //
        Rectangle {
            id: canvas

            clip: true

            anchors {
                left: parent.left
                right: parent.right
                top: controlsBackground.bottom
                bottom: parent.bottom
            }

            // IngeScapeQuick API: simple binding to subscribe to a IngeScape input
            color: IngeScape.inputs.canvasColor


            Behavior on color {
                ColorAnimation {}
            }


            MouseArea {
                id: canvasMouseArea

                anchors.fill: parent

                // IngeScapeQuick API: automatically bind QML signals to IngeScape outputs
                IngeScapeOutputBinding {
                    target: canvasMouseArea

                    signalHandlers: "onClicked"

                    outputsPrefix: "canvas_"
                }
            }


            Image {
                id: myImage

                property real opacityInPercent: 100.0

                opacity: Math.max(Math.min(opacityInPercent/100.0, 1), 0.0)

                source: "https://i2.wp.com/ingenuity.io/wp-content/uploads/2017/10/logo-I2-site-05.png"


                // IngeScapeQuick API: automatically bind a QML property to a IngeScape input
                // here, the 'source' property will be binded to a IngeScape input named 'imageSource'
                IngeScapeInputBinding on source {
                    inputName: "imageSource"
                }


                // IngeScapeQuick API: automatically bind a QML property to a IngeScape input
                // here, the 'rotation' property will be binded to a IngeScape input named 'imageRotationInDegrees'
                IngeScapeInputBinding on rotation {
                    inputName: "imageRotationInDegrees"
                }

                // IngeScapeQuick API: automatically bind a IngeScape output to a QML property
                // here, the 'opacityInPercent' property be binded to a IngeScape input named 'imageOpacityInPercent'
                IngeScapeInputBinding on opacityInPercent {
                    inputName: "imageOpacityInPercent"
                }



                // IngeScapeQuick API: automatically bind a IngeScape output to a QML property
                // here, the 'source' property will update the IngeScape output named 'imageSource'
                IngeScapeOutputBinding on source {
                    outputName: "imageSource"
                }

                // IngeScapeQuick API: automatically bind a IngeScape output to a QML property
                // here, the 'opacityInPercent' property will update the IngeScape output named 'imageOpacityInPercent'
                IngeScapeOutputBinding on opacityInPercent {
                    outputName: "imageOpacityInPercent"
                }
            }


            Item {
                id: myCircle

                // IngeScapeQuick API: simple bindings to subscribe to IngeScape inputs
                x: IngeScape.inputs.circleCx
                y: IngeScape.inputs.circleCy

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


            Rectangle {
                id: myRectangle

                x: 100
                y: 100
                width: 60
                height: 100

                border {
                    width: 3
                    color: "#CC000000"
                }

                color: "#CCFF0000"


                // IngeScapeQuick API: automatically bind a QML property to a IngeScape input
                // here, the 'x' property will be binded to a IngeScape input named 'rect_x'
                IngeScapeInputBinding on x {
                    inputName: "rect_x"
                }


                // IngeScapeQuick API: automatically bind a set of QML properties to a set of IngeScape inputs
                IngeScapeInputBinding {
                    target: myRectangle

                    properties: "y, width, height, color, border.color, border.width"
                    inputsPrefix: "rect_"
                }

                Component.onCompleted: {
                    console.log("Test to read value of rect_border.width = " + IngeScape.inputs["rect_border.width"]);
                }
            }
        }


        // IngeScapeQuick API: use a Connections item to subscribe to IngeScape input changes
        Connections {
            target: IngeScape.inputs

            onCanvasColorChanged: {
                console.log("IngeScape input canvasColor has changed - new value is " + IngeScape.inputs.canvasColor);
            }

            onCircleXChanged: {
                console.log("IngeScape input circleX has changed - new value is " + IngeScape.inputs.circleX);
            }

            onCircleYChanged: {
                console.log("IngeScape input circleY has changed - new value is " + IngeScape.inputs.circleY);
            }

            onImpulsionChanged: {
                myCirclePulseAnimation.start();
            }
        }


        // IngeScapeQuick API: subscribe to forceStop and observeOutput
        Connections {
            target: IngeScape

            onForcedStop: {
                console.log("Forced stop");
            }

            onObserveOutput: {
                console.log("IngeScape output " + name +" has changed - new value is " + value);
            }
        }
    }
}
