#-------------------------------------------------
#
# Project created by QtCreator 2017-01-05T12:18:11
#
#-------------------------------------------------

win32:{
    TARGET = mastic
}

TEMPLATE = lib

DEFINES += MASTIC

win32:{
    DEFINES +=  _CRT_SECURE_NO_WARNINGS \
                _CRT_NONSTDC_NO_DEPRECATE
}

include ('../dependencies/mastic_api_crossplateform.pri')

