/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Chloé Roumieu <roumieu@ingenuity.io>
 */


import QtQuick 2.0

import INGESCAPE 1.0

Item {
    id: root

    //---------------------------------------
    //
    // Properties
    //
    //---------------------------------------

    // Redefine our default property to add extra children to our 'contentItem' item
    // NB: We use the 'data' property instead of the 'children' property to allow any type
    //     of content and not only visual items (data is a list<Object> AND children is a list<Item>)
    default property alias contents: contentItem.data

    // Keys: the list of drag keys our DropArea will accept
    property alias keysDragNDrop: bottomDropArea.keys

    // Draggable item is active or not
    property bool dragEnable : true

    // Size of the area at the top and bottom of the list that will trigger autoscrolling
    property int scrollEdgeSize: 10

    // Internal: set to -1 when drag-scrolling up and 1 when drag-scrolling down
    property int _scrollingDirection: 0

    // Internal: shortcut to access the attached ListView from everywhere.
    // Shorter than root.ListView.view
    property ListView _listView: ListView.view

    // The item that will become the parent of our draggable item during a drag operation
    property Item parentItemWhenDragged: _listView ? _listView.parent : null

    // Color of placeholders
    property color placeholderColor: "lightgrey" // IngeScapeTheme.editorsBackgroundColor

    width: contentItem.width
    height: topPlaceholder.height + wrapperParent.height + bottomPlaceholder.height

    //---------------------------------------
    //
    // Signals
    //
    //---------------------------------------

    // Triggered to request an update of the list's elements
    signal moveItemRequested(int from, int to)


    //---------------------------------------
    //
    // Behavior
    //
    //---------------------------------------

    // Timer used to scroll our list view to ensure that our item is visible after a drop
    Timer {
        id: makeDroppedItemVisibleTimer

        interval: 0

        onTriggered: {
            if (root._listView) {
                root._listView.positionViewAtIndex(model.index, ListView.Contain);
            }
        }
    }

    // Animation to scroll up the list
    SmoothedAnimation {
        id: upAnimation

        target: root._listView

        property: "contentY"

        to: 0

        running: (root._scrollingDirection === -1)
    }

    // Animation to scroll down the list
    SmoothedAnimation {
        id: downAnimation

        target: root._listView

        property: "contentY"

        to: root._listView ? ((contentItem.height + _listView.spacing) * (root._listView.count) - _listView.height) : 0

        running: (root._scrollingDirection === 1)
    }

    //---------------------------------------
    //
    // Content
    //
    //---------------------------------------

    //
    // Top placeholder (to have an empty space above our item)
    //
    Rectangle {
       id: topPlaceholder
       anchors {
           left: parent.left
           right: parent.right
       }
       height: 0

       // Take the radius of the content to drag so that placeholder look at him
       radius: contents[0].radius ? contents[0].radius : 0

       color: root.placeholderColor
   }

    // Item used to wrap our contentItem. This item will be drag-n-dropped
    // and reparented during drag-n-drop
    Item {
        id: wrapperParent

        anchors {
            left: parent.left
            right: parent.right
            top: topPlaceholder.bottom
        }
        height: contentItem.height

        Item {
            id: contentItemWrapper

            anchors.fill: parent

            // Configure drag
            Drag.keys: root.keysDragNDrop
            Drag.source: wrapperParent
            Drag.active: dragArea.drag.active
            Drag.hotSpot {
                x: contentItem.width/2
                y: contentItem.height/2
            }

            MouseArea {
                id: dragArea
                anchors.fill: parent

                enabled: dragEnable && _listView.count > 1 // Allow it only if there is more than one item in the list

                drag.target: parent

                // NB: we disable smoothed drag to ensure that our item will remain under the mouse cursor (no latency)
                drag.smoothed: false

                // Constraints on Y axis to drag only in listview
                drag.minimumY: _listView ? _listView.y - 2 : 0 // 2 = small margin to be able to overtake listview
                drag.maximumY: {
                    if (root._listView) {
                        ((contentItem.height + _listView.spacing) * (root._listView.count- 1) > root._listView.height)
                                // If listview is full of item (all along its height) => drag the element all along the list
                                ? (_listView.y + _listView.height - contentItem.height + 2) // 2 = small margin to be able to overtake listview

                                // If not => drag the element all along number of elements
                                : ((contentItem.height + _listView.spacing) * (root._listView.count- 1) + 2); // 2 = small margin to be able to overtake listview
                     }
                    else {
                        0;
                    }
                }

                onReleased: {
                    forceActiveFocus();

                    // Check if a drag-n-drop is in progress
                    if (drag.active) {
                        var dropArea = contentItemWrapper.Drag.target;
                        if (!dropArea) {
                            return;
                        }
                        var dropIndex = dropArea.dropIndex;

                        // If the target item is below us, then decrement dropIndex because the target item is going to move up when
                        // our item leaves its place
                        if (model.index < dropIndex) {
                            dropIndex--;
                        }
                        if (model.index === dropIndex) {
                            return;
                        }
                        root.moveItemRequested(model.index, dropIndex);
                        makeDroppedItemVisibleTimer.start();
                    }
                }
            }

            // Item that will contain our real content
            // NB: our content must have a fixed width and a fixed height
            //     It MUST NOT rely on anchors
            Item {
                id: contentItem

                width: childrenRect.width
                height: childrenRect.height
            }
        }
    }

    //
    // Bottom placeholder (to have an empty space above our item)
    //
    Rectangle {
        id: bottomPlaceholder
        anchors {
            left: parent.left
            right: parent.right
            top: wrapperParent.bottom
        }
        height: 0

        // Take the radius of the content to drag so that placeholder look at him
        radius: contents[0].radius ? contents[0].radius : 0

        color: root.placeholderColor
    }


    //
    // Drop area above our item
    //
    // NB: topDropArea is defined after contentItemWrapperParent to ensure that it will be above any DropArea
    //     defined by our custom content
    //
    Loader {
        id: topDropAreaLoader

        // NB: we only need a top drop area for our first listview item.
        //     Otherwise, we have a conflict with the bottom drop area of the previous listview item
        active: model.index === 0
        enabled: active

        anchors {
            left: parent.left
            right: parent.right
            bottom: wrapperParent.verticalCenter
        }
        height: contentItem.height

        sourceComponent: Component {
            DropArea {
                keys: root.keysDragNDrop

                // NB: property used to know where a dragged item must be inserted
                property int dropIndex: 0

                enabled: !dragArea.drag.active
            }
        }
    }


    //
    // Drop area below our item
    //
    // NB: bottomDropArea is defined after contentItemWrapperParent to ensure that it will be above any DropArea
    //     defined by our custom content
    //
    DropArea {
        id: bottomDropArea
        anchors {
            left: parent.left
            right: parent.right
            top: wrapperParent.verticalCenter
        }

        property bool isLast: model.index === _listView.count - 1

        height: isLast ? _listView.contentHeight - y : contentItem.height

        keys: root.keysDragNDrop

        enabled: !dragArea.drag.active

        // NB: property used to know where a dragged item must be inserted
        property int dropIndex: model.index + 1
    }


    //---------------------------------------
    //
    // States
    //
    //---------------------------------------

    states: [
        // Our contentItem is being dragged
        State {
            name: "dragging"
            when: dragArea.drag.active

            // Update the parent of our contentItem wrapper to display it above our ListView
            ParentChange {
                target: contentItemWrapper
                parent: parentItemWhenDragged
            }

            // Update the dimensions of our content item wrapper
            // otherwise it will take the size of its new parent
            PropertyChanges {
                target: contentItemWrapper

                anchors.fill: undefined
                width: contentItem.width
                height: contentItem.height
            }

            // Resize the parent of our customItem wrapper to collapse it
            PropertyChanges {
                target: wrapperParent
                height: 0
            }

            PropertyChanges {
                target: wrapperParent
                focus: true
            }

            // Auto-scroll
            PropertyChanges {
                target: root
                _scrollingDirection: {
                    if ((root._listView) && (contentItem.height * root._listView.count > root._listView.height)) {
                        var yCoord = root._listView.mapFromItem(dragArea, 0, dragArea.mouseY).y;
                        if (yCoord < scrollEdgeSize) {
                            // Top edge: scroll up
                            return -1;
                        } else if (yCoord > _listView.height - scrollEdgeSize) {
                            // Bottom edge: scroll down
                            return 1;
                        } else {
                            // No scroll
                            return 0;
                        }
                    }
                    else {
                        // No listview or listview not full of items (all along its height) => no scroll
                        return 0;
                    }
                }
            }
        },


        // Our contentItem has been dropped below our item
        State {
            name: "droppingBelow"
            when: bottomDropArea.containsDrag

            PropertyChanges {
                target: bottomPlaceholder
                height: contentItem.height
            }

            PropertyChanges {
                target: bottomDropArea
                height: contentItem.height * 2
            }
        },

       // Our contentItem has been dropped above our item
        State {
            name: "droppingAbove"
            when: topDropAreaLoader.item.containsDrag

            PropertyChanges {
                target: topPlaceholder
                height: contentItem.height
            }

            PropertyChanges {
                target: topDropAreaLoader
                height: contentItem.height * 2
            }
        }
    ]


    //---------------------------------------
    //
    // Transitions
    //
    //---------------------------------------

    transitions: [
        // NB: We must use transitions to animate 'height' changes. Otherwise, behavior animations
        //     may be interrupted improperly by Qt
        Transition {
            from: "*"
            to: "droppingTop, droppingBelow"

            PropertyAnimation {
                target: topPlaceholder
                properties: "height"
            }

            PropertyAnimation {
                target: bottomPlaceholder
                properties: "height"
            }

            PropertyAnimation {
                target: wrapperParent
                properties: "height"
            }
        }
    ]
}
