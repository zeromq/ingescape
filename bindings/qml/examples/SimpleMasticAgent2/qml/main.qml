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
        console.log("Mastic version is " + Mastic.version + " (" + Mastic.versionString + ")");

        // MasticQuick API: verbose mode
        Mastic.isVerbose = false;

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

            // MasticQuick API: simple binding to subscribe to a Mastic input
            color: Mastic.inputs.canvasColor

            Behavior on color {
                ColorAnimation {}
            }


            Image {
                id: myImage

                property real opacityInPercent: 100.0

                opacity: Math.max(Math.min(opacityInPercent/100.0, 1), 0.0)

                source: "https://i2.wp.com/ingenuity.io/wp-content/uploads/2017/10/logo-I2-site-05.png"


                // MasticQuick API: automatically bind a QML property to a Mastic input
                // here, the 'source' property will be binded to a Mastic input named 'imageSource'
                MasticInputBinding on source {
                    inputName: "imageSource"
                }


                // MasticQuick API: automatically bind a QML property to a Mastic input
                // here, the 'rotation' property will be binded to a Mastic input named 'imageRotationInDegrees'
                MasticInputBinding on rotation {
                    inputName: "imageRotationInDegrees"
                }

                // MasticQuick API: automatically bind a Mastic output to a QML property
                // here, the 'opacityInPercent' property be binded to a Mastic input named 'imageOpacityInPercent'
                MasticInputBinding on opacityInPercent {
                    inputName: "imageOpacityInPercent"
                }



                // MasticQuick API: automatically bind a Mastic output to a QML property
                // here, the 'source' property will update the Mastic output named 'imageSource'
                MasticOutputBinding on source {
                    outputName: "imageSource"
                }

                // MasticQuick API: automatically bind a Mastic output to a QML property
                // here, the 'opacityInPercent' property will update the Mastic output named 'imageOpacityInPercent'
                MasticOutputBinding on opacityInPercent {
                    outputName: "imageOpacityInPercent"
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


                // MasticQuick API: automatically bind a QML property to a Mastic input
                // here, the 'x' property will be binded to a Mastic input named 'rect_x'
                MasticInputBinding on x {
                    inputName: "rect_x"
                }


                // MasticQuick API: automatically bind a set of QML properties to a set of Mastic inputs
                MasticInputBinding {
                    target: myRectangle

                    properties: "y, width, height, color, border.color, border.width"
                    inputsPrefix: "rect_"
                }

                Component.onCompleted: {
                    console.log("Test to read value of rect_border.width = " + Mastic.inputs["rect_border.width"]);
                }
            }
        }


        // MasticQuick API: use a Connections item to subscribe to Mastic input changes
        Connections {
            target: Mastic.inputs

            onCanvasColorChanged: {
                console.log("Mastic input canvasColor has changed - new value is " + Mastic.inputs.canvasColor);
            }

            onCircleXChanged: {
                console.log("Mastic input circleX has changed - new value is " + Mastic.inputs.circleX);
            }

            onCircleYChanged: {
                console.log("Mastic input circleY has changed - new value is " + Mastic.inputs.circleY);
            }

            onImpulsionChanged: {
                console.log("Mastic input impulsion has changed - new value is " + Mastic.inputs.impulsion);
                myCirclePulseAnimation.start();
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
    }
}
