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
        // Seamless background (bitmap version)
        //
        // Pros:
        //  - work with any tile pattern
        //  - lightweight (mostly GPU)
        //
        // Cons:
        //  - hard to perform a good looking zoom (scaling artifacts)
        //
        /*
        Image {
            id: seamlessBackgroundBitmap

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
        */


        //
        // Seamless background (vector version)
        //
        // Pros:
        //  - pan and zoom
        //
        // Cons:
        //  - grid pattern only (ah-hoc look)
        //  - heavier processing load (scale only because items are created/deleted)
        //
        Rectangle {
            id: seamlessBackgroundVector

            anchors.fill: parent

            color: "#282C34"

            SeamlessGrid {
                anchors.fill: parent

                offsetX: workspace.x
                offsetY: workspace.y

                cellSize: 150 * workspace.scale
                numberOfSubCells: 5
            }
        }




        //
        // Workspace background interaction: pan & zoom
        //
        PinchArea {
            anchors.fill: parent

            pinch.target: workspace
            pinch.minimumScale: 0.25
            pinch.maximumScale: 4

            MouseArea {
                anchors.fill: parent

                drag.target: workspace
            }



            //-----------------------------------------------
            //
            // Workspace: nodes and links will be added here
            //
            //-----------------------------------------------
            Item {
                id: workspace

                transformOrigin: Item.TopLeft

                width: parent.width
                height: parent.height



                //------------------------------------------------
                //
                // Pseudo content to test pan & zoom, DnD, links
                //
                //------------------------------------------------


                // Minimum length of control handle
                readonly property real controlHandleMinLength: 20;

                // Maximum Z-index
                property int maxZ: 0

                // Links
                property int linkWidth: 4
                property int linkFuzzyRadius: 4
                property color linkColor: "#ffffff"
                property color linkHoverColor: "firebrick"

                // Slots
                property color inletColor: "#0CB8FF"
                property color outletColor: "#ff9933"

                // Node
                property color nodeColor: "#17191F"
                property color nodeCollapsedColor: "darkkhaki"
                property color nodeBorderColor: "#939CAA"
                property color nodeSelectedBorderColor: "#ffffff"


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

                    firstPoint: Qt.point(item1.x + item1NodeOut.x + item1NodeOut.width/2, item1.y + item1NodeOut.y + item1NodeOut.height/2)

                    secondPoint: Qt.point(item3.x + item3NodeIn1.x + item3NodeIn1.width/2, item3.y + item3NodeIn1.y + item3NodeIn1.height/2)

                    onFirstPointChanged: updateControlPoints();
                    onSecondPointChanged: updateControlPoints();
                    Component.onCompleted: updateControlPoints();

                    stroke: (mouseAreaLink1.pressed) ? workspace.linkHoverColor : workspace.linkColor
                    strokeWidth: workspace.linkWidth

                    fuzzyColor: workspace.linkHoverColor
                    fuzzyRadius: (mouseAreaLink1.containsMouse) ? workspace.linkFuzzyRadius : 0

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

                    firstPoint: Qt.point(item2.x + item2NodeOut.x + item2NodeOut.width/2, item2.y + item2NodeOut.y + item2NodeOut.height/2)

                    secondPoint: Qt.point(item3.x + item3NodeIn2.x + item3NodeIn2.width/2, item3.y + item3NodeIn2.y + item3NodeIn2.height/2)

                    onFirstPointChanged: updateControlPoints();
                    onSecondPointChanged: updateControlPoints();
                    Component.onCompleted: updateControlPoints();

                    stroke: (mouseAreaLink2.pressed) ? workspace.linkHoverColor : workspace.linkColor
                    strokeWidth: workspace.linkWidth

                    fuzzyColor: workspace.linkHoverColor
                    fuzzyRadius: (mouseAreaLink2.containsMouse) ? workspace.linkFuzzyRadius : 0


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


                    firstPoint: Qt.point(item2.x + item2NodeOut.x + item2NodeOut.width/2, item2.y + item2NodeOut.y + item2NodeOut.height/2)

                    secondPoint: Qt.point(item4.x + item4NodeIn.x + item4NodeIn.width/2, item4.y + item4NodeIn.y + item4NodeIn.height/2)

                    onFirstPointChanged: updateControlPoints();
                    onSecondPointChanged: updateControlPoints();
                    Component.onCompleted: updateControlPoints();

                    stroke: (mouseAreaLink3.pressed) ? workspace.linkHoverColor : workspace.linkColor
                    strokeWidth: workspace.linkWidth

                    fuzzyColor: workspace.linkHoverColor
                    fuzzyRadius: (mouseAreaLink3.containsMouse) ? workspace.linkFuzzyRadius : 0


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


                    firstPoint: Qt.point(item3.x + item3NodeOut1.x + item3NodeOut1.width/2, item3.y + item3NodeOut1.y + item3NodeOut1.height/2)

                    secondPoint: Qt.point(item5.x + item5NodeIn1.x + item5NodeIn1.width/2, item5.y + item5NodeIn1.y + item5NodeIn1.height/2)

                    onFirstPointChanged: updateControlPoints();
                    onSecondPointChanged: updateControlPoints();
                    Component.onCompleted: updateControlPoints();

                    stroke: (mouseAreaLink4.pressed) ? workspace.linkHoverColor : workspace.linkColor
                    strokeWidth: workspace.linkWidth

                    fuzzyColor: workspace.linkHoverColor
                    fuzzyRadius: (mouseAreaLink4.containsMouse) ? workspace.linkFuzzyRadius : 0


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


                    firstPoint: Qt.point(item3.x + item3NodeOut2.x + item3NodeOut2.width/2, item3.y + item3NodeOut2.y + item3NodeOut2.height/2)

                    secondPoint: Qt.point(item5.x + item5NodeIn2.x + item5NodeIn2.width/2, item5.y + item5NodeIn2.y + item5NodeIn2.height/2)

                    onFirstPointChanged: updateControlPoints();
                    onSecondPointChanged: updateControlPoints();
                    Component.onCompleted: updateControlPoints();

                    stroke: (mouseAreaLink5.pressed) ? workspace.linkHoverColor : workspace.linkColor
                    strokeWidth: workspace.linkWidth

                    fuzzyColor: workspace.linkHoverColor
                    fuzzyRadius: (mouseAreaLink5.containsMouse) ? workspace.linkFuzzyRadius : 0


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

                    firstPoint: Qt.point(item4.x + item4NodeOut.x + item4NodeOut.width/2, item4.y + item4NodeOut.y + item4NodeOut.height/2)

                    secondPoint: Qt.point(item5.x + item5NodeIn2.x + item5NodeIn2.width/2, item5.y + item5NodeIn2.y + item5NodeIn2.height/2)

                    onFirstPointChanged: updateControlPoints();
                    onSecondPointChanged: updateControlPoints();
                    Component.onCompleted: updateControlPoints();

                    stroke: (mouseAreaLink6.pressed) ? workspace.linkHoverColor : workspace.linkColor
                    strokeWidth: workspace.linkWidth

                    fuzzyColor: workspace.linkHoverColor
                    fuzzyRadius: (mouseAreaLink6.containsMouse) ? workspace.linkFuzzyRadius : 0


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

                    color: workspace.nodeColor

                    radius: 8

                    border {
                        width: 2
                        color: (mouseArea1.pressed) ? workspace.nodeSelectedBorderColor : workspace.nodeBorderColor
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
                            color: workspace.outletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.outletColor
                        }
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

                    color: workspace.nodeColor

                    radius: 8

                    border {
                        width: 2
                        color: mouseArea2.pressed ? workspace.nodeSelectedBorderColor : workspace.nodeBorderColor
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
                            color: workspace.outletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.outletColor
                        }
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


                    color: (isClosed) ? workspace.nodeCollapsedColor : workspace.nodeColor

                    radius: 8

                    border {
                        width: 2
                        color: mouseArea3.pressed ? workspace.nodeSelectedBorderColor : workspace.nodeBorderColor
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
                            color: workspace.inletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.inletColor
                        }
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
                            color: workspace.inletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.inletColor
                        }
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
                            color: workspace.outletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.outletColor
                        }
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
                            color: workspace.outletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.outletColor
                        }
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


                    color: workspace.nodeColor

                    radius: 8

                    border {
                        width: 2
                        color: mouseArea4.pressed ? workspace.nodeSelectedBorderColor : workspace.nodeBorderColor
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
                            color: workspace.inletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.inletColor
                        }
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
                            color: workspace.outletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.outletColor
                        }
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

                    color: (isClosed) ? workspace.nodeCollapsedColor : workspace.nodeColor

                    radius: 8

                    border {
                        width: 2
                        color: mouseArea5.pressed ? workspace.nodeSelectedBorderColor : workspace.nodeBorderColor
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
                            color: workspace.inletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.inletColor
                        }
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
                            color: workspace.inletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.inletColor
                        }
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
