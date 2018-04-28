import QtQuick 2.9


Item {
    id: root

    width: 1024
    height: 768


    //----------------------------
    //
    // Properties
    //
    //----------------------------

    // Foreground color
    property color foregroundColor: "#CCFFFFFF"


    // Flag indicating if our loading feedback is running or not
    property bool isRunning: root.visible



    //----------------------------
    //
    // Content
    //
    //----------------------------

    Item {
        id: container

        anchors {
            fill: parent
        }

        property alias _realContentWidth: content.width
        property alias _realContentHeight: content.height

        // Required scale factor
        readonly property real _scaleFactor: Math.min(width/_realContentWidth, height/_realContentHeight)

        // Required offset
        readonly property real _dx: (width - _realContentWidth * _scaleFactor)/2
        readonly property real _dy: (height - _realContentHeight * _scaleFactor)/2


        Item {
            id: content

            width: gears.width
            height: 720

            transformOrigin: Item.TopLeft

            x: container._dx
            y: container._dy

            scale: container._scaleFactor

            Item {
                id: gears

                width: 500
                height: 500
                y: 110

                Image {
                    id: gear1

                    x: -80
                    y: 160
                    width: 300
                    height: 300

                    source: "qrc:/resources/images/gear1.png"

                    transformOrigin: Item.Center

                    // NB: We must use an Animator instead of an Animation to perform
                    //     our animation even when the UI thread is blocked
                    RotationAnimator {
                        target: gear1
                        running: root.isRunning
                        from: 0
                        to: -360
                        duration: 4500
                        loops: Animation.Infinite
                    }
                }


                Image {
                    id: gear2

                    x: 104
                    y: 26
                    width: 178
                    height: 178

                    source: "qrc:/resources/images/gear2.png"

                    transformOrigin: Item.Center

                    // NB: We must use an Animator instead of an Animation to perform
                    //     our animation even when the UI thread is blocked
                    RotationAnimator {
                        target: gear2
                        running: root.isRunning
                        from: 0
                        to: 360
                        duration: 2500
                        loops: Animation.Infinite
                    }
                }


                Image {
                    id: gear3

                    x: 259
                    y: 85
                    width: 210
                    height: 210

                    source: "qrc:/resources/images/gear3.png"

                    transformOrigin: Item.Center

                    // NB: We must use an Animator instead of an Animation to perform
                    //     our animation even when the UI thread is blocked
                    RotationAnimator {
                        target: gear3
                        running: root.isRunning
                        from: 0
                        to: -360
                        duration: 3000
                        loops: Animation.Infinite
                    }
                }


                Image {
                    id: foreground

                    anchors.fill: parent

                    source: "qrc:/resources/images/hole.png"
                }
            }

            Rectangle {
                id: topPart

                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    bottom: gears.top
                }

                color: root.foregroundColor
            }

            Rectangle {
                id: bottomPart

                anchors {
                    left: parent.left
                    right: parent.right
                    top: gears.bottom
                    bottom: parent.bottom
                }

                color: root.foregroundColor
            }


            // Top text
            Text {
                anchors {
                    fill: topPart
                    margins: 0
                }

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: qsTr("ingeScape\nQML Playground")

                color: "#005EB8"

                font {
                    bold: true
                    pixelSize: 48
                }
            }


            // Bottom text
            Text {
                anchors {
                    fill: bottomPart
                    margins: 20
                }

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: qsTr("Loading... please wait")

                color: "#005EB8"

                font {
                    bold: true
                    pixelSize: 36
                }
            }
        }
    }



    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        height: container._dy

        color: root.foregroundColor
    }

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        height: container._dy

        color: root.foregroundColor
    }

    Rectangle {
        anchors {
            top: parent.top
            topMargin: root.padding
            bottom: parent.bottom
            bottomMargin: root.padding
            left: parent.left
        }

        width: container._dx

        color: root.foregroundColor
    }

    Rectangle {
        anchors {
            top: parent.top
            topMargin: root.padding
            bottom: parent.bottom
            bottomMargin: root.padding
            right: parent.right
        }

        width: container._dx

        color: root.foregroundColor
    }


}

