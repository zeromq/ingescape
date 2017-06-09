#-------------------------------------------------
#
# Project created by QtCreator 2017-01-05T12:18:11
#
#-------------------------------------------------

TARGET = mastic

#TEMPLATE = lib
CONFIG += console

DEFINES += MASTIC

win32:{
    DEFINES +=  _CRT_SECURE_NO_WARNINGS \
                _CRT_NONSTDC_NO_DEPRECATE \
                WINDOWS
}
#UNCOMMENT THIS LINE TO ANDROID BUILD OR COMMENT FOR ANOTHER UNIX PLATFORM
#CONFIG+=android_compilation

#UNCOMMENT THIS LINE TO RASPBERRY BUILD OR COMMENT FOR ANOTHER UNIX PLATFORM
#CONFIG+=raspberry_compilation


include ('../dependencies/mastic_api_crossplateform.pri')

