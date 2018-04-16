import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Qt.labs.settings 1.0


import MasticPlayground 1.0

import MasticQuick 1.0


Item {
    id: root


    //-------------------------------------------
    //
    // Properties
    //
    //-------------------------------------------

    // Font
    property font font: Qt.font({family: "Helvetica", pixelSize: 13, bold: true});


    height: 50



    //-------------------------------------------
    //
    // Methods
    //
    //-------------------------------------------

    // Auto-restart Mastic if needed
    function autoRestartMasticIfNeeded()
    {
        if (
            // We are allowed to restart Mastic
            MasticPlaygroundController.needToRestartMastic
            &&
            // We have values to restart Mastic
            ((currentNetworkDevice.text.length !== 0) && (currentPort.text.length !== 0))
            )
        {
            Mastic.startWithDevice(currentNetworkDevice.text, currentPort.text);
        }
    }



    //-------------------------------------------
    //
    // Content
    //
    //-------------------------------------------

    Rectangle {
        id: background

        anchors {
            fill: parent
        }

        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "#17616D"
            }
            GradientStop {
                position: 1.0
                color: "#0F3D3F"
            }
        }

        // Bottom separator
        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            height: 1

            color: "#303338"
        }


        //
        // Select view mode
        //
        Row {
            id: selectViewMode

            anchors {
                verticalCenter: parent.verticalCenter

                left: parent.left
                leftMargin: 5
            }

            spacing: 10

            ExclusiveGroup {
                id: viewModeGroup
            }

            Repeater {
                model: MasticPlaygroundController.viewModes

                delegate: RadioButton {
                    exclusiveGroup: viewModeGroup

                    text: modelData

                    checked: (modelData === MasticPlaygroundController.currentViewMode)

                    onCheckedChanged: {
                        if (checked)
                        {
                            MasticPlaygroundController.currentViewMode = modelData;
                        }
                    }

                    style: RadioButtonStyle {
                        label: Text {
                            anchors.verticalCenter: parent.verticalCenter

                            font: root.font

                            color: "#FFFFFF"

                            text: control.text
                        }
                    }
                }
            }
        }



        //
        // Separator
        //
        Rectangle {
            id: separator

            anchors {
                top: parent.top
                bottom: parent.bottom
                left: selectViewMode.right

                margins: 4
                leftMargin: 10
            }

            width: 2

            color: "#232528"
        }


        //
        // Mastic
        //
        Item {
            id: masticCommands

            anchors {
                top: parent.top
                bottom: parent.bottom
                left: separator.right
                leftMargin: 10
                right: parent.right
            }

            Row {
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }

                spacing: 10


                //
                // Network device
                //
                Row {
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                    }

                    spacing: 5

                    Text {
                        anchors.verticalCenter: parent.verticalCenter

                        text: qsTr("Network device")

                        font: root.font

                        color: "#FFFFFF"
                    }

                    TextField {
                        id: currentNetworkDevice

                        anchors.verticalCenter: parent.verticalCenter

                        width: 70

                        enabled: !(Mastic.isStarted)
                    }
                }


                //
                // Port
                //
                Row {
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                    }

                    spacing: 5

                    Text {
                        anchors.verticalCenter: parent.verticalCenter

                        text: qsTr("Port")

                        font: root.font

                        color: "#FFFFFF"
                    }

                    TextField {
                        id: currentPort

                        anchors.verticalCenter: parent.verticalCenter

                        width: 70

                        validator: IntValidator {bottom: 1; top: 65535;}

                        enabled: !(Mastic.isStarted)
                    }
                }


                //
                // Start / stop
                //
                Button {
                    anchors.verticalCenter: parent.verticalCenter

                    text: Mastic.isStarted ? qsTr("Stop") : qsTr("Start")

                    enabled: (
                              Mastic.isStarted
                              ||
                              ((currentNetworkDevice.text.length !== 0) && (currentPort.text.length !== 0))
                              )

                    onClicked: {
                        if (Mastic.isStarted)
                        {
                            Mastic.stop();
                        }
                        else
                        {
                            Mastic.startWithDevice(currentNetworkDevice.text, currentPort.text);
                        }
                    }
                }


                //
                // Auto-restart
                //
                CheckBox {
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTr("Auto\nrestart")

                    checked: MasticPlaygroundController.autoRestartMastic

                    onCheckedChanged: {
                        MasticPlaygroundController.autoRestartMastic = checked;
                    }

                    style: CheckBoxStyle {
                        label: Text {
                            text: control.text

                            font: root.font

                            color: "#FFFFFF"
                        }
                    }
                }


                //
                // Mute / unmute
                //
                Button {
                    anchors.verticalCenter: parent.verticalCenter

                    text: Mastic.isMuted ? qsTr("Unmute") : qsTr("Mute")

                    onClicked: {
                        Mastic.isMuted = !Mastic.isMuted;
                    }
                }
            }
        }
    }



    //-------------------------------------------
    //
    // Settings
    //
    //-------------------------------------------

    Settings {
        category: "Mastic"

        property alias networkDevice: currentNetworkDevice.text
        property alias port: currentPort.text
    }
}
