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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import INGESCAPE 1.0


//
// Drag-n-droppable listview item
// - It only works with vertical listviews (TODO horizontal listviews)
// - It MUST not have any anchors or a predefined size (width and height properties must not be set)
// - It MUST have a content
// - Its content MUST have a predefined size (width and height properties must be set) AND MUST not use anchors
//
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

    // The item that will become the parent of our draggable item during a drag operation
    property Item parentItemWhenDragged: _listView ? _listView.parent : null

    // Color of placeholders
    property color placeholderColor: "lightgrey" // IngeScapeTheme.editorsBackgroundColor

    // Radius of placeholders
    property real placeholderRadius : 0

    // Index of our item
    property int itemIndex: model.index


    // Set dimensions of our item
    width: contentItem.width
    height: topPlaceholder.height + wrapperParent.height + bottomPlaceholder.height


    //---------------------------------------
    //
    // Private properties
    //
    //---------------------------------------

    // Internal: set to -1 when drag-scrolling up and 1 when drag-scrolling down, 0 means no scrolling
    property int _scrollingDirection: 0

    // Internal: shortcut to access the attached ListView from everywhere.
    // Shorter than root.ListView.view
    property ListView _listView: ListView.view


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

        // TODO: define the minimum delay
        interval: 1

        onTriggered: {
            if (root._listView) {
                root._listView.positionViewAtIndex(root.itemIndex, ListView.Contain);
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

       height: (topDropAreaLoader.item && topDropAreaLoader.item.containsDrag && (root.itemIndex === 0))
               ? contentItem.height : 0

       radius: placeholderRadius

       color: root.placeholderColor

       // Animation only when user is dragging an item (not dropping)
       Behavior on height {
           enabled : (topDropAreaLoader.item && !topDropAreaLoader.item.containsDrag)
           NumberAnimation {}
       }
   }

    //
    // Default parent of our contentItem wrapper
    //
    Item {
        id: wrapperParent

        anchors {
            left: parent.left
            right: parent.right
            top: topPlaceholder.bottom
        }

        height: contentItem.height

        // Item used to wrap our contentItem. This item will be drag-n-dropped
        // and reparented during drag-n-drop
        Item {
            id: contentItemWrapper

            anchors.fill: parent

            // Configure drag
            Drag.keys: root.keysDragNDrop
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

                onReleased: {
                    forceActiveFocus();

                    // Check if a drag-n-drop is in progress
                    if (drag.active)
                    {
                        var dropIndex = -1;

                        // Get our current index
                        var currentIndex = root.itemIndex;

                        // Try to get our drop area
                        var dropArea = contentItemWrapper.Drag.target;
                        if (dropArea)
                        {
                            // Get our drop index
                            dropIndex = dropArea.dropIndex;
                        }
                        else
                        {
                            // No drop area, we will try to find an index

                            // Get oordinate of our drag hotspot
                            var y = contentItemWrapper.y + contentItem.height/2;

                            if (root._listView !== null)
                            {
                                // Check if we are above our listview
                                if (y <= root._listView.y)
                                {
                                    dropIndex = 0;
                                }
                                // Check if we are below our listview
                                else if (y >= (root._listView.y + root._listView.height))
                                {
                                    // NB: We assume that we drop our item at the last position of our listview
                                    dropIndex = root._listView.count;
                                }
                                // Check if we are below the last item of our listview
                                else if ((y - root._listView.y + root._listView.originY) >= root._listView.contentHeight)
                                {
                                    dropIndex = root._listView.count;
                                }
                                else
                                {
                                    // Try to find the item at this ordinate
                                    dropIndex = root._listView.indexAt(contentItem.width/2, y - root._listView.y + root._listView.contentY);
                                    if (dropIndex >= 0)
                                    {
                                        // Check if we must drop our item above or below
                                        var dropIndex2 = root._listView.indexAt(contentItem.width/2, contentItemWrapper.y - root._listView.y + root._listView.contentY);
                                        if (dropIndex === dropIndex2)
                                        {
                                            // Same item, we must drop our item below the first drop index
                                            dropIndex++;
                                        }
                                        // Else: different items, we will replace our first item
                                    }
                                }
                            }
                        }

                        // Check if we have a drop index
                        if (dropIndex >= 0)
                        {
                            // If our item is dropped below, we must correct the drop index
                            // to take into account the displacement of our item
                            // i.e. our item will leave its place
                            if (currentIndex < dropIndex)
                            {
                                dropIndex--;
                            }

                            // Check if our index has really changed
                            if (currentIndex !== dropIndex)
                            {
                                // Request an update of index
                                root.moveItemRequested(currentIndex, dropIndex);

                                // Scroll our list view to ensure that our item will be visible
                                makeDroppedItemVisibleTimer.start();
                            }
                            // Else: nothing to do
                        }
                        else
                        {
                            // No drop index, we don't know where to insert our item
                            console.log(root + " error: can not perform drop")
                        }
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

        radius: placeholderRadius

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
        active: root.itemIndex === 0
        enabled: active

        anchors {
            left: parent.left
            right: parent.right
            bottom: (dragArea.drag.active && topDropAreaLoader.active) ? wrapperParent.bottom : wrapperParent.verticalCenter
        }

        height: (root.itemIndex === 0)
                ? ((item && item.containsDrag) ? contentItem.height * 4 : contentItem.height)
                : 0

        sourceComponent: Component {
            DropArea {
                id: topDropArea

                keys: root.keysDragNDrop

                // NB: property used to know where a dragged item must be inserted
                property int dropIndex: 0
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

        height: (root._listView && (root.itemIndex === (root._listView.count - 1)))
                ? Math.max(root._listView.height - y, contentItem.height)
                : contentItem.height

        enabled: !dragArea.drag.active

        keys: root.keysDragNDrop

        // NB: property used to know where a dragged item must be inserted
        property int dropIndex: root.itemIndex + 1
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
                height:  (root.itemIndex === 0) ? Math.max(contentItem.height / 4, 5)  : 0
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

                // Ensure that we have enough space
                height: root._listView && (root.itemIndex === (root._listView.count - 1))
                        ? Math.max(root._listView.height - y, contentItem.height * 2)
                        : contentItem.height * 2
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
            to: "droppingBelow"

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
