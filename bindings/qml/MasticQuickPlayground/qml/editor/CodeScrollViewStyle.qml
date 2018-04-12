import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4


ScrollViewStyle {
    id: root

    //--------------------------
    //
    // Custom properties
    //
    //--------------------------

    // Size (width or height) of our scrollbars
    property real scrollbarSize: 15


    //--------------------------------
    //
    // Configure our ScrollViewStyle
    //
    //---------------------------------

    // Desktop usage: we don't want to hide our scrollbars
    transientScrollBars: false


    // Background of our scrollbar
    scrollBarBackground: Item {
        implicitWidth: root.scrollbarSize
        implicitHeight: root.scrollbarSize

        clip: true
    }


    // Increment button of our scrollbars
    incrementControl: Item {
        implicitWidth:  1
        implicitHeight: 1
    }


    // Decrement button of our scrollbars
    decrementControl: Item {
        implicitWidth:  1
        implicitHeight: 1
    }


    // Corner between scrollbars
    corner: Item {
        implicitWidth:  1
        implicitHeight: 1
    }


    // Handle of scrollbars
    handle: Item {
        implicitWidth: root.scrollbarSize
        implicitHeight: root.scrollbarSize

        Rectangle {
            color: styleData.pressed ? "#878786" : "#989797";

            anchors {
                fill: parent
                topMargin: 1
                leftMargin: 2
                rightMargin: 0
                bottomMargin: 1
            }

            opacity : 0.8
            radius: root.scrollbarSize/2
        }

     }

}
