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
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Chloé Roumieu      <roumieu@ingenuity.io>
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

I2PopupBase {
    id: rootPopup

    width: 600
    height: containerPopup.height

    dismissOnOutsideTap: false

    // Our controller
    property LicensesController licensesController: null

    // Property to correctly size the popup when there are several licenses to show
    property real maxHeight : parent.height - 40 // 40 to let a margin above and below our popup

    // Extra information to display when there is no license
    property string extraInformationOnNoLicense: ""


    //--------------------------------------------------------
    //
    //
    // Callbacks
    //
    //
    //--------------------------------------------------------

    onOpened: {
        if (rootPopup.licensesController) {
            txtLicensesPath.text = rootPopup.licensesController.licensesPath;
        }
        rootPopup.focus = true; // Set the focus to catch keyboard press on Return/Escape
    }

    Keys.onEscapePressed: {
        //console.log("QML: Escape Pressed");
        rootPopup.close();
    }

    Keys.onReturnPressed: {
        //console.log("QML: Return Pressed");
        rootPopup.close();
    }


    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

    //
    // Background
    //
    Rectangle {
        anchors.fill: parent

        color: IngeScapeTheme.veryDarkGreyColor

        border {
            width: 2
            color: IngeScapeTheme.editorsBackgroundBorderColor
        }

        radius: 5
    }

    //
    // Popup content
    //
    Item {
        id: containerPopup
        anchors {
            top: parent.top
            right: parent.right
            rightMargin: 18
            left: parent.left
            leftMargin: 18
        }
        height: childrenRect.height

        // Header content : Title and license directory path
        Item {
            id: header
            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
            }
            height: childrenRect.height

            Text {
                id: title
                anchors {
                    top: parent.top
                    topMargin: 25
                    left: parent.left
                }

                text: qsTr("Licenses")

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 23
                }
            }

            Item {
                id: directoryPathItem
                anchors {
                    left: parent.left
                    right: parent.right
                    top: title.bottom
                    topMargin: 18
                }

                height: childrenRect.height

                Text {
                    text: qsTr("Directory path")

                    color: IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight : Font.Medium
                        pixelSize : 16
                    }
                }

                Item {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: childrenRect.height

                    TextField {
                        id: txtLicensesPath

                        anchors {
                            left: parent.left
                            right: btnSelectLicencesDirectory.left
                            rightMargin: 10
                        }
                        height: btnSelectLicencesDirectory.height
                        verticalAlignment: TextInput.AlignVCenter

                        text: "" //rootPopup.controller.licensesPath

                        enabled: false

                        style: I2TextFieldStyle {
                            backgroundColor: IngeScapeTheme.darkBlueGreyColor
                            backgroundDisabledColor: IngeScapeTheme.darkBlueGreyColor

                            borderColor: IngeScapeTheme.whiteColor
                            borderDisabledColor: IngeScapeTheme.whiteColor

                            borderErrorColor: IngeScapeTheme.redColor

                            radiusTextBox: 1
                            borderWidth: 0;
                            borderWidthActive: 1

                            textIdleColor: IngeScapeTheme.whiteColor
                            textDisabledColor: IngeScapeTheme.whiteColor

                            padding.left: 6
                            padding.right: 3

                            font {
                                pixelSize:15
                                family: IngeScapeTheme.textFontFamily
                            }
                        }
                    }

                    Button {
                        id: btnSelectLicencesDirectory

                        property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

                        anchors {
                            right: parent.right
                        }

                        height: boundingBox.height
                        width: boundingBox.width

                        text: "Change..."
                        activeFocusOnPress: true

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
                            if (rootPopup.licensesController)
                            {
                                var directoryPath = rootPopup.licensesController.selectLicensesDirectory();
                                if (directoryPath) {
                                    txtLicensesPath.text = directoryPath;
                                    rootPopup.licensesController.updateLicensesPath(directoryPath);
                                }
                            }
                        }
                    }
                }
            }

            Item {
                id: errorMessage
                anchors {
                    top: directoryPathItem.bottom
                    topMargin: 10
                    left: parent.left
                    right: parent.right
                }
                height: (errorText.text === "") ? 0 : 30
                visible: (rootPopup.licensesController && (rootPopup.licensesController.errorMessageWhenLicenseFailed !== ""))

                SvgImage {
                    id: errorPicto
                    anchors.verticalCenter: parent.verticalCenter
                    height: errorPicto.svgheight
                    svgElementId : "mapping-mode-message-warning"                    
                }

                Text {
                    id: errorText
                    anchors {
                        bottom: errorPicto.bottom
                        left: errorPicto.right
                        leftMargin: 10
                    }

                    wrapMode: Text.WordWrap
                    text: rootPopup.licensesController ? rootPopup.licensesController.errorMessageWhenLicenseFailed : ""

                    color: IngeScapeTheme.orangeColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight : Font.Medium
                        pixelSize : 16
                    }
                }
            }


            // Vertical space
            Item {
                anchors {
                    top: errorMessage.bottom
                    left: parent.left
                    right: parent.right
                }
                height: 10
            }
        }


        // Content when user have one license or more
        Item {
            id: content
            anchors {
                top: header.bottom
                topMargin: 20
                right: parent.right
                left: parent.left
            }
            height: childrenRect.height

            visible: rootPopup.licensesController.licenseDetailsList && rootPopup.licensesController.licenseDetailsList.count > 0

            Column {
                id: summary
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                spacing : 10

                Text {
                    id : summaryTitle
                    anchors {
                        left: parent.left
                        leftMargin: 10
                    }

                    text: rootPopup.licensesController.licenseDetailsList.count > 1 ? qsTr("Summary") : qsTr("Your license")

                    color: IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        capitalization: Font.AllUppercase
                        weight : Font.Medium
                        pixelSize : 20
                    }
                }

                Rectangle {
                    id: summarySeparator
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    height: 1
                    color: IngeScapeTheme.whiteColor
                }

                LicenseInformationView {
                    id: summaryView
                    anchors {
                       right: parent.right
                       left: parent.left
                    }
                    licenseInformation: rootPopup.licensesController.mergedLicense
                }
            }

            Item {
                id: licensesDetails
                anchors {
                    top: summary.bottom
                    topMargin : 25
                    left: parent.left
                    right: parent.right
                }
                height: childrenRect.height

                Item {
                    id: onlyOneLicenseView
                    anchors {
                        top: parent.top
                        right: parent.right
                        left: parent.left
                    }
                    height : childrenRect.height
                    visible: rootPopup.licensesController.licenseDetailsList.count === 1

                    property LicenseInformationM model : rootPopup.licensesController.licenseDetailsList.count === 1 ? rootPopup.licensesController.licenseDetailsList.get(0) : null

                    LabellessSvgButton {
                        id: buttonDelete
                        anchors {
                            top: parent.top
                            right: parent.right
                        }
                        enabled: true

                        releasedID: "delete-license"
                        pressedID: "delete-license-pressed"
                        disabledID: "delete-license-pressed"

                        onClicked: {
                            // Open delete confirmation popup
                            deleteConfirmationPopup.license = onlyOneLicenseView.model
                            deleteConfirmationPopup.open();
                        }
                    }

                    SvgImage {
                        id: licenseFilePicto
                        anchors {
                            verticalCenter : buttonDelete.verticalCenter
                            left: parent.left
                        }

                        visible: licenseFileText.text !== ""

                        svgElementId : onlyOneLicenseView.model && onlyOneLicenseView.model.ingescapeLicenseValidity  &&  onlyOneLicenseView.model.editorLicenseValidity ? "license-ok" : "license-fail"
                    }

                    Text {
                        id : licenseFileText
                        anchors {
                            verticalCenter: licenseFilePicto.verticalCenter
                            left: licenseFilePicto.right
                            leftMargin: 8
                        }

                        text: onlyOneLicenseView.model && onlyOneLicenseView.model.fileName

                        color: IngeScapeTheme.whiteColor
                        font {
                            family: IngeScapeTheme.textFontFamily
                            weight : Font.Medium
                            pixelSize : 14
                        }
                    }

                    Rectangle {
                        id: endSeparator
                        anchors {
                            left: parent.left
                            right: parent.right
                            top: buttonDelete.bottom
                            topMargin : 10
                        }
                        height: 1
                        color: IngeScapeTheme.whiteColor
                    }
                }

                Item {
                    id: severalLicensesView
                    anchors {
                        top: parent.top
                        right: parent.right
                        left: parent.left
                    }
                    height: severalLicensesView.visible ? rootPopup.maxHeight - licensesDetails.y - content.y - footer.height
                                                        : 0 // To resize popup

                    visible: rootPopup.licensesController.licenseDetailsList.count > 1

                    Behavior on height {
                        NumberAnimation {}
                    }

                    Text {
                        id : detailsTitle
                        anchors {
                            top: parent.top
                            left: parent.left
                            leftMargin: 10
                        }

                        text: qsTr("Your licenses")

                        color: IngeScapeTheme.whiteColor
                        font {
                            family: IngeScapeTheme.textFontFamily
                            capitalization: Font.AllUppercase
                            weight : Font.Medium
                            pixelSize : 20
                        }
                    }

                    Rectangle {
                        id: detailsSeparator
                        anchors {
                            left: parent.left
                            right: parent.right
                            top: detailsTitle.bottom
                            topMargin : 10
                        }
                        height: 1
                        color: IngeScapeTheme.whiteColor
                    }

                    ScrollView {
                        id: detailsScrollView
                        anchors {
                            top: detailsSeparator.bottom
                            topMargin: 10
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }

                        style: IngeScapeScrollViewStyle {
                        }

                        // Prevent drag overshoot on Windows
                        flickableItem.boundsBehavior: Flickable.OvershootBounds

                        // Content of our scrollview
                        ListView {
                            model: rootPopup.licensesController ? rootPopup.licensesController.licenseDetailsList : 0

                            delegate: Item {
                                anchors {
                                    left : parent.left
                                    right : parent.right
                                }
                                height: childrenRect.height

                                ExpanderItem {
                                    id: expanderLicense
                                    anchors {
                                        left : parent.left
                                        right : parent.right
                                    }

                                    headerText: model.fileName
                                    headerHeight: 35
                                    leftMarginTextHeader : 40

                                    pictoHeader: model && model.ingescapeLicenseValidity  &&  model.editorLicenseValidity ? "license-ok" : "license-fail"

                                    contentHeightWhenOpened : expandingLicenseInfosContainer.height

                                    data : Item {
                                        id: expandingLicenseInfosContainer
                                        width: parent.width
                                        height: (expandingLicenseInfos.height + expandingLicenseInfos.anchors.topMargin + expandingLicenseInfos.anchors.bottomMargin)

                                        LicenseInformationView {
                                            id: expandingLicenseInfos
                                            anchors {
                                                top: parent.top
                                                topMargin: 10
                                                bottomMargin: 10
                                                right: parent.right
                                                rightMargin: 40
                                                left: parent.left
                                                leftMargin: 40
                                            }

                                            simplifiedWiew: false
                                            licenseInformation: rootPopup.licensesController.licenseDetailsList.get(index)
                                        }
                                    }
                                }

                                Item {
                                    id: containerButton
                                    anchors {
                                        top: parent.top
                                        right: parent.right
                                    }
                                    height: 35

                                    LabellessSvgButton {
                                        id: btnDeleteSeveralLicense
                                        anchors {
                                            right: parent.right
                                            rightMargin: 40
                                            verticalCenter: parent.verticalCenter
                                        }

                                        visible: expanderLicense.checked

                                        releasedID: "delete-license"
                                        pressedID: "delete-license-pressed"
                                        disabledID: "delete-license-pressed"

                                        onClicked: {
                                            // Open delete confirmation popup
                                            deleteConfirmationPopup.license = model.QtObject
                                            deleteConfirmationPopup.open();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }


        // Content when user have no license
        Item {
            id: noLicenseContent
            anchors.fill: content
            visible: !content.visible

            Text {
                id: noLicenseContentText
                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }

                text: qsTr("No license file found.\n " + rootPopup.extraInformationOnNoLicense + "\n\n\nTo change this, please set the license directory above,\ndrop a license file here \nor use the \"Import...\" button below.")
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter

                color: IngeScapeTheme.whiteColor
                wrapMode: Text.WordWrap
                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize : 18
                    italic: true
                }
            }

            Button {
                property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

                height: boundingBox.height
                width: boundingBox.width

                activeFocusOnPress: true
                text: qsTr("Import...")

                anchors {
                    top: noLicenseContentText.bottom
                    topMargin: 25
                    horizontalCenter: parent.horizontalCenter
                }

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
                    if (rootPopup.licensesController)
                    {
                        rootPopup.licensesController.importLicense();
                    }
                }
            }
        }


        // Drop Area to add a license from the editor (only when user have no license)
        DropArea {
            id: dropZone
            anchors.fill: content

            enabled: true

            property bool dragHovering: false

            onEntered: {
                dragHovering = true
            }

            onExited: {
                dragHovering = false
            }

            onDropped: {
                dragHovering = false
                if (drop.hasUrls && rootPopup.licensesController)
                {
                    rootPopup.licensesController.addLicenses(drop.urls)
                }
            }

            // Overlay appearing when the user drags something over the drop zone
            Rectangle {
                id: dropZoneOverlay
                anchors.fill: parent

                color: IngeScapeTheme.veryLightGreyColor
                opacity: dropZone.dragHovering ? 0.65 : 0

                Behavior on opacity {
                    NumberAnimation {}
                }
            }
        }

        // Footer content : Button "OK"
        Item {
            id: footer
            anchors {
                top: content.bottom
                right : parent.right
                left: parent.left
            }
            height: 70


            Button {
                id: okButton
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                }

                property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");
                height: boundingBox.height
                width: boundingBox.width

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
//                    console.log("QML: OK on License Popup")
                    rootPopup.close();
                }
            }
        }
    }


    //
    // Delete Confirmation Popup
    //

    // Overlay layer used to display delete confirmation popup
    I2Layer {
        id: licensePopupLayer
        anchors.fill: parent
        objectName: "licensePopupLayer"
    }

    ConfirmationPopup {
        id: deleteConfirmationPopup

        property LicenseInformationM license: null

        layerColor : '#99000000';
        layerObjectName : 'licensePopupLayer';

        confirmationText: qsTr("You will definitely lose all the rights this license grants you.\nDo you want to delete it completely ?")

        onConfirmed: {
            if (deleteConfirmationPopup.license && rootPopup.licensesController)
            {
                rootPopup.licensesController.deleteLicense(license);
            }
        }
    }
}
