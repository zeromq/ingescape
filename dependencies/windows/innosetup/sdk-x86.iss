; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!
#define MyAppName "IngeScape SDK x86"
#define MyAppVersion "0.8.1"
#define MyAppPublisher "Ingenuity I/O, Inc."
#define MyAppURL "http://www.ingescape.com/"
#define DestinationFolderName "Ingescape"
#define SetupName "igs_sdk.x86"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{F6C2D2B5-0441-4A79-AFF8-3F2831CEB30E}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#DestinationFolderName}
DisableDirPage=yes
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
OutputBaseFilename={#SetupName}
Compression=lzma
SolidCompression=yes
; Tell Windows Explorer to reload the environment
ChangesEnvironment=yes
;Icon to display for the Uninstall entry in the Add/Remove Programs Control Panel applet 
UninstallDisplayIcon={app}\icon.ico

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"

[Files]
; redist = release libs
Source: "C:\ingescape\libs\release\*"; DestDir: "{app}\libs"; Flags: ignoreversion
; headers
Source: "C:\ingescape\include\*"; DestDir: "{app}\include\ingescape"; Flags: ignoreversion
;icon file
Source: "..\..\..\applications\apps\IngeScape-Editor\icon.ico"; DestDir: "{app}"; Flags: ignoreversion
;TODO : add samples

;TODO : add documentation

; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Registry]
; Key redist
Root: HKCU; Subkey: "Environment"; ValueType:string; ValueName: "IGS_LIBS_PATH"; \
    ValueData: "{app}\libs"; Flags: preservestringtype uninsdeletevalue ; 
  ; Key header
Root: HKCU; Subkey: "Environment"; ValueType:string; ValueName: "IGS_INCLUDE_PATH"; \
    ValueData: "{app}\include"; Flags: preservestringtype uninsdeletevalue ; 