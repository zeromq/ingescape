import QtQuick 2.9
import QtLocation 5.9
import QtPositioning 5.8
import IngeScapeQuick 1.0


Item {
    id: root

    anchors {
        fill: parent
    }


    // Behavior
    Component.onCompleted: {
        // Set agent name and definition info
        IngeScape.agentName = "SimpleMap-agent"
        IngeScape.definitionName = IngeScape.agentName;
        IngeScape.definitionDescription = "Definition of " + IngeScape.agentName;
        IngeScape.definitionVersion = "0.0";

        // Create inputs
        IngeScape.createInputDouble("deltaX");
        IngeScape.createInputDouble("deltaY");
        IngeScape.createInputDouble("deltaZoom");

        // Create outputs
        IngeScape.createOutputDouble("longitude", map.center.longitude);
        IngeScape.createOutputDouble("latitude", map.center.latitude);
        IngeScape.createOutputDouble("zoomLevel", map.zoomLevel);
    }


    // Subscribe to inputs
    Connections {
        target: IngeScape.inputs

        onDeltaXChanged: {
            updateMapCenter();

            repeat(function(){
                updateMapCenter();
            }, 50);
        }

        onDeltaYChanged: {
            updateMapCenter();

            repeat(function(){
                updateMapCenter();
            }, 50);
        }

        onDeltaZoomChanged: {
            zoomMap( IngeScape.inputs.deltaZoom );
        }
    }



    //----------------
    //
    // Methods
    //
    //----------------

    // Update our map
    function updateMapCenter()
    {
        var dx = IngeScape.inputs.deltaX;
        var dy = IngeScape.inputs.deltaY;

        dx = Math.min(1.0, Math.max(-1.0, dx));
        dy = Math.min(1.0, Math.max(-1.0, dy));

        var coeff = 20;
        var deltaPanX = (dx > 0 ? -1 : 1) * Math.pow(Math.abs(dx), 1.5) * coeff;
        var deltaPanY = (dy < 0 ? -1 : 1) * Math.pow(Math.abs(dy), 1.5) * coeff;

        panMap(deltaPanX, deltaPanY);

        if ((dx === 0) && (dy === 0))
        {
            repeatTimer.stop();
        }
    }


    // Pan map by (dx, dy)
    function panMap(dx, dy)
    {
        var mapCenterPointPixels = Qt.point(map.width/2.0 - dx, map.height/2.0 - dy);
        map.center = map.toCoordinate(mapCenterPointPixels);
    }


    // Zoom map
    function zoomMap(delta)
    {
        map.zoomLevel += delta;
    }


    Timer {
        id: repeatTimer

        repeat: true
        running: false

        property var callback
        property var last

        onTriggered: callback()
    }


    function repeat(callback, delay) {
        var now = new Date().getTime();
        if (
            (repeatTimer.last && (now < repeatTimer.last + delay ))
            ||
            !repeatTimer.running
            )
        {
            repeatTimer.stop();
            repeatTimer.callback = callback;
            repeatTimer.interval = delay;
            repeatTimer.last = now;
            repeatTimer.start();
        }
        else
        {
            repeatTimer.last = now;
        }
    }



    //----------------
    //
    // Content
    //
    //----------------

    // Map plugin
    Plugin {
        id: mapPlugin
        name: "osm"
    }

    // Map
    Map {
        id: map

        anchors {
            fill: parent
        }

        plugin: mapPlugin

        // Enable pan, flick, and pinch gestures to zoom in and out
        gesture.acceptedGestures: MapGestureArea.PanGesture | MapGestureArea.FlickGesture | MapGestureArea.PinchGesture | MapGestureArea.RotationGesture | MapGestureArea.TiltGesture
        gesture.flickDeceleration: 3000
        gesture.enabled: true


        center: poi1.coordinate
        zoomLevel: 10

        // Update outputs when our center changes
        onCenterChanged: {
            IngeScape.outputs.longitude = center.longitude;
            IngeScape.outputs.latitude = center.latitude;
        }

        // Update outputs when our zoom level changes
        onZoomLevelChanged: {
            IngeScape.outputs.zoomLevel = zoomLevel;
        }


        MapQuickItem {
            id: poi1

            coordinate {
                latitude: 43.5523623
                longitude: 1.4906598
            }

            sourceItem: Rectangle {
                width: 14
                height: width
                radius: width/2

                color: "black"

                border {
                    width: 2
                    color: "white"
                }

                smooth: true

                Text{
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        top: parent.bottom
                    }

                    text: "Ingenuity i/o"

                    color:"black"

                    font {
                        pixelSize: 16
                        bold: true
                    }

                    styleColor: "white"
                    style: Text.Outline
                }
            }

            anchorPoint: Qt.point(sourceItem.width/2, sourceItem.height/2)
        }
    }
}
