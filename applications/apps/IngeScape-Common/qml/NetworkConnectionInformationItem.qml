/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Chloé Roumieu   <roumieu@ingenuity.io>
 *
 */

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

    // Redefine our default property to add extra children to our 'extraContentItem' item
    // NB: We use the 'data' property instead of the 'children' property to allow any type
    //     of content and not only visual items (data is a list<Object> AND children is a list<Item>)
    default property alias extraContent: extraContentItem.data

    // Flag indicating if our edition mode is opened
    readonly property alias isEditionModeOpened: rootPrivate.isEditionModeOpened

    // Content width of our component
    readonly property alias contentWidth: content.width

    // Flag indicating if editor is started on ingescape platform
    // NB : if false, it means that no network devices were available
    //      or that user have to make a choice to launch the editor on igs platform
    property bool editorStartedOnIgs: true

    // Current network device
    property string currentNetworkDevice: ""

    // Current port
    property int currentPort: 5670

    // Duration of animations in milliseconds (250 ms => default duration of QML animations)
    property int animationDuration: 250

    // List of network devices
    property alias listOfNetworkDevices: selectNetworkDeviceCombobox.model

    // Selected index of our list of network devices
    property alias listOfNetworkDevicesSelectedIndex: selectNetworkDeviceCombobox.selectedIndex;

    // Auto-close timeout in milliseconds
    property alias autoCloseTimeoutInMilliseconds: autoCloseTimer.interval

    // Open settings button source appearance
    property string settingsReleasedId : "mapping-mode-settings"
    property string settingsHighlightedId: "mapping-mode-settings-hover"
    property string settingsPressedId : "mapping-mode-settings-pressed"

    //
    // Configure our item
    //
    width: 243
    height: contentMouseArea.height

    // Radius
    topRightRadius: 5
    topLeftRadius: 5
    bottomRightRadius: 5
    bottomLeftRadius: 5

    fuzzyRadius: 8

    color: rootPrivate.mustBeHighlighted ? IngeScapeTheme.darkBlueGreyColor : IngeScapeTheme.veryDarkGreyColor

    borderColor: rootPrivate.mustBeHighlighted ? IngeScapeTheme.whiteColor : IngeScapeTheme.editorsBackgroundBorderColor
    borderWidth: 1


    //
    // Private properties
    //
    QtObject {
        id: rootPrivate

        // Flag indicating if we can perform animations
        property bool canPerformAnimations: false

        // Flag indicating if our edition mode is opened
        property bool isEditionModeOpened: false

        // Flag indicating if we can auto-close our edition mode
        property bool canAutoCloseEditionMode: false

        // Boolean indicating if we must highlight our component
        property bool mustBeHighlighted : !root.isEditionModeOpened && contentMouseArea.enabled && contentMouseArea.containsMouse && !contentMouseArea.pressed

        // Flag indicating if some network devices are available
        property bool networkDevicesAvailable : root.listOfNetworkDevices.length > 0
    }


    //--------------------------------------------------------
    //
    // Signals
    //
    //--------------------------------------------------------

    // Triggered when our edition mode will be opened
    signal willOpenEditionMode();

    // Triggered when our edition mode will be closed
    signal willCloseEditionMode();

    // Triggered when we press the "OK" button
    signal changeNetworkSettings(string networkDevice, int port);

    signal connectChanged(bool connect);


    //--------------------------------------------------------
    //
    // Methods
    //
    //--------------------------------------------------------

    // Open our widget
    function open()
    {
        if (!rootPrivate.isEditionModeOpened)
        {
            // Notify change
            root.willOpenEditionMode();

            // Update internal states
            rootPrivate.isEditionModeOpened = true;
            rootPrivate.canAutoCloseEditionMode = true;

            // Get focus
            root.forceActiveFocus();

            // Reset UI
            // - network device
            resetComboboxSelectNetworkDevice();
            // - port
            selectPortTextfield.text = Qt.binding(function() {
               return root.currentPort;
            });
        }
    }

    // Close our widget
    function close()
    {
        if (rootPrivate.isEditionModeOpened)
        {
            // Stop our timer
            autoCloseTimer.stop();

            // Notify change
            root.willCloseEditionMode();

            // Update internal states
            rootPrivate.isEditionModeOpened = false;
            rootPrivate.canAutoCloseEditionMode = false;
        }
    }

    // Reset our combobox used to select a network device
    function resetComboboxSelectNetworkDevice()
    {
        // Close our combobox
        selectNetworkDeviceCombobox.close();

        // Update selected index
        selectNetworkDeviceCombobox.selectedIndex = Qt.binding(function() {
            var index = (
                         (root.listOfNetworkDevices)
                         ? root.listOfNetworkDevices.indexOf(root.currentNetworkDevice)
                         : -1
                        );

            // When our current network device is offline, we try to help our end-users
            // by selecting a network device when we can i.e. when there is a single network device available
            return (
                    (index >= 0)
                    ? index
                    : (((root.listOfNetworkDevices) && (root.listOfNetworkDevices.length === 1)) ? 0 : -1)
                   );
        });
    }

    // Check if values are valid or not
    function checkValues(index, networkDevice, port, forceQmlUpdate)
    {
        return (
                // Coherent values
                (index >= 0)
                && (networkDevice !== "")
                && (port !== "")
                // Values are still available
                && listOfNetworkDevices
                && (listOfNetworkDevices.indexOf(networkDevice) >= 0)
                );
    }

    // Reset auto-close timer
    function resetAutoCloseTimer()
    {
        rootPrivate.canAutoCloseEditionMode = false;
        rootPrivate.canAutoCloseEditionMode = true;
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

    onListOfNetworkDevicesChanged: {
        resetComboboxSelectNetworkDevice();

        rootPrivate.networkDevicesAvailable = root.listOfNetworkDevices.length > 0;
        if (!rootPrivate.networkDevicesAvailable) {
            close();
        }
    }

    onVisibleChanged: {
        if (visible == false) {
            close();
        }
    }

    // Timer used to auto-close our edition mode
    Timer {
        id: autoCloseTimer

        running: root.editorStartedOnIgs && rootPrivate.canAutoCloseEditionMode
                 && !(selectNetworkDeviceCombobox.comboList.visible || selectPortTextfield.activeFocus)

        repeat: false
        triggeredOnStart: false

        interval: 20000

        onTriggered: {
            root.close();
        }
    }


    //--------------------------------------------------------
    //
    // Content
    //
    //--------------------------------------------------------

    //
    // Mouse Area allowing hover & click on our component
    //
    MouseArea {
        id: contentMouseArea

        anchors.top : parent.top

        width: parent.width
        height: childrenRect.height

        enabled: rootPrivate.networkDevicesAvailable

        hoverEnabled: true

        onClicked: {
            if (rootPrivate.isEditionModeOpened && root.editorStartedOnIgs)
            {
                root.close();
            }
            else if (!rootPrivate.isEditionModeOpened)
            {
                root.open();
            }
        }

        onPositionChanged: {
            if (rootPrivate.isEditionModeOpened) {
                resetAutoCloseTimer();
            }
        }


        //
        // Content of our component
        //
        Column {
            id: content

            anchors {
                top: parent.top
                left: parent.left
                leftMargin: 10
                right: parent.right
                rightMargin: 8
            }

            //
            // Header of our component (always visible)
            //
            Item {
                id : header

                width: parent.width
                height: 70

                // Header of our component when editor is online
                Item {
                    id: headerContentOnline
                    anchors.fill: parent

                    visible: rootPrivate.networkDevicesAvailable

                    // Button - Open network settings
                    LabellessSvgButton {
                        id: buttonOpenSettings

                        anchors {
                            right: parent.right
                            top: parent.top
                            topMargin: 8
                        }

                        releasedID: rootPrivate.mustBeHighlighted ? root.settingsHighlightedId : root.settingsReleasedId
                        pressedID: root.settingsPressedId

                        onClicked: {
                            rootPrivate.isEditionModeOpened ? root.close() : root.open();
                        }

//                        Controls2.ToolTip {
//                            delay: Qt.styleHints.mousePressAndHoldInterval
//                            visible: buttonOpenSettings.enabled && buttonOpenSettings.__behavior.containsMouse
//                            text: qsTr("Edit network settings")
//                        }
                    }

                    // Toggle - Connect/Disconnect current platform
                    Button {
                        id : connectButton
                        anchors {
                            left: parent.left
                            top : buttonOpenSettings.verticalCenter
                            rightMargin: 8
                        }

                        visible: true
                        enabled: visible && root.editorStartedOnIgs

                        style: I2SvgToggleButtonStyle {
                            fileCache: IngeScapeTheme.svgFileIngeScape

                            toggleCheckedReleasedID: "mapping-mode-toggle-connected";
                            toggleCheckedPressedID: "mapping-mode-toggle-connected-pressed";
                            toggleUncheckedReleasedID: "mapping-mode-toggle-disconnected";
                            toggleUncheckedPressedID: "mapping-mode-toggle-disconnected-pressed";

                            // No disabled states
                            toggleCheckedDisabledID: ""; // No checked disable state
                            toggleUncheckedDisabledID: "mapping-mode-toggle-disconnected-disabled";

                            labelMargin: 0;
                        }

                        onClicked: {
                            root.connectChanged(checked);
                            checked = false;
                        }

                        Binding {
                            target: connectButton
                            property: "checked"
                            value: IgsNetworkController.isStarted
                        }
                    }

                    // Text CONNECTED/DISCONNECTED
                    Text {
                        id: textInfoPart1
                        anchors {
                            verticalCenter: connectButton.verticalCenter
                            left: connectButton.visible ? connectButton.right : parent.left
                            right: parent.right
                            leftMargin: 8
                        }

                        text: (IgsNetworkController && IgsNetworkController.isStarted)  ? qsTr("ON-line")
                                                                                        : qsTr("OFF-line")

                        color: (IgsNetworkController && IgsNetworkController.isStarted) ? IngeScapeTheme.whiteColor
                                                                                        : IngeScapeTheme.lightGreyColor
                        font {
                            family: IngeScapeTheme.labelFontFamily
                            weight: Font.Bold
                            pixelSize: 18
                            capitalization: Font.AllUppercase
                        }
                    }

                    // Infos about network device and port used
                    Text {
                       id: textInfoPart2

                       anchors {
                           left: textInfoPart1.left
                           right: textInfoPart1.right
                           top: connectButton.bottom
                       }

                       visible : root.editorStartedOnIgs

                       color: (IgsNetworkController && IgsNetworkController.isStarted) ? IngeScapeTheme.veryLightGreyColor
                                                                                       : IngeScapeTheme.lightGreyColor

                       font {
                           family: IngeScapeTheme.heading2Font
                           weight: Font.Normal
                           pixelSize: 14
                       }

                       elide: Text.ElideRight

                       text: qsTr("on " + root.currentNetworkDevice + " and port " + root.currentPort)
                   }
                }

                // Header of our component when editor is offline
                Item {
                    id: headerContentOffline

                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                        horizontalCenter: parent.horizontalCenter
                    }

                    width: childrenRect.width

                    visible: !headerContentOnline.visible

                    SvgImage {
                        id: warningPicto

                        anchors.verticalCenter: parent.verticalCenter

                        svgElementId : "mapping-mode-message-warning"
                    }

                    Text {
                        id: message

                        anchors {
                            left: warningPicto.right
                            leftMargin: 15
                            verticalCenter: parent.verticalCenter
                        }

                        text: "No network device"

                        color: IngeScapeTheme.lightGreyColor
                        font {
                            family: IngeScapeTheme.labelFontFamily
                            weight: Font.ExtraBold
                            pixelSize: 18
                            capitalization: Font.AllUppercase
                        }
                    }
                }
            }


            //
            // Network settings (only visible when it is opened)
            //
            Item {
                id: editNetworkSettings

                width: parent.width
                height: (rootPrivate.isEditionModeOpened) ?  childrenRect.height : 0

                opacity: (rootPrivate.isEditionModeOpened) ? 1 : 0

                visible: (height !== 0)

                clip: (height !== childrenRect.height)

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


                // First separator
                Rectangle {
                    id: separator

                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                    }

                    height: 1

                    color: IngeScapeTheme.editorsBackgroundBorderColor
                }


                // Item that will contain our extra content
                // NB: our extra content must have a fixed width and a fixed height
                //     It MUST NOT rely on anchors
                Item {
                    id: extraContentItem

                    anchors {
                        top: separator.top
                        left: parent.left
                    }

                    height: childrenRect.height
                    width: childrenRect.width
                }


                // Select network device
                Text {
                    id: labelNetworkDevice

                    anchors {
                        left: parent.left
                        right: parent.right
                        top: extraContentItem.bottom
                        topMargin: 14
                    }

                    text: qsTr("Network device")

                    color: IngeScapeTheme.lightGreyColor

                    font {
                        family: IngeScapeTheme.textFontFamily
                        pixelSize: 16
                    }
                }

                I2ComboboxStringList {
                    id: selectNetworkDeviceCombobox

                    anchors {
                        left: parent.left
                        right: parent.right
                        top: labelNetworkDevice.bottom
                        topMargin: 9
                    }

                    height : 22

                    style: IngeScapeComboboxStyle {}
                    scrollViewStyle: IngeScapeScrollViewStyle {}

                    _mouseArea.hoverEnabled: true

                    placeholderText: qsTr("Select a network device...")

                    model: null
                    selectedIndex: -1

                    delegate: customDelegate.component

                    IngeScapeToolTipComboboxDelegate {
                        id: customDelegate

                        comboboxStyle: selectNetworkDeviceCombobox.style
                        selection: selectNetworkDeviceCombobox.selectedIndex

                        height: selectNetworkDeviceCombobox.height
                        width:  selectNetworkDeviceCombobox.width

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

                    onSelectedIndexChanged: {
                        root.resetAutoCloseTimer();
                    }
                }


                // Select port
                Text {
                    id: labelPort
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: selectNetworkDeviceCombobox.bottom
                        topMargin: 18
                    }

                    text: qsTr("Port")

                    color: IngeScapeTheme.lightGreyColor

                    font {
                        family: IngeScapeTheme.textFontFamily
                        pixelSize: 16
                    }
                }

                TextField {
                    id: selectPortTextfield

                    anchors {
                        left: parent.left
                        right: parent.right
                        top: labelPort.bottom
                        topMargin: 9
                    }

                    height: 22

                    verticalAlignment: TextInput.AlignVCenter

                    text: root.currentPort

                    validator: IntValidator {
                        bottom: 1
                        top: 65535
                    }

                    style: I2TextFieldStyle {
                        backgroundColor: IngeScapeTheme.darkBlueGreyColor
                        borderColor: IngeScapeTheme.whiteColor
                        borderErrorColor: IngeScapeTheme.redColor
                        radiusTextBox: 1
                        borderWidth: 0
                        borderWidthActive: 1
                        textIdleColor: IngeScapeTheme.whiteColor
                        textDisabledColor: IngeScapeTheme.darkGreyColor

                        padding {
                            left: 3
                            right: 3
                        }

                        font {
                            pixelSize:15
                            family: IngeScapeTheme.textFontFamily
                        }
                    }

                    onTextChanged: {
                        root.resetAutoCloseTimer();
                    }
                }


                // Actions
                Item {
                    id: actions

                    anchors {
                        top: selectPortTextfield.bottom
                        left: parent.left
                        right: parent.right
                        topMargin: 25
                    }

                    height: 43

                    property var buttonBoundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

                    Row {
                        anchors {
                            right: parent.right
                            top: parent.top
                        }

                        height: actions.buttonBoundingBox.height

                        spacing: 15

                        // Cancel button
                        Button {
                            id: buttonCancel

                            anchors {
                                top: parent.top
                                bottom: parent.bottom
                            }

                            visible: root.editorStartedOnIgs

                            width: actions.buttonBoundingBox.width

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

                        // Ok button
                        Button {
                            id: buttonOk

                            anchors {
                                top: parent.top
                                bottom: parent.bottom
                            }

                            width: actions.buttonBoundingBox.width

                            enabled: root.checkValues(selectNetworkDeviceCombobox.selectedIndex,
                                                      selectNetworkDeviceCombobox.selectedItem,
                                                      selectPortTextfield.text,
                                                      (root.listOfNetworkDevices ? root.listOfNetworkDevices.length : 0)
                                                      )

                            activeFocusOnPress: true

                            text: qsTr("OK")

                            style: I2SvgButtonStyle {
                                fileCache: IngeScapeTheme.svgFileIngeScape

                                pressedID: releasedID + "-pressed"
                                releasedID: "button"
                                disabledID: releasedID + "-disabled"

                                font {
                                    family: IngeScapeTheme.textFontFamily
                                    weight: Font.Medium
                                    pixelSize: 16
                                }
                                labelColorPressed: IngeScapeTheme.blackColor
                                labelColorReleased: IngeScapeTheme.whiteColor
                                labelColorDisabled: IngeScapeTheme.greyColor
                            }

                            onClicked: {
                                root.changeNetworkSettings(selectNetworkDeviceCombobox.selectedItem, selectPortTextfield.text);
                            }
                        }
                    }
                }
            }
        }
    }


    //--------------------------------------------------------
    //
    // Layers
    //
    //--------------------------------------------------------

    // layer used to display comboboxes
    I2Layer {
        id: overlayLayerComboBox

        objectName: "overlayLayerComboBox"

        anchors.fill: parent
    }
}
