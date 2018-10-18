import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0
import INGESCAPE 1.0

Item {
    id : dropDown

    property alias placeholderText: _comboPlaceholder.text
    property alias text: _comboText.text
    property alias checkAllText: checkAllBox.text
    property alias model: _combolist.model
    property alias delegate: _combolist.delegate
    property alias checkAllState: checkAllBox.checkedState
    property alias isPartiallyChecked: checkAllBox.isPartiallyChecked
    property alias comboButton: _comboButton
    property alias popup: popup

    // count displayable items
    property int countDisplayItem: 5

    onVisibleChanged: {
        if (!visible) {
            close();
        }
    }

    signal checkAll()
    signal uncheckAll()
    signal popupOpen()

    /***
    * open function : open the combobox
    ***/
    function open() {
        _comboButton.checked = true;
        popup.open();
    }

    /***
    * close function : close the combobox
    ***/
    function close() {
        _comboButton.checked = false;
        dropDown.forceActiveFocus();
        popup.close();
    }

    Rectangle {
        id: _comboButton

        property bool checked : false;

        width: parent.width
        height: parent.height
        radius: 1

        border.width: _mouseAreaCombo.containsPress ? 1 : 0
        border.color: IngeScapeTheme.darkBlueGreyColor
        color: _mouseAreaCombo.containsPress ? IngeScapeTheme.darkGreyColor2 : IngeScapeTheme.darkBlueGreyColor

        Text {
            id: _comboPlaceholder

            visible: (_comboText.text === "");
            anchors {
                verticalCenter: parent.verticalCenter;
                left: parent.left;
                leftMargin: 10
                right: _imageCombo.left;
                rightMargin: 10
            }

            font {
                pixelSize: 15
                family: IngeScapeTheme.textFontFamily;
                italic : true;
            }

            color : IngeScapeTheme.greyColor
            verticalAlignment: Text.AlignVCenter;
            elide : Text.ElideRight;
        }


        Text {
            id:_comboText

            anchors {
                verticalCenter: parent.verticalCenter;
                left: parent.left;
                leftMargin: 10
                right: _imageCombo.left;
                rightMargin: 10
            }

            font {
                pixelSize: 15
                family: IngeScapeTheme.textFontFamily;
            }

            color : IngeScapeTheme.lightGreyColor
            verticalAlignment: Text.AlignVCenter;
            elide : Text.ElideRight;
        }


        Image {
            id: _imageCombo;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.rightMargin: 10
            rotation : (_comboButton.checked ? 180 : 0);
            source : "image://I2svg/resources/SVG/ingescape-pictos.svg#iconCombo";

            Behavior on rotation {
                NumberAnimation {}
            }
        }


        MouseArea {
            id: _mouseAreaCombo;
            anchors.fill: parent;
            activeFocusOnTab: true;
            onClicked: {
                _mouseAreaCombo.forceActiveFocus();
                _comboButton.checked ? close() : open()
            }
        }

        onVisibleChanged: {
            if (!visible) {
                close();
            }
        }
    }

    I2PopupBase {
        id: popup

        anchors.top: _comboButton.bottom
        width: _comboButton.width
        height: ((_combolist.count < dropDown.countDisplayItem) ? (1 + _combolist.count) * _comboButton.height
                                                                : (1 + dropDown.countDisplayItem + 0.5) * _comboButton.height )

        isModal: true
        layerColor: "transparent"
        layerObjectName: "overlayLayerComboBox"
        dismissOnOutsideTap: true

        keepRelativePositionToInitialParent : true;

        onClosed: {
            dropDown.close();
        }

        Rectangle {
            id : popUpBackground
            anchors.fill : parent
            color: IngeScapeTheme.darkBlueGreyColor
        }

        ScrollView {
            id : _scrollView

            visible: _comboButton.checked;

            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            width: _comboButton.width

            // Prevent drag overshoot on Windows
            flickableItem.boundsBehavior: Flickable.OvershootBounds

            style: IngeScapeScrollViewStyle {}

            contentItem: Item {
                width: _scrollView.width
                height: _scrollView.height

                CheckBox {
                    id: checkAllBox

                    property string text: ""
                    property bool isPartiallyChecked: false

                    anchors {
                        left: parent.left
                        leftMargin :10
                        right : parent.right
                        rightMargin : 10
                        top : parent.top
                    }
                    height: _comboButton.height

                    checked: false;
                    activeFocusOnPress: true;

                    style: CheckBoxStyle {
                        label: Text {
                            anchors {
                                verticalCenter: parent.verticalCenter
                                verticalCenterOffset: 1
                            }

                            color: IngeScapeTheme.lightGreyColor

                            elide: Text.ElideRight
                            text: checkAllBox.text

                            font {
                                family: IngeScapeTheme.textFontFamily
                                pixelSize: 16
                            }

                        }

                        indicator: Rectangle {
                            implicitWidth: 14
                            implicitHeight: 14
                            border.width: 0
                            color: IngeScapeTheme.veryDarkGreyColor

                            I2SvgItem {
                                visible: (control.checkedState === Qt.Checked)
                                anchors.centerIn: parent

                                svgFileCache: IngeScapeTheme.svgFileINGESCAPE;
                                svgElementId: "check";

                            }

                            Text {
                                visible : checkAllBox.isPartiallyChecked
                                anchors {
                                    centerIn: parent
                                }

                                color: IngeScapeTheme.lightGreyColor

                                text: "-"
                                elide: Text.ElideRight

                                font {
                                    family: IngeScapeTheme.textFontFamily
                                    pixelSize: 16
                                }
                            }
                        }
                    }

                    onClicked : {
                        // reset isPartiallyChecked property
                        checkAllBox.isPartiallyChecked = false;

                        // send signal to check/uncheck all entries
                        checkAllBox.checked ? checkAll() : uncheckAll()
                    }

                    Connections {
                        target: popup

                        onOpened : {
                            popupOpen();
                        }
                    }
                }

                ListView {
                    id: _combolist

                    boundsBehavior: Flickable.StopAtBounds
                    interactive: false

                    anchors {
                        top : checkAllBox.bottom
                        left: parent.left
                        right: parent.right
                    }
                    height: contentHeight
                }
            }
        }
    }
}

