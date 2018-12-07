;NSIS installation script for ingescape on windows

;NSIS plugins used here
!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "x64.nsh"

!define APPNAME "Ingescape"
!define COMPANYNAME "Ingenuity IO"
!define DESCRIPTION "Build software agents for your specific needs, using various operating systems"
!define HELPURL "https://ingescape.com/"
!define ABOUTURL "https://ingescape.com/"

##############################################
# General configuration
##############################################

;Installer name
Name "${APPNAME}"

;Name of the installer file
OutFile "${APPNAME}-Runtime-Installer.exe"

;Default install folder
InstallDir "$COMMONFILES\${APPNAME}"

;Get installation folder from registry if available
InstallDirRegKey HKCU "Software\${APPNAME}" ""

;Request application privileges for Windows Vista
RequestExecutionLevel admin

##############################################
# Interface Settings
##############################################

;define MUI_ICON ${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico
;define MUI_UNICON ${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico
;define MUI_HEADERIMAGE
;define MUI_HEADERIMAGE_BITMAP ${NSISDIR}\Contrib\Graphics\Header\nsis.bmp
;define MUI_HEADERIMAGE_UNBITMAP ${NSISDIR}\Contrib\Graphics\Header\nsis.bmp
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
!define MUI_FINISHPAGE_LINK_LOCATION "${HELPURL}"
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

!macro VerifyUserIsAdmin
UserInfo::GetAccountType
pop $0
${If} $0 != "admin" ;Require admin rights on NT4+
        messageBox mb_iconstop "Administrator rights required!"
        setErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
        quit
${EndIf}
!macroend

Function .onInit
	SetShellVarContext all
    System::Call 'kernel32::CreateMutex(p 0, i 0, t "IngescapeInstallerMutex") p .r1 ?e'
    Pop $R0
    
    StrCmp $R0 0 +3
    MessageBox MB_OK|MB_ICONEXCLAMATION "The installer is already running."
    Abort

    ${If} ${RunningX64}
        StrCpy $INSTDIR "$COMMONFILES64\${APPNAME}"
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

Section "Runtime dll" SecRuntime
    SectionIn RO

    ${If} ${RunningX64}
        SetOutPath $INSTDIR\x86_64
        File "../../win32-x86_64/*.dll"
    ${Else}
        SetOutPath $INSTDIR\i686
        File "../../win32-i686/*.dll"
    ${EndIf}
    SetOutPath $INSTDIR
    File /oname=logo.ico ../../editor/apps/IngeScape-Editor/icon_IGS.ico ;Actualy use in unstall property panel

    ;Store installation folder
    WriteRegStr HKCU "Software\${APPNAME}" "" $INSTDIR

    ;Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"


    # Registry information for add/remove programs
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayName" "${COMPANYNAME} - ${APPNAME} - ${DESCRIPTION}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "InstallLocation" "$\"$INSTDIR$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayIcon" "$\"$INSTDIR\logo.ico$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "Publisher" "$\"${COMPANYNAME}$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "HelpLink" "$\"${HELPURL}$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "URLInfoAbout" "$\"${ABOUTURL}$\""
    # There is no option for modifying or repairing the install
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "NoRepair" 1
SectionEnd

##############################################
# Descriptions
##############################################

;Language strings
LangString DESC_SecRuntime ${LANG_ENGLISH} "All dll needed to run agents using ingescape"

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SecRuntime} $(DESC_SecRuntime)
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
