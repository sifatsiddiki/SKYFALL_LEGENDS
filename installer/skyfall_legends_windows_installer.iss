; Skyfall Legends Windows Installer (Inno Setup)
; This script assumes you already built SkyfallLegends.exe (x64)
; from the C++/SFML project (e.g. with Visual Studio / MinGW).

#define MyAppName "Skyfall Legends"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "Skyfall Legends Team"
#define MyAppExeName "SkyfallLegends.exe"

[Setup]
AppId={{D9F76055-0D75-4A4B-9C48-1F9D2F4E5B77}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={pf64}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableDirPage=no
DisableProgramGroupPage=no
OutputDir=.
OutputBaseFilename=SkyfallLegends_Setup
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64
WizardStyle=modern
SetupLogging=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; GroupDescription: "Additional icons:"; Flags: unchecked

[Files]
; Main game executable (pre-built)
Source: "..\build\SkyfallLegends.exe"; DestDir: "{app}"; Flags: ignoreversion

; SFML and other runtime DLLs, if needed – place them next to the exe in build\ and list here
; Example:
; Source: "..\build\sfml-graphics-2.dll"; DestDir: "{app}"; Flags: ignoreversion
; Source: "..\build\sfml-window-2.dll";   DestDir: "{app}"; Flags: ignoreversion
; Source: "..\build\sfml-system-2.dll";   DestDir: "{app}"; Flags: ignoreversion
; Source: "..\build\sfml-audio-2.dll";    DestDir: "{app}"; Flags: ignoreversion

; Assets – textures, sounds, fonts, music
Source: "..\assets\*"; DestDir: "{app}\assets"; Flags: recursesubdirs ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\Uninstall {#MyAppName}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "Launch {#MyAppName}"; Flags: nowait postinstall skipifsilent
