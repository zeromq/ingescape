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
        // Seamless background
        //
        Image {
            id: seamlessBackground

            width: content.width + sourceSize.width
            height: content.height + sourceSize.height

            horizontalAlignment: Image.AlignLeft
            verticalAlignment: Image.AlignTop
            fillMode: Image.Tile

            // Source must be a tileable pattern
            source: "qrc:/resources/images/agentsmapping-background-tile.jpg"

            x: ((workspace.x % sourceSize.width) + sourceSize.width) % sourceSize.width - sourceSize.width
            y: ((workspace.y % sourceSize.height) + sourceSize.height) % sourceSize.height - sourceSize.height
        }



        //
        // Background interaction
        //
        MouseArea {
            anchors.fill: parent

            drag.target: workspace


            //
            // Workspace
            //
            Item {
                id: workspace

                width: parent.width
                height: parent.height

                // Minimum length of control handle
                readonly property real controlHandleMinLength: 20;

                // Maximum Z-index
                property int maxZ: 0


                //
                // Link between [item1, nodeOut] and [item3, nodein1]
                //
                I2CubicBezierCurve {
                    id: link1

                    function updateControlPoints()
                    {
                        var dx = secondPoint.x - firstPoint.x;
                        var dy = secondPoint.y - firstPoint.y;
                        var offsetX = Math.max(Math.abs(0.5 * dx), workspace.controlHandleMinLength);
                        var offsetY = Math.max(Math.abs(0.5 * dy), workspace.controlHandleMinLength);

                        if (dx > 0)
                        {
                            offsetY = 0;
                        }
                        else
                        {
                            offsetX = Math.max(offsetX, offsetY);

                            if (dy < 0)
                            {
                                offsetY = -offsetY;
                            }
                        }


                        firstControlPoint = Qt.point(firstPoint.x + offsetX, firstPoint.y + offsetY);
                        secondControlPoint = Qt.point(secondPoint.x - offsetX, secondPoint.y - offsetY );
                    }

                    strokeWidth: 6

                    firstPoint: Qt.point(item1.x + item1NodeOut.x + item1NodeOut.width/2, item1.y + item1NodeOut.y + item1NodeOut.height/2)

                    secondPoint: Qt.point(item3.x + item3NodeIn1.x + item3NodeIn1.width/2, item3.y + item3NodeIn1.y + item3NodeIn1.height/2)

                    onFirstPointChanged: updateControlPoints();
                    onSecondPointChanged: updateControlPoints();
                    Component.onCompleted: updateControlPoints();

                    stroke: (mouseAreaLink1.pressed) ? "firebrick" : "midnightblue"

                    fuzzyColor: "yellow"
                    fuzzyRadius: (mouseAreaLink1.containsMouse) ? 5 : 0

                    // Clip:true to clip our mousearea
                    clip: true

                    MouseArea {
                        id: mouseAreaLink1

                        anchors.fill: parent

                        hoverEnabled: true
                    }
                }


                //
                // Link between [item2, nodeOut] and [item3, nodein2]
                //
                I2CubicBezierCurve {
                    id: link2

                    function updateControlPoints()
                    {
                        var dx = secondPoint.x - firstPoint.x;
                        var dy = secondPoint.y - firstPoint.y;
                        var offsetX = Math.max(Math.abs(0.5 * dx), workspace.controlHandleMinLength);
                        var offsetY = Math.max(Math.abs(0.5 * dy), workspace.controlHandleMinLength);

                        if (dx > 0)
                        {
                            offsetY = 0;
                        }
                        else
                        {
                            offsetX = Math.max(offsetX, offsetY);

                            if (dy < 0)
                            {
                                offsetY = -offsetY;
                            }
                        }


                        firstControlPoint = Qt.point(firstPoint.x + offsetX, firstPoint.y + offsetY);
                        secondControlPoint = Qt.point(secondPoint.x - offsetX, secondPoint.y - offsetY );
                    }

                    strokeWidth: 6

                    firstPoint: Qt.point(item2.x + item2NodeOut.x + item2NodeOut.width/2, item2.y + item2NodeOut.y + item2NodeOut.height/2)

                    secondPoint: Qt.point(item3.x + item3NodeIn2.x + item3NodeIn2.width/2, item3.y + item3NodeIn2.y + item3NodeIn2.height/2)

                    onFirstPointChanged: updateControlPoints();
                    onSecondPointChanged: updateControlPoints();
                    Component.onCompleted: updateControlPoints();

                    stroke: (mouseAreaLink2.pressed) ? "firebrick" : "midnightblue"

                    fuzzyColor: "yellow"
                    fuzzyRadius: (mouseAreaLink2.containsMouse) ? 5 : 0

                    // Clip:true to clip our mousearea
                    clip: true

                    MouseArea {
                        id: mouseAreaLink2

                        anchors.fill: parent

                        hoverEnabled: true
                    }
                }


                //
                // Link between [item2, nodeOut] and [item4, nodeIn]
                //
                I2CubicBezierCurve {
                    id: link3

                    function updateControlPoints()
                    {
                        var dx = secondPoint.x - firstPoint.x;
                        var dy = secondPoint.y - firstPoint.y;
                        var offsetX = Math.max(Math.abs(0.5 * dx), workspace.controlHandleMinLength);
                        var offsetY = Math.max(Math.abs(0.5 * dy), workspace.controlHandleMinLength);

                        if (dx > 0)
                        {
                            offsetY = 0;
                        }
                        else
                        {
                            offsetX = Math.max(offsetX, offsetY);

                            if (dy < 0)
                            {
                                offsetY = -offsetY;
                            }
                        }


                        firstControlPoint = Qt.point(firstPoint.x + offsetX, firstPoint.y + offsetY);
                        secondControlPoint = Qt.point(secondPoint.x - offsetX, secondPoint.y - offsetY );
                    }

                    strokeWidth: 6

                    firstPoint: Qt.point(item2.x + item2NodeOut.x + item2NodeOut.width/2, item2.y + item2NodeOut.y + item2NodeOut.height/2)

                    secondPoint: Qt.point(item4.x + item4NodeIn.x + item4NodeIn.width/2, item4.y + item4NodeIn.y + item4NodeIn.height/2)

                    onFirstPointChanged: updateControlPoints();
                    onSecondPointChanged: updateControlPoints();
                    Component.onCompleted: updateControlPoints();

                    stroke: (mouseAreaLink3.pressed) ? "firebrick" : "midnightblue"

                    fuzzyColor: "yellow"
                    fuzzyRadius: (mouseAreaLink3.containsMouse) ? 5 : 0

                    // Clip:true to clip our mousearea
                    clip: true

                    MouseArea {
                        id: mouseAreaLink3

                        anchors.fill: parent

                        hoverEnabled: true
                    }
                }


                //
                // Link between [item3, nodeOut1] and [item5, nodeIn1]
                //
                I2CubicBezierCurve {
                    id: link4

                    function updateControlPoints()
                    {
                        var dx = secondPoint.x - firstPoint.x;
                        var dy = secondPoint.y - firstPoint.y;
                        var offsetX = Math.max(Math.abs(0.5 * dx), workspace.controlHandleMinLength);
                        var offsetY = Math.max(Math.abs(0.5 * dy), workspace.controlHandleMinLength);

                        if (dx > 0)
                        {
                            offsetY = 0;
                        }
                        else
                        {
                            offsetX = Math.max(offsetX, offsetY);

                            if (dy < 0)
                            {
                                offsetY = -offsetY;
                            }
                        }


                        firstControlPoint = Qt.point(firstPoint.x + offsetX, firstPoint.y + offsetY);
                        secondControlPoint = Qt.point(secondPoint.x - offsetX, secondPoint.y - offsetY );
                    }

                    strokeWidth: 6

                    firstPoint: Qt.point(item3.x + item3NodeOut1.x + item3NodeOut1.width/2, item3.y + item3NodeOut1.y + item3NodeOut1.height/2)

                    secondPoint: Qt.point(item5.x + item5NodeIn1.x + item5NodeIn1.width/2, item5.y + item5NodeIn1.y + item5NodeIn1.height/2)

                    onFirstPointChanged: updateControlPoints();
                    onSecondPointChanged: updateControlPoints();
                    Component.onCompleted: updateControlPoints();

                    stroke: (mouseAreaLink4.pressed) ? "firebrick" : "midnightblue"

                    fuzzyColor: "yellow"
                    fuzzyRadius: (mouseAreaLink4.containsMouse) ? 5 : 0

                    // Clip:true to clip our mousearea
                    clip: true

                    MouseArea {
                        id: mouseAreaLink4

                        anchors.fill: parent

                        hoverEnabled: true
                    }
                }


                //
                // Link between [item3, nodeOut2] and [item5, nodeIn2]
                //
                I2CubicBezierCurve {
                    id: link5

                    function updateControlPoints()
                    {
                        var dx = secondPoint.x - firstPoint.x;
                        var dy = secondPoint.y - firstPoint.y;
                        var offsetX = Math.max(Math.abs(0.5 * dx), workspace.controlHandleMinLength);
                        var offsetY = Math.max(Math.abs(0.5 * dy), workspace.controlHandleMinLength);

                        if (dx > 0)
                        {
                            offsetY = 0;
                        }
                        else
                        {
                            offsetX = Math.max(offsetX, offsetY);

                            if (dy < 0)
                            {
                                offsetY = -offsetY;
                            }
                        }


                        firstControlPoint = Qt.point(firstPoint.x + offsetX, firstPoint.y + offsetY);
                        secondControlPoint = Qt.point(secondPoint.x - offsetX, secondPoint.y - offsetY );
                    }

                    strokeWidth: 6

                    firstPoint: Qt.point(item3.x + item3NodeOut2.x + item3NodeOut2.width/2, item3.y + item3NodeOut2.y + item3NodeOut2.height/2)

                    secondPoint: Qt.point(item5.x + item5NodeIn2.x + item5NodeIn2.width/2, item5.y + item5NodeIn2.y + item5NodeIn2.height/2)

                    onFirstPointChanged: updateControlPoints();
                    onSecondPointChanged: updateControlPoints();
                    Component.onCompleted: updateControlPoints();

                    stroke: (mouseAreaLink5.pressed) ? "firebrick" : "midnightblue"

                    fuzzyColor: "yellow"
                    fuzzyRadius: (mouseAreaLink5.containsMouse) ? 5 : 0

                    // Clip:true to clip our mousearea
                    clip: true

                    MouseArea {
                        id: mouseAreaLink5

                        anchors.fill: parent

                        hoverEnabled: true
                    }
                }


                //
                // Link between [item4, nodeOut] and [item5, nodeIn2]
                //
                I2CubicBezierCurve {
                    id: link6

                    function updateControlPoints()
                    {
                        var dx = secondPoint.x - firstPoint.x;
                        var dy = secondPoint.y - firstPoint.y;
                        var offsetX = Math.max(Math.abs(0.5 * dx), workspace.controlHandleMinLength);
                        var offsetY = Math.max(Math.abs(0.5 * dy), workspace.controlHandleMinLength);

                        if (dx > 0)
                        {
                            offsetY = 0;
                        }
                        else
                        {
                            offsetX = Math.max(offsetX, offsetY);

                            if (dy < 0)
                            {
                                offsetY = -offsetY;
                            }
                        }


                        firstControlPoint = Qt.point(firstPoint.x + offsetX, firstPoint.y + offsetY);
                        secondControlPoint = Qt.point(secondPoint.x - offsetX, secondPoint.y - offsetY );
                    }

                    strokeWidth: 6

                    firstPoint: Qt.point(item4.x + item4NodeOut.x + item4NodeOut.width/2, item4.y + item4NodeOut.y + item4NodeOut.height/2)

                    secondPoint: Qt.point(item5.x + item5NodeIn2.x + item5NodeIn2.width/2, item5.y + item5NodeIn2.y + item5NodeIn2.height/2)

                    onFirstPointChanged: updateControlPoints();
                    onSecondPointChanged: updateControlPoints();
                    Component.onCompleted: updateControlPoints();

                    stroke: (mouseAreaLink6.pressed) ? "firebrick" : "midnightblue"

                    fuzzyColor: "yellow"
                    fuzzyRadius: (mouseAreaLink6.containsMouse) ? 5 : 0

                    // Clip:true to clip our mousearea
                    clip: true

                    MouseArea {
                        id: mouseAreaLink6

                        anchors.fill: parent

                        hoverEnabled: true
                    }
                }


                //
                // Item 1
                //
                Rectangle {
                    id: item1

                    x: 50
                    y: 50

                    width: 150
                    height: 70

                    radius: 25

                    border {
                        width: 2
                        color: mouseArea1.drag.active ? "green" : "black"
                    }

                    Rectangle {
                        id: item1NodeOut

                        anchors {
                            horizontalCenter: parent.right
                            verticalCenter: parent.verticalCenter
                        }

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: "black"
                            width: 2
                        }

                        color: "lightsteelblue"
                    }


                    MouseArea {
                        id: mouseArea1

                        anchors.fill: parent

                        drag.target: parent

                        onPressed: {
                            parent.z = workspace.maxZ++;
                        }
                    }
                }


                //
                // Item 2
                //
                Rectangle {
                    id: item2

                    x: 50
                    y: 300

                    width: 150
                    height: 70

                    radius: 25

                    border {
                        width: 2
                        color: mouseArea2.drag.active ? "green" : "black"
                    }

                    Rectangle {
                        id: item2NodeOut

                        anchors {
                            horizontalCenter: parent.right
                            verticalCenter: parent.verticalCenter
                        }

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: "black"
                            width: 2
                        }

                        color: "lightsteelblue"
                    }


                    MouseArea {
                        id: mouseArea2

                        anchors.fill: parent

                        drag.target: parent

                        onPressed: {
                            parent.z = workspace.maxZ++;
                        }
                    }
                }


                //
                // Item 3
                //
                Rectangle {
                    id: item3

                    // Flag indicating if our item is closed or not
                    property bool isClosed: false

                    x: 400
                    y: 150

                    width: 150
                    height: (isClosed) ? 75 : 300

                    radius: 25

                    color: (isClosed) ? "darkkhaki" : "white"


                    border {
                        width: 2
                        color: mouseArea3.drag.active ? "green" : "black"
                    }

                    Behavior on height {
                        NumberAnimation {}
                    }

                    Rectangle {
                        id: item3NodeIn1

                        anchors {
                            horizontalCenter: parent.left
                        }

                        y: (item3.isClosed) ? (parent.height/2 - height/2) : (parent.height/3 - height/2)

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: "black"
                            width: 2
                        }

                        color: (item3.isClosed) ? "darkgreen" : "lightgreen"
                    }


                    Rectangle {
                        id: item3NodeIn2

                        anchors {
                            horizontalCenter: parent.left
                        }

                        y: (item3.isClosed) ? (parent.height/2 - height/2) : (parent.height * 2/3 - height/2)

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: "black"
                            width: 2
                        }

                        color: (item3.isClosed) ? "darkgreen" : "lightgreen"
                    }


                    Rectangle {
                        id: item3NodeOut1

                        anchors {
                            horizontalCenter: parent.right
                        }

                        y: (item3.isClosed) ? (parent.height/2 - height/2) : (parent.height/3 - height/2)

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: "black"
                            width: 2
                        }

                        color: (item3.isClosed) ? "steelblue" : "lightsteelblue"
                    }


                    Rectangle {
                        id: item3NodeOut2

                        anchors {
                            horizontalCenter: parent.right
                        }

                        y: (item3.isClosed) ? (parent.height/2 - height/2) : (parent.height * 2/3 - height/2)

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: "black"
                            width: 2
                        }

                        color: (item3.isClosed) ? "steelblue" : "lightsteelblue"
                    }


                    MouseArea {
                        id: mouseArea3

                        anchors.fill: parent

                        drag.target: parent

                        onPressed: {
                            parent.z = workspace.maxZ++;
                        }

                        onDoubleClicked: {
                            item3.isClosed = !item3.isClosed;
                        }
                    }
                }


                //
                // Item 4
                //
                Rectangle {
                    id: item4

                    x: 400
                    y: 550

                    width: 150
                    height: 70

                    radius: 25

                    border {
                        width: 2
                        color: mouseArea4.drag.active ? "green" : "black"
                    }


                    Rectangle {
                        id: item4NodeIn

                        anchors {
                            verticalCenter: parent.verticalCenter
                            horizontalCenter: parent.left
                        }

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: "black"
                            width: 2
                        }

                        color: "lightgreen"
                    }

                    Rectangle {
                        id: item4NodeOut

                        anchors {
                            horizontalCenter: parent.right
                            verticalCenter: parent.verticalCenter
                        }

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: "black"
                            width: 2
                        }

                        color: "lightsteelblue"
                    }


                    MouseArea {
                        id: mouseArea4

                        anchors.fill: parent

                        drag.target: parent

                        onPressed: {
                            parent.z = workspace.maxZ++;
                        }
                    }
                }


                //
                // Item 5
                //
                Rectangle {
                    id: item5

                    // Flag indicating if our item is closed or not
                    property bool isClosed: false


                    x: 700
                    y: 250

                    width: 150
                    height: (isClosed) ? 75 : 150

                    radius: 25

                    color: (isClosed) ? "darkkhaki" : "white"

                    border {
                        width: 2
                        color: mouseArea5.drag.active ? "green" : "black"
                    }

                    Behavior on height {
                        NumberAnimation {}
                    }

                    Rectangle {
                        id: item5NodeIn1

                        anchors {
                            horizontalCenter: parent.left
                        }

                        y: (item5.isClosed) ? (parent.height/2 - height/2) : (parent.height/3 - height/2)

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: "black"
                            width: 2
                        }

                        color: (item5.isClosed) ? "darkgreen" : "lightgreen"
                    }


                    Rectangle {
                        id: item5NodeIn2

                        anchors {
                            horizontalCenter: parent.left
                        }

                        y: (item5.isClosed) ? (parent.height/2 - height/2) : (parent.height * 2/3 - height/2)

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: "black"
                            width: 2
                        }

                        color: (item5.isClosed) ? "darkgreen" : "lightgreen"
                    }

                    MouseArea {
                        id: mouseArea5

                        anchors.fill: parent

                        drag.target: parent

                        onPressed: {
                            parent.z = workspace.maxZ++;
                        }

                        onDoubleClicked: {
                            item5.isClosed = !item5.isClosed;
                        }
                    }
                }
            }
        }
    }
}
