import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.0 as Controls2

import INGESCAPE 1.0

import I2Quick 1.0


I2CustomRectangle {
    id: root

    //--------------------------------------------------------
    //
    // Properties
    //
    //--------------------------------------------------------

    // Flag indicating if our edition mode is opened
    readonly property alias isEditionModeOpened: rootPrivate.isEditionModeOpened

    // Flag indicating if we are online
    property bool isOnline: false

    // Current network device
    property string currentNetworkDevice: "null"

    // Current port
    property int currentPort: 31520

    // Duration of animations in milliseconds (250 ms => default duration of QML animations)
    property int animationDuration: 250

    // List of network devices
    property alias listOfNetworkDevices: selectNetworkDeviceCombobox.model

    // Selected index of our list of network devices
    property alias listOfNetworkDevicesSelectedIndex: selectNetworkDeviceCombobox.selectedIndex;



    //
    // Configure our item
    //
    width: childrenRect.x * 2 + childrenRect.width
    height: childrenRect.y * 2 + childrenRect.height

    bottomLeftRadius: 5

    color: (root.isOnline) ? IngeScapeTheme.veryDarkGreyColor : "#8C1B1F"

    borderColor: (root.isOnline) ? IngeScapeTheme.editorsBackgroundBorderColor : "#FF1D25"
    borderWidth: 1

    fuzzyRadius: 8



    //
    // Private properties
    //
    QtObject {
        id: rootPrivate

        // Flag indicating if we can perform animations
        property bool canPerformAnimations: false

        // Flag indicating if our edition mode is opened
        property bool isEditionModeOpened: false
    }



    //--------------------------------------------------------
    //
    // Signals
    //
    //--------------------------------------------------------


    signal changeNetworkSettings(string networkDevice, int port, bool clearPlatform);



    //--------------------------------------------------------
    //
    // Methods
    //
    //--------------------------------------------------------

    // Open our widget
    function open()
    {
        rootPrivate.isEditionModeOpened = true;
    }


    // Close our widget
    function close()
    {
        rootPrivate.isEditionModeOpened = false;
    }



    //--------------------------------------------------------
    //
    // Behavior
    //
    //--------------------------------------------------------

    Behavior on color {
        enabled: rootPrivate.canPerformAnimations

        ColorAnimation {
            duration: root.animationDuration
        }
    }


    Behavior on borderColor {
        enabled: rootPrivate.canPerformAnimations

        ColorAnimation {
            duration: root.animationDuration
        }
    }


    Component.onCompleted: {
        rootPrivate.canPerformAnimations = true;
    }




    //--------------------------------------------------------
    //
    // Content
    //
    //--------------------------------------------------------


    Column {
        id: content

        anchors {
            top: parent.top
            topMargin: 15
            left: parent.left
            leftMargin: 15
        }

        width: (rootPrivate.isEditionModeOpened)
               ? 300
               : Math.min(textConnectionInfo.implicitWidth + textConnectionInfo.anchors.rightMargin + buttons.width, 300)


        Behavior on width {
            enabled: rootPrivate.canPerformAnimations

            NumberAnimation {
                duration: root.animationDuration
            }
        }


        // Header
        Item {
            id: header

            anchors {
                left: parent.left
                right: parent.right
            }

            height: childrenRect.y + childrenRect.height


            // Buttons
            Item {
                id: buttons

                anchors {
                    right: parent.right
                }

                width: childrenRect.width
                height: childrenRect.height


                // Button - Edit network settings
                LabellessSvgButton {
                    id: buttonOpenSettings

                    releasedID: "settings"
                    pressedID: "settings-pressed"
                    disabledID: "settings-disabled"

                    visible: !rootPrivate.isEditionModeOpened
                    enabled: visible

                    onClicked: {
                        root.open();
                    }

                    Controls2.ToolTip {
                        delay: Qt.styleHints.mousePressAndHoldInterval
                        visible: buttonOpenSettings.enabled && buttonOpenSettings.__behavior.containsMouse
                        text: qsTr("Edit network settings")
                    }
                }


                // Button - Quit edit network settings
                LabellessSvgButton {
                    id: buttonCloseSettings

                    releasedID: "closeEditor"
                    pressedID: "closeEditor-pressed"

                    visible: rootPrivate.isEditionModeOpened
                    enabled: visible

                    onClicked: {
                        root.close();
                    }

                    Controls2.ToolTip {
                        delay: Qt.styleHints.mousePressAndHoldInterval
                        visible: buttonCloseSettings.enabled && buttonCloseSettings.__behavior.containsMouse
                        text: qsTr("Close network settings")
                    }
                }
            }


            // Infos
            Text {
                id: textConnectionInfo

                anchors {
                    left: parent.left
                    right: buttons.left
                    rightMargin: 20
                }

                color: IngeScapeTheme.whiteColor

                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 16
                }

                elide: Text.ElideRight

                text: (root.isOnline) ? qsTr("Connected on %1\nwith port %2").arg(root.currentNetworkDevice).arg(root.currentPort)
                                      : qsTr("Offline")

                Behavior on height {
                    enabled: rootPrivate.canPerformAnimations

                    NumberAnimation {
                        duration: root.animationDuration
                    }
                }
            }
        }


        // Edit network settings
        Item {
            id: editNetworkSettings

            anchors {
                left: parent.left
                right: parent.right
            }

            height: (rootPrivate.isEditionModeOpened) ? editNetworkSettingsColumn.height : 0

            opacity: (rootPrivate.isEditionModeOpened) ? 1 : 0

            visible: (height != 0)
            clip: (height !== editNetworkSettingsColumn.height)

            Behavior on height {
                enabled: rootPrivate.canPerformAnimations

                NumberAnimation {
                    duration: root.animationDuration
                }
            }

            Behavior on opacity {
                enabled: rootPrivate.canPerformAnimations

                NumberAnimation {
                    duration: root.animationDuration
                }
            }

            Column {
                id: editNetworkSettingsColumn

                anchors {
                    left: parent.left
                    right: parent.right
                }

                spacing: 10

                // Title
                Column {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    spacing: 10

                    Item {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        height: 10

                        Rectangle {
                            anchors {
                                bottom: parent.bottom
                                left: parent.left
                                right: parent.right
                            }

                            height: 1

                            color: background.borderColor
                        }
                    }

                    Text {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        text: qsTr("Edit network settings")

                        color: IngeScapeTheme.whiteColor

                        font {
                            family: IngeScapeTheme.textFontFamily
                            weight: Font.Medium
                            pixelSize: 20
                        }
                    }
                }


                // Select network device
                Column {
                    id: selectNetworkDevice

                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    spacing: 6

                    Text {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        text: qsTr("Network device")

                        color: IngeScapeTheme.whiteColor

                        font {
                            family: IngeScapeTheme.textFontFamily
                            weight: Font.Medium
                            pixelSize: 16
                        }
                    }


                    I2ComboboxStringList {
                        id: selectNetworkDeviceCombobox

                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        height : 25

                        style: IngeScapeComboboxStyle {}
                        scrollViewStyle: IngeScapeScrollViewStyle {}

                        _mouseArea.hoverEnabled: true

                        Controls2.ToolTip {
                            delay: Qt.styleHints.mousePressAndHoldInterval
                            visible: selectNetworkDeviceCombobox._mouseArea.containsMouse
                            text: selectNetworkDeviceCombobox.text
                        }


                        model: null
                        selectedIndex: -1

                        delegate: customDelegate.component

                        IngeScapeToolTipComboboxDelegate {
                            id: customDelegate

                            comboboxStyle: selectNetworkDeviceCombobox.style
                            selection: selectNetworkDeviceCombobox.selectedIndex

                            height: selectNetworkDeviceCombobox.comboButton.height
                            width:  selectNetworkDeviceCombobox.comboButton.width

                            // Called from the component's MouseArea
                            // 'index' is the index of the clicked component inside the model.
                            function onDelegateClicked(index) {
                                selectNetworkDeviceCombobox.onDelegateClicked(index)
                            }

                            // Called from the component to get the text of the current item to display
                            // 'index' is the index of the component to be displayed inside the model.
                            function getItemText(index) {
                                return selectNetworkDeviceCombobox.modelToString(selectNetworkDeviceCombobox.model[index]);
                            }
                        }


                        onSelectedItemChanged: {
                            // TODO
                        }
                    }

                }


                // Select port
                Column {
                    id: selectPort

                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    spacing: 6

                    Text {
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        text: qsTr("Port")

                        color: IngeScapeTheme.whiteColor

                        font {
                            family: IngeScapeTheme.textFontFamily
                            weight: Font.Medium
                            pixelSize: 16
                        }
                    }


                    TextField {
                        id: selectPortTextfield

                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        height: 25

                        verticalAlignment: TextInput.AlignVCenter

                        text: root.currentPort

                        validator: IntValidator {
                            bottom: 1
                            top: 65535
                        }

                        style: I2TextFieldStyle {
                            backgroundColor: IngeScapeTheme.darkBlueGreyColor
                            borderColor: IngeScapeTheme.whiteColor;
                            borderErrorColor: IngeScapeTheme.redColor
                            radiusTextBox: 1
                            borderWidth: 0;
                            borderWidthActive: 1
                            textIdleColor: IngeScapeTheme.whiteColor;
                            textDisabledColor: IngeScapeTheme.darkGreyColor;

                            padding {
                                left: 3
                                right: 3
                            }

                            font {
                                pixelSize:15
                                family: IngeScapeTheme.textFontFamily
                            }

                        }

                        Binding {
                            target: selectPortTextfield
                            property: "text"
                            value: root.port
                        }
                    }

                }


                // Actions
                Item {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: 40

                    property var buttonBoundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");


                    Row {
                        anchors {
                            right: parent.right
                            bottom: parent.bottom
                        }

                        height: childrenRect.height

                        spacing: 15

                        Button {
                            id: buttonCancel

                            width: buttonBoundingBox.width
                            height: buttonBoundingBox.height

                            activeFocusOnPress: true

                            text: qsTr("Cancel")

                            style: I2SvgButtonStyle {
                                fileCache: IngeScapeTheme.svgFileIngeScape

                                pressedID: releasedID + "-pressed"
                                releasedID: "button"
                                disabledID: releasedID + "-disabled"

                                font {
                                    family: IngeScapeTheme.textFontFamily
                                    weight : Font.Medium
                                    pixelSize : 16
                                }
                                labelColorPressed: IngeScapeTheme.blackColor
                                labelColorReleased: IngeScapeTheme.whiteColor
                                labelColorDisabled: IngeScapeTheme.whiteColor

                            }

                            onClicked: {
                                root.close();
                            }
                        }

                        Button {
                            id: buttonOk

                            width: buttonBoundingBox.width
                            height: buttonBoundingBox.height

                            enabled: false

                            activeFocusOnPress: true

                            text: qsTr("OK")

                            style: I2SvgButtonStyle {
                                fileCache: IngeScapeTheme.svgFileIngeScape

                                pressedID: releasedID + "-pressed"
                                releasedID: "button"
                                disabledID: releasedID + "-disabled"

                                font {
                                    family: IngeScapeTheme.textFontFamily
                                    weight : Font.Medium
                                    pixelSize : 16
                                }
                                labelColorPressed: IngeScapeTheme.blackColor
                                labelColorReleased: IngeScapeTheme.whiteColor
                                labelColorDisabled: IngeScapeTheme.greyColor
                            }

                            onClicked: {
                                //TODO
                            }
                        }
                    }
                }
            }
        }
    }

}
