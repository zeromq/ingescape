import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

// TOOLTIP:
// https://doc.qt.io/qt-5.11/qtquickcontrols2-differences.html
// Qt Quick Controls 1: Button and Action have built-in Qt Widgets-based tooltips
// Qt Quick Controls 2: ToolTip can be attached to any Item

// Needed to access to ToolTip
import QtQuick.Controls.Private 1.0

// Needed to access to ToolTip (https://doc.qt.io/qt-5.11/qml-qtquick-controls2-tooltip.html)
import QtQuick.Controls 2.0 as Controls2


import I2Quick 1.0
import INGESCAPE 1.0


Item{
    id: _combobox

    height : 22
    width : 115


    /**
     * ComboBox Properties
     */

    // count displayable items
    property int countDisplayItem: 5;

    //selected Item & Index
    property var selectedItem;
    property alias selectedIndex: _combolist.currentIndex;

    //Model
    property alias model: _combolist.model;

    //ScrollView
    property alias scrollViewStyle: _scrollView.style;

    //List Element Count
    property alias count: _combolist.count;

    property alias text: _comboText.text;

    property alias placeholderText: _comboPlaceholder.text;

    //Flag to display list on top
    property bool openOnTop: false;

    // Flag to use modelToString function or "modelData"
    property bool useQStringList : false;

    property Item style: I2ComboboxStyle {
        backgroundColorIdle: IngeScapeTheme.darkBlueGreyColor
        backgroundColorDisabled: IngeScapeTheme.darkBlueGreyColor
        borderWidthIdle: 0
        borderColorPressed: IngeScapeTheme.darkBlueGreyColor
        borderWidthPressed: 1
        currentTextColorIdle: IngeScapeTheme.whiteColor
        placeholderTextColor: IngeScapeTheme.greyColor
        currentTextColorDisabled: IngeScapeTheme.greyColor
        backgroundColorPressed: IngeScapeTheme.darkGreyColor2
        radius: 1

        font {
            family : IngeScapeTheme.textFontFamily
            pixelSize : 15
        }

        placeholderFont {
            family : IngeScapeTheme.textFontFamily
            pixelSize : 15
            italic : true
        }

        listBackgroundColorIdle : "#2C333E"
        listBackgroundColorSelected: listBackgroundColorIdle
        listItemTextColorIdle : IngeScapeTheme.whiteColor
        frameVisible : false
        listRadius: 0
        listItemTextColorSelected : IngeScapeTheme.orangeColor
        listBackgroundColorPressed : IngeScapeTheme.darkBlueGreyColor

        isI2Icon : false
        dropdownIcon : "image://I2svg/resources/SVG/ingescape-pictos.svg#iconCombo"
        iconMarginRight : 6

        layerObjectName : "overlayLayerComboBox"
    }


    onVisibleChanged: {
        if (!visible) {
            close();
        }
    }

    onSelectedItemChanged: {
        _updateCurrentSelection();
    }

    onCountChanged: {
         _updateCurrentSelection();
    }

    onModelChanged: {
        _updateCurrentSelection();
    }

    onStyleChanged: {
        if (style) {
            style.control = _combobox;
        }
    }


    // signal triggered when ComboBox is closing
    signal closing();

    // signal triggered when ComboBox is open
    signal opening();

    /////////////////////////////////////////////////////

    /**
      * Function get it change for each type of list
      */
    function _getItemModel(index)
    {
        var modelIsSortFilterLister = (model.hasOwnProperty('resultCount'));
        if(modelIsSortFilterLister)
        {
             return _combolist.model.resultGet(index);
        }
        else if(typeof _combolist.model.get != "undefined")
        {
            return  _combolist.model.get(index);
        }
        else
        {
            // Its an array
            return _combolist.model[index];
        }
    }

    /**
      * Function called when we need to update our current selection
      */
    function _updateCurrentSelection() {
        if (selectedItem)
        {
            // Check if we have a simple list or a sort-filter list


            var index = 0;
            var found = false;

            while (!found && (index < _combolist.count))
            {
                var item = _getItemModel(index);
                var textItem = (typeof item !=="string")? modelToString(item):item;
                if (
                        (useQStringList === false && modelToString(selectedItem) === textItem)
                        || ((typeof selectedItem === "string") && selectedItem === textItem)
                        || (useQStringList === true && selectedItem.modelData === textItem)
                        )
                {
                    _combolist.currentIndex = index;
                    _comboText.text = textItem;
                    found = true;
                }

                index++;
             }
        }
        else {
            _combolist.currentIndex = -1;
            _comboText.text = "";
        }
    }


    /**
     * function model to string
     * convert a list model to a string
     * NB : Overload the function to match with your model.
     */
    function modelToString(model) {
        return model.name + " : " + model.cost+" €";
    }

    /***
    * open function : open the combobox
    ***/
    function open() {
       _comboButton.checked = true;
        popup.open();
       opening();
    }

    /***
    * close function : close the combobox
    ***/
    function close() {
        _comboButton.checked = false;
         popup.close();
        closing();
    }


    /*Keys.onReturnPressed: {
        _comboButton.checked ? _combobox.close() : _combobox.open();
    }*/


    Rectangle {
        id: _comboButton

        property bool checked : false;

        width:  parent.width
        height: parent.height
        radius: _combobox.style.radius

        color: (!_combobox.enabled ? _combobox.style.backgroundColorDisabled : (_mouseAreaCombo.pressed ? _combobox.style.backgroundColorPressed : _combobox.style.backgroundColorIdle));
        border.width: (!_combobox.enabled ? _combobox.style.borderWidthDisabled : (_mouseAreaCombo.pressed ? _combobox.style.borderWidthPressed : _combobox.style.borderWidthIdle));
        border.color: (!_combobox.enabled ? _combobox.style.borderColorDisabled : (_mouseAreaCombo.pressed ? _combobox.style.borderColorPressed : _combobox.style.borderColorIdle));

        Text {
            id: _comboPlaceholder

            visible: (_comboText.text === "")

            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 10
                right: _imageCombo.left
                rightMargin: 10
            }
            font: _combobox.style.placeholderFont
            color: (!_combobox.enabled ? _combobox.style.currentTextColorDisabled : _combobox.style.placeholderTextColor)
            verticalAlignment: Text.AlignVCenter
            elide : Text.ElideRight
        }

        Text {
            id: _comboText

            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 10;
                right: _imageCombo.left
                rightMargin: 10
            }

            font: _combobox.style.font
            color: (!_combobox.enabled ? _combobox.style.currentTextColorDisabled : (_mouseAreaCombo.pressed ?_combobox.style.currentTextColorPressed : _combobox.style.currentTextColorIdle));
            verticalAlignment: Text.AlignVCenter;
            elide : Text.ElideRight;
        }



        I2Icon {
            id: _iconcCombo

            visible: style.isI2Icon

            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: style.iconMarginRight
            }

            rotation: (_comboButton.checked ? (openOnTop ? style.dropdownIconRotateIdle : style.dropdownIconRotateDowned)
                                            : (openOnTop ? style.dropdownIconRotateDowned : style.dropdownIconRotateIdle) )
            iconType: style.dropdownIcon
            color: (!_combobox.enabled ? _combobox.style.borderColorDisabled
                                       : (_mouseAreaCombo.pressed ? _combobox.style.borderColorPressed : _combobox.style.borderColorIdle) )

            Behavior on rotation {
                NumberAnimation {}
            }
        }

        Image {
            id: _imageCombo

            visible: !style.isI2Icon

            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: style.iconMarginRight
            }
            rotation: (_comboButton.checked ? (openOnTop ? style.dropdownIconRotateIdle : style.dropdownIconRotateDowned)
                                            : (openOnTop ? style.dropdownIconRotateDowned : style.dropdownIconRotateIdle) )

            source: !style.isI2Icon ? style.dropdownIcon : ""

            Behavior on rotation {
                NumberAnimation {}
            }
        }


        MouseArea {
            id: _mouseAreaCombo

            anchors.fill: parent

            activeFocusOnTab: true

            enabled: (_combolist.count !== 0)
            hoverEnabled: enabled

            onClicked: {
                _mouseAreaCombo.forceActiveFocus();
                if (_comboButton.checked) {
                    close();
                }
                else {
                    open();
                }
            }
        }

        Controls2.ToolTip {
            delay: 500
            visible: _mouseAreaCombo.containsMouse
            text: _comboText.text
        }

        onVisibleChanged: {
            if (!visible) {
                _combobox.close();
            }
        }
    }

    I2PopupBase {
        id: popup

        anchors.top: (!openOnTop ? _comboButton.bottom : undefined);
        anchors.bottom: (openOnTop ? _comboButton.top : undefined);

        width: _comboButton.width
        height: ((_combolist.count < _combobox.countDisplayItem) ? _combolist.count * _comboButton.height
                                                                 : (_combobox.countDisplayItem + 0.5) * _comboButton.height )


        isModal: true;
        layerColor: "transparent"
        layerObjectName: _combobox.style.layerObjectName;

        dismissOnOutsideTap: true
        enabled: popup.isOpened

        keepRelativePositionToInitialParent : true;

        onClosed: {
            _combobox.close();
        }

        /*Keys.onReturnPressed: {
            if (_comboButton.checked) {
                _combobox.close();
            }
            else {
                _combobox.open();
            }
        }*/


        ScrollView {
             id: _scrollView

             anchors {
                 top: parent.top
                 bottom: parent.bottom
             }
             width: _comboButton.width

             frameVisible: _combobox.style.frameVisible

             style: IngeScapeScrollViewStyle {
             }

             // Prevent drag overshoot on Windows
             //flickableItem.boundsBehavior: Flickable.OvershootBounds

             // Content of our scrollview
             ListView {
                id: _combolist

                width: parent.width

                boundsBehavior: Flickable.StopAtBounds

                model: 0

                currentIndex : -1

                delegate: Component {
                    Rectangle {
                        color:  (_mouseAreaItem.containsPress ? _combobox.style.listBackgroundColorPressed : ((_combolist.currentIndex === index) ? _combobox.style.listBackgroundColorSelected : _combobox.style.listBackgroundColorIdle));
                        height: _comboButton.height
                        width:  _comboButton.width

                        border.width: _combobox.style.listCellBorderWidth
                        border.color :_combobox.style.listCellBorderColor
                        radius: 0;

                        Text {
                            id: _itemText

                            anchors {
                                verticalCenter: parent.verticalCenter
                                left: parent.left
                                leftMargin : 10
                                right: parent.right
                            }

                            elide: Text.ElideRight
                            font: _combobox.style.itemsFont;

                            color: (_mouseAreaItem.containsPress ? _combobox.style.listItemTextColorPressed
                                                                 : ((_combolist.currentIndex === index) ? _combobox.style.listItemTextColorSelected : _combobox.style.listItemTextColorIdle));
                            text: if (!useQStringList) {
                                      modelToString(model);
                                  }
                                  else {
                                      modelData;
                                  }
                        }

                        MouseArea {
                            id: _mouseAreaItem

                            anchors.fill: parent

                            hoverEnabled: true

                            onClicked: {
                               close();

                                if (!useQStringList) {
                                    // Check if we have a simple ListModel OR a QAbstractListModel
                                    if (model.hasOwnProperty('QtObject'))
                                    {
                                        // Our model is a wrapper (QQmlDMAbstractItemModelData) and not the QObject
                                        // stored in our list
                                        // Thus, we must use a specific property to access our QObject, here 'QtObject'
                                        // that is the property defined by I2 classes derived from QAbstractListModel
                                        _combobox.selectedItem = model.QtObject;
                                    }
                                    else
                                    {
                                        // We can use our raw model
                                        _combobox.selectedItem = model;
                                    }
                                }
                                else {
                                    _combobox.selectedItem = modelData;
                                }
                            }

                            onExited: {
                                Tooltip.hideText();
                            }
                            onCanceled: {
                                Tooltip.hideText();
                            }

                            Timer {
                                interval: 400
                                running: _mouseAreaItem.containsMouse

                                onTriggered: {
                                    Tooltip.showText(_mouseAreaItem, Qt.point(_mouseAreaItem.mouseX, _mouseAreaItem.mouseY), _itemText.text);
                                }
                            }
                        }

                        // Induce bug with the trackpad (Window is no more interactive)
                        /*Controls2.ToolTip {
                            delay: 400
                            visible: _mouseAreaItem.containsMouse
                            text: _itemText.text
                        }*/

                    }
                }
            }
        }
    }
}

