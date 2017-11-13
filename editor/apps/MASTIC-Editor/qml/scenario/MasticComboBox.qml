import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0
import MASTIC 1.0

I2Combobox {
    id : root
    height : 22
    width : 115



    style : I2ComboboxStyle {
        backgroundColorIdle : MasticTheme.darkBlueGreyColor
        borderWidthIdle : 0
        borderColorPressed: MasticTheme.darkBlueGreyColor
        borderWidthPressed: 1
        currentTextColorIdle : MasticTheme.lightGreyColor
        backgroundColorPressed : MasticTheme.darkGreyColor2
        radius: 1

        font {
            family : MasticTheme.textFontFamily
            pixelSize : 15
        }

        listBackgroundColorIdle : backgroundColorIdle
        listItemTextColorIdle : MasticTheme.lightGreyColor
        frameVisible : false
        listRadius: 0
        listItemTextColorSelected : MasticTheme.whiteColor
        listBackgroundColorPressed : MasticTheme.darkOrangeColor2

        isI2Icon : false
        dropdownIcon : "image://I2svg/resources/SVG/mastic-pictos.svg#iconCombo"
        iconMarginRight : 6

        layerObjectName : "overlayLayer2"
    }

}
