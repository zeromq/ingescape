;NSIS installation script for ingescape on windows
; Some, but not all, commands are added in comments to be able to test it quickly

;NSIS plugins used here
!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "x64.nsh"

!define APPNAME "Ingescape"
!define COMPANYNAME "Ingenuity IO"
!define ABOUTURL "https://ingescape.com/"

##############################################
# General configuration
##############################################

;Installer name
Name "${APPNAME}"

;Name of the installer file
OutFile "${APPNAME}-Sdk-Installer.exe"

;Default install folder
InstallDir "$COMMONFILES\${APPNAME}"

;Get installation folder from registry if available
InstallDirRegKey HKCU "Software\${APPNAME}" ""

;Request application privileges for Windows Vista
RequestExecutionLevel admin

##############################################
# Interface Settings
##############################################

;!define MUI_ICON ${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico
;!define MUI_UNICON ${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico
;!define MUI_HEADERIMAGE
;!define MUI_HEADERIMAGE_BITMAP ${NSISDIR}\Contrib\Graphics\Header\nsis.bmp
;!define MUI_HEADERIMAGE_UNBITMAP ${NSISDIR}\Contrib\Graphics\Header\nsis.bmp
;!define MUI_WELCOMEFINISHPAGE_BITMAP icons/logo-1.bmp
;!define MUI_WELCOMEFINISHPAGE_BITMAP_NOSTRETCH
;!define MUI_UNWELCOMEFINISHPAGE_BITMAP icons/logo-1.bmp
;!define MUI_UNWELCOMEFINISHPAGE_BITMAP_NOSTRETCH
;!define MUI_COMPONENTSPAGE_CHECKBITMAP ${NSISDIR}\Contrib\Graphics\Checks\modern.bmp
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_UNFINISHPAGE_NOAUTOCLOSE
!define MUI_ABORTWARNING
;!define MUI_ABORTWARNING_TEXT "Good bye ?"
!define MUI_UNABORTWARNING
;!define MUI_UNABORTWARNING_TEXT "Good bye ?"


##############################################
# Pages
##############################################

;!define MUI_PAGE_HEADER_TEXT "test"
;!define MUI_PAGE_HEADER_SUBTEXT "test"

;!define MUI_WELCOMEPAGE_TITLE "test"
;!define MUI_WELCOMEPAGE_TEXT "test"
!insertmacro MUI_PAGE_WELCOME

; TODO Add license
;!insertmacro MUI_PAGE_LICENSE "./License.txt"

;!define MUI_COMPONENTSPAGE_TEXT_TOP "text"
;!define MUI_COMPONENTSPAGE_TEXT_DESCRIPTION_TITLE "text"
!insertmacro MUI_PAGE_COMPONENTS

;!define MUI_DIRECTORYPAGE_TEXT_TOP "text"
!insertmacro MUI_PAGE_DIRECTORY

; TODO Add uninstall link in start menu ? 
;!insertmacro MUI_PAGE_STARTMENU "pageid" $StartMenuFolder

!insertmacro MUI_PAGE_INSTFILES

;!define MUI_FINISHPAGE_TITLE "text"
;!define MUI_FINISHPAGE_TEXT "text"
!define MUI_FINISHPAGE_LINK "Visit ${APPNAME} website"
!define MUI_FINISHPAGE_LINK_LOCATION "${ABOUTURL}"
!define MUI_FINISHPAGE_NOREBOOTSUPPORT
!insertmacro MUI_PAGE_FINISH


!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
;!insertmacro MUI_UNPAGE_LICENSE "${NSISDIR}\Docs\Modern UI\License.txt"
;!insertmacro MUI_UNPAGE_COMPONENTS
;!insertmacro MUI_UNPAGE_DIRECTORY
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

##############################################
# Languages
##############################################

!insertmacro MUI_LANGUAGE "English"

##############################################
# Installer Sections
##############################################

; Check user right
!macro VerifyUserIsAdmin
UserInfo::GetAccountType
pop $0
${If} $0 != "admin" ;Require admin rights on NT4+
        messageBox mb_iconstop "Administrator rights required!"
        setErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
        quit
${EndIf}
!macroend

####################
# Runtime Sections #
####################

Function RegisterInstallerInRegistry
    SetOutPath $INSTDIR
    File /oname=logo.ico ../../editor/apps/IngeScape-Editor/icon_IGS.ico ;Actualy use in unstall property panel

    ;Store installation folder
    WriteRegStr HKCU "Software\${APPNAME}" "" $INSTDIR

    ;Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"

    # Registry information for add/remove programs
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayName" "${APPNAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "InstallLocation" "$\"$INSTDIR$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayIcon" "$\"$INSTDIR\logo.ico$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "Publisher" "$\"${COMPANYNAME}$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "URLInfoAbout" "$\"${ABOUTURL}$\""
    # There is no option for modifying or repairing the install
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "NoRepair" 1
FunctionEnd

Section "Runtime dll 64 bits" SecRuntime64
    ${If} ${RunningX64}
        Call RegisterInstallerInRegistry
    ${EndIf}
    
    SetOutPath $INSTDIR\x86_64
    File "../../win32-x86_64/*.dll"
SectionEnd

Section "Runtime dll 32 bits" SecRuntime32
    ${IfNot} ${RunningX64}
        Call RegisterInstallerInRegistry
    ${EndIf}

    SetOutPath $INSTDIR\i686
    File "../../win32-i686/*.dll"
SectionEnd

####################
# SDK Sections     #
####################

Function ExportIngescapeHeader
    SetOutPath $INSTDIR\include\ingescape
    File "../../src/include/ingescape.h"
FunctionEnd

Function ExportCzmqHeader
    ; Even for x86_64 do not need to search header in x86 folder because header are the same
    SetOutPath $INSTDIR\include
    File "/usr/i686-w64-mingw32/include/czmq.h"
    File "/usr/i686-w64-mingw32/include/czmq_library.h"
    File "/usr/i686-w64-mingw32/include/czmq_prelude.h"
    File "/usr/i686-w64-mingw32/include/zactor.h"
    File "/usr/i686-w64-mingw32/include/zarmour.h"
    File "/usr/i686-w64-mingw32/include/zcert.h"
    File "/usr/i686-w64-mingw32/include/zcertstore.h"
    File "/usr/i686-w64-mingw32/include/zchunk.h"
    File "/usr/i686-w64-mingw32/include/zclock.h"
    File "/usr/i686-w64-mingw32/include/zconfig.h"
    File "/usr/i686-w64-mingw32/include/zdigest.h"
    File "/usr/i686-w64-mingw32/include/zdir.h"
    File "/usr/i686-w64-mingw32/include/zdir_patch.h"
    File "/usr/i686-w64-mingw32/include/zfile.h"
    File "/usr/i686-w64-mingw32/include/zframe.h"
    File "/usr/i686-w64-mingw32/include/zhash.h"
    File "/usr/i686-w64-mingw32/include/zhashx.h"
    File "/usr/i686-w64-mingw32/include/ziflist.h"
    File "/usr/i686-w64-mingw32/include/zlist.h"
    File "/usr/i686-w64-mingw32/include/zlistx.h"
    File "/usr/i686-w64-mingw32/include/zloop.h"
    File "/usr/i686-w64-mingw32/include/zmsg.h"
    File "/usr/i686-w64-mingw32/include/zpoller.h"
    File "/usr/i686-w64-mingw32/include/zsock.h"
    File "/usr/i686-w64-mingw32/include/zstr.h"
    File "/usr/i686-w64-mingw32/include/zsys.h"
    File "/usr/i686-w64-mingw32/include/zuuid.h"
    File "/usr/i686-w64-mingw32/include/zauth.h"
    File "/usr/i686-w64-mingw32/include/zbeacon.h"
    File "/usr/i686-w64-mingw32/include/zgossip.h"
    File "/usr/i686-w64-mingw32/include/zmonitor.h"
    File "/usr/i686-w64-mingw32/include/zproxy.h"
    File "/usr/i686-w64-mingw32/include/zrex.h"
FunctionEnd

Function ExportZmqHeader
    ; Even for x86_64 do not need to search header in x86 folder because header are the same
    SetOutPath $INSTDIR\include
    File "/usr/i686-w64-mingw32/include/zmq.h"
    File "/usr/i686-w64-mingw32/include/zmq_utils.h"
    File "/usr/i686-w64-mingw32/include/sodium.h"
    File /r "/usr/i686-w64-mingw32/include/sodium"
FunctionEnd

Function ExportZyreHeader
    ; Even for x86_64 do not need to search header in x86 folder because header are the same
    SetOutPath $INSTDIR\include
    File "/usr/i686-w64-mingw32/include/zyre.h"
    File "/usr/i686-w64-mingw32/include/zyre_event.h"
    File "/usr/i686-w64-mingw32/include/zyre_library.h"
FunctionEnd

Function ExportYajlHeader
    ; Even for x86_64 do not need to search header in x86 folder because header are the same
    SetOutPath $INSTDIR\include
    File /r "/usr/i686-w64-mingw32/include/yajl"
FunctionEnd

SectionGroup "64 bits SDK" Group64Bits
    Section "SDK Ingescape" SecIngescapeSdk64
        Call ExportIngescapeHeader

        SetOutPath $INSTDIR\x86_64
        File "../../win32-x86_64/libingescape.dll.a"
        File "../../win32-x86_64/libingescape.def"
    SectionEnd

    Section "SDK CZmq" SecCZmqSdk64
        Call ExportCzmqHeader

        SetOutPath $INSTDIR\x86_64
        File "/usr/x86_64-w64-mingw32/lib/libczmq.dll.a"
    SectionEnd

    Section "SDK Zmq" SecZmqSdk64
        Call ExportZmqHeader

        SetOutPath $INSTDIR\x86_64
        File "/usr/x86_64-w64-mingw32/lib/libzmq.dll.a"
        File "/usr/x86_64-w64-mingw32/lib/libsodium.dll.a"
    SectionEnd

    Section "SDK Zyre" SecZyreSdk64
        Call ExportZyreHeader

        SetOutPath $INSTDIR\x86_64
        File "/usr/x86_64-w64-mingw32/lib/libzyre.dll.a"
    SectionEnd

    Section "SDK Yajl" SecYajlSdk64
        Call ExportYajlHeader

        SetOutPath $INSTDIR\x86_64
        File "/usr/x86_64-w64-mingw32/lib/libyajl.dll.a"
    SectionEnd
SectionGroupEnd

SectionGroup "32 bits SDK" Group32Bits
    Section "SDK Ingescape" SecIngescapeSdk32
        Call ExportIngescapeHeader

        SetOutPath $INSTDIR\i686
        File "../../win32-i686/libingescape.dll.a"
        File "../../win32-i686/libingescape.def"
    SectionEnd

    Section "SDK CZmq" SecCZmqSdk32
        Call ExportCzmqHeader

        SetOutPath $INSTDIR\i686
        File "/usr/i686-w64-mingw32/lib/libczmq.dll.a"
    SectionEnd

    Section "SDK Zmq" SecZmqSdk32
        Call ExportZmqHeader

        SetOutPath $INSTDIR\i686
        File "/usr/i686-w64-mingw32/lib/libzmq.dll.a"
        File "/usr/i686-w64-mingw32/lib/libsodium.dll.a"
    SectionEnd

    Section "SDK Zyre" SecZyreSdk32
        Call ExportZyreHeader

        SetOutPath $INSTDIR\i686
        File "/usr/i686-w64-mingw32/lib/libzyre.dll.a"
    SectionEnd

    Section "SDK Yajl" SecYajlSdk32
        Call ExportYajlHeader

        SetOutPath $INSTDIR\i686
        File "/usr/i686-w64-mingw32/lib/libyajl.dll.a"
    SectionEnd
SectionGroupEnd

Function .onInit
	SetShellVarContext all
    System::Call 'kernel32::CreateMutex(p 0, i 0, t "IngescapeInstallerMutex") p .r1 ?e'
    Pop $R0
    
    StrCmp $R0 0 +3
    MessageBox MB_OK|MB_ICONEXCLAMATION "The installer is already running."
    Abort

    ${If} ${RunningX64}
        SectionSetFlags ${SecRuntime64} 17 ; 17 is SF_SELECTED & SF_RO
        StrCpy $INSTDIR "$COMMONFILES64\${APPNAME}"
    ${Else}
        SectionSetFlags ${SecRuntime32} 17 ; 17 is SF_SELECTED & SF_RO
    ${EndIf}

	!insertmacro VerifyUserIsAdmin
FunctionEnd

Function un.onInit
	SetShellVarContext all
 
	#Verify the uninstaller - last chance to back out
	MessageBox MB_OKCANCEL "Permanantly remove ${APPNAME}?" IDOK next
		Abort
	next:

	!insertmacro VerifyUserIsAdmin
FunctionEnd

##############################################
# Descriptions
##############################################

;Language strings
LangString DESC_Group64Bits ${LANG_ENGLISH} "Available SDK for 64 bits OS"
LangString DESC_Group32Bits ${LANG_ENGLISH} "Available SDK for 32 bits OS"
LangString DESC_SecRuntime64 ${LANG_ENGLISH} "All dll needed to run agents using ingescape in 64 bits OS"
LangString DESC_SecRuntime32 ${LANG_ENGLISH} "All dll needed to run agents using ingescape in 32 bits OS"
LangString DESC_SecIngescapeSdk64 ${LANG_ENGLISH} "Ingescape header and linkable library for 64 bits OS"
LangString DESC_SecIngescapeSdk32 ${LANG_ENGLISH} "Ingescape header and linkable library for 32 bits OS"
LangString DESC_SecCZmqSdk64 ${LANG_ENGLISH} "CZmq header and linkable library for 64 bits OS"
LangString DESC_SecCZmqSdk32 ${LANG_ENGLISH} "CZmq header and linkable library for 32 bits OS"
LangString DESC_SecZmqSdk64 ${LANG_ENGLISH} "Zmq header and linkable library for 64 bits OS"
LangString DESC_SecZmqSdk32 ${LANG_ENGLISH} "Zmq header and linkable library for 32 bits OS"
LangString DESC_SecZyreSdk64 ${LANG_ENGLISH} "Zyre header and linkable library for 64 bits OS"
LangString DESC_SecZyreSdk32 ${LANG_ENGLISH} "Zyre header and linkable library for 32 bits OS"
LangString DESC_SecYajlSdk64 ${LANG_ENGLISH} "Yajl header and linkable library for 64 bits OS"
LangString DESC_SecYajlSdk32 ${LANG_ENGLISH} "Yajl header and linkable library for 32 bits OS"

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${Group64Bits} $(DESC_Group64Bits)
!insertmacro MUI_DESCRIPTION_TEXT ${Group32Bits} $(DESC_Group32Bits)
!insertmacro MUI_DESCRIPTION_TEXT ${SecRuntime64} $(DESC_SecRuntime64)
!insertmacro MUI_DESCRIPTION_TEXT ${SecRuntime32} $(DESC_SecRuntime32)
!insertmacro MUI_DESCRIPTION_TEXT ${SecIngescapeSdk64} $(DESC_SecIngescapeSdk64)
!insertmacro MUI_DESCRIPTION_TEXT ${SecIngescapeSdk32} $(DESC_SecIngescapeSdk32)
!insertmacro MUI_DESCRIPTION_TEXT ${SecCZmqSdk64} $(DESC_SecCZmqSdk64)
!insertmacro MUI_DESCRIPTION_TEXT ${SecCZmqSdk32} $(DESC_SecCZmqSdk32)
!insertmacro MUI_DESCRIPTION_TEXT ${SecZmqSdk64} $(DESC_SecZmqSdk64)
!insertmacro MUI_DESCRIPTION_TEXT ${SecZmqSdk32} $(DESC_SecZmqSdk32)
!insertmacro MUI_DESCRIPTION_TEXT ${SecZyreSdk64} $(DESC_SecZyreSdk64)
!insertmacro MUI_DESCRIPTION_TEXT ${SecZyreSdk32} $(DESC_SecZyreSdk32)
!insertmacro MUI_DESCRIPTION_TEXT ${SecYajlSdk64} $(DESC_SecYajlSdk64)
!insertmacro MUI_DESCRIPTION_TEXT ${SecYajlSdk32} $(DESC_SecYajlSdk32)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

##############################################
# Uninstaller Section
##############################################

Section "Uninstall"

    Delete "$INSTDIR\Uninstall.exe"

    RMDir /r "$INSTDIR"

    DeleteRegKey /ifempty HKCU "Software\Ingescape"

    # Remove uninstaller information from the registry
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}"
SectionEnd
