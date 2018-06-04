import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0
import INGESCAPE 1.0

I2Combobox {
    id: root
    height : 22
    width : 115

    style : I2ComboboxStyle {
        backgroundColorIdle : IngeScapeTheme.darkBlueGreyColor
        backgroundColorDisabled: IngeScapeTheme.darkBlueGreyColor
        borderWidthIdle : 0
        borderColorPressed: IngeScapeTheme.darkBlueGreyColor
        borderWidthPressed: 1
        currentTextColorIdle : IngeScapeTheme.whiteColor
        placeholderTextColor: IngeScapeTheme.greyColor
        currentTextColorDisabled:  IngeScapeTheme.greyColor
        backgroundColorPressed : IngeScapeTheme.darkGreyColor2
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


    scrollViewStyle: IngeScapeScrollViewStyle {
    }


}
