import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import IngeScapePlayground 1.0



Item {
    id: root

    //-------------------------------------------
    //
    // Properties
    //
    //-------------------------------------------


    // Font
    property font font: Qt.font({family: "Helvetica", pixelSize: 16});


    // Source code
    property string sourceCode: "import QtQuick 2.9\nRectangle {\nanchors.fill:parent\ncolor: 'red'\n}"


    //-------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------


    // Throttle function used to avoid useless updates
    function throttle(timer, delay, callback) {
        var now = new Date().getTime();

        if (
                (timer.lastTime && (now < (timer.lastTime + delay)))
                ||
                !timer.running
                )
        {
            // Reset timer
            timer.stop();
            timer.callback = callback;
            timer.interval = delay;
            timer.lastTime = now;

            // Start timer
            timer.start();
        }
        else
        {
            timer.lastTime = now;
        }
    }



    //-------------------------------------------
    //
    // Behavior
    //
    //-------------------------------------------

    Component.onCompleted: {
        codeEditor.forceActiveFocus();
    }



    //-------------------------------------------
    //
    // Properties
    //
    //-------------------------------------------

    Item {
        id: content

        anchors.fill: parent


        // Right separator
        Rectangle {
            anchors {
                top: parent.top
                bottom: parent.bottom
                right: parent.right
            }

            width: 1

            color: "#989797"
        }


        // Line numbers and source code
        Row {
            id: sourceCodeView

            anchors {
                top: parent.top
                bottom: parent.bottom
            }

            // Scrollview containing line numbers
            ScrollView {
                id: scrollViewLineNumbers

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }

                width: scrollViewLineNumbersContent.width

                enabled: false

                horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
                verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff

                contentItem: Item {
                    id: scrollViewLineNumbersContent

                    implicitWidth: backgroundLineNumbers.width
                    implicitHeight: backgroundLineNumbers.height

                    // Column displaying the number of each line
                    Rectangle {
                        id: backgroundLineNumbers

                        width: 50

                        height: Math.max(columnLineNumbers.height, scrollViewLineNumbers.height)

                        color: "#f0f0f0"

                        // Highlight feedback for our current line
                        Rectangle {
                            id: currentLineHighlight

                            anchors {
                                left: parent.left
                                right: parent.right
                            }

                            height: codeEditor.cursorRectangle.height

                            y: codeEditor.cursorRectangle.y

                            color: "lightsteelblue"
                        }

                        // Line numbers
                        Column {
                            id: columnLineNumbers

                            anchors{
                                left: parent.left
                                right: parent.right
                            }

                            spacing: 0

                            Repeater {
                                model: codeEditor.lineCount

                                Text {
                                    anchors{
                                        left: parent.left
                                        right: parent.right
                                        rightMargin: 5
                                    }

                                    text: (index + 1)

                                    color: "#666666"

                                    font: root.font

                                    horizontalAlignment: TextEdit.AlignRight
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }
                }
            }



            // Scrollview containing our source code
            ScrollView {
                id: scrollViewCodeEditor

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }

                width: content.width -scrollViewLineNumbers.width - sourceCodeView.spacing

                // Size of scrollbars
                property int scrollbarSize: 15

                style: CodeScrollViewStyle {
                    scrollbarSize: scrollViewCodeEditor.scrollbarSize
                }


                contentItem: Item {
                    width: codeEditor.width
                    height: codeEditor.height

                    // Source code editor
                    TextEdit {
                        id: codeEditor

                        width: Math.max(scrollViewCodeEditor.width -scrollViewCodeEditor.scrollbarSize, contentWidth)

                        wrapMode: Text.NoWrap

                        font: root.font

                        selectByMouse: true

                        inputMethodHints: Qt.ImhNoPredictiveText

                        text: root.sourceCode

                        onTextChanged: {
                            throttle(timerCodeEditor, 500, function() {
                               root.sourceCode = text;
                            });
                        }
                    }


                    // To highlight keywords of our source code
                    QMLSyntaxHighlighter {
                        quickTextDocument: codeEditor.textDocument
                    }


                    // Timer used to update our source code
                    Timer {
                        id: timerCodeEditor

                        repeat: false
                        running: false

                        property var callback
                        property var lastTime

                        onTriggered: callback();
                    }
                }
            }
        }


        // Connect our two scrollviews
        Binding {
            target: scrollViewLineNumbers.flickableItem
            property: 'contentY'
            value: scrollViewCodeEditor.flickableItem.contentY
        }
    }
}
