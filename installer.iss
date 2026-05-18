[Setup]
AppName=TSReaderPro
AppVersion=2.8.53h-memorial
AppPublisher=COOL.STF
AppPublisherURL=https://github.com/TSReader/TSReader
DefaultDirName={commonpf32}\TSReaderPro
DefaultGroupName=TSReaderPro
OutputDir={#SourcePath}\installer_output
OutputBaseFilename=TSReaderPro_Setup
Compression=lzma2
SolidCompression=yes
SetupIconFile={#SourcePath}\dvb.ico
UninstallDisplayIcon={app}\dvb.ico
ArchitecturesAllowed=x86compatible
WizardStyle=modern
LicenseFile={#SourcePath}\LICENSE
PrivilegesRequired=admin
DisableDirPage=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Main executable
Source: "{#SourcePath}\build\Release\TSReaderPro.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourcePath}\TSReaderPro.exe.manifest"; DestDir: "{app}"; DestName: "TSReaderPro.exe.manifest"; Flags: ignoreversion

; Core DLLs
Source: "{#SourcePath}\build\Release\libfaad2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourcePath}\build\Release\PEGRPCS.DLL"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourcePath}\build\Release\TSReader_SourceHelper.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourcePath}\_ISource.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourcePath}\pthreadVSE2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourcePath}\adpsi30.dll"; DestDir: "{app}"; Flags: ignoreversion

; Application plugin DLLs
Source: "{#SourcePath}\TSReader_MPEG4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourcePath}\TSReader_ForVid.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourcePath}\TSReader_Scheduler.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourcePath}\TSReader_VLC.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourcePath}\TSReader_UDPSender.dll"; DestDir: "{app}"; Flags: ignoreversion

; Helper executables
Source: "{#SourcePath}\vc1-decoder.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourcePath}\TSReader_ArchiveMonitor.exe"; DestDir: "{app}"; Flags: ignoreversion

; Bitmaps (all)
Source: "{#SourcePath}\*.bmp"; DestDir: "{app}"; Flags: ignoreversion

; PNG images
Source: "{#SourcePath}\*.png"; DestDir: "{app}"; Flags: ignoreversion

; Audio
Source: "{#SourcePath}\archive-siren.wav"; DestDir: "{app}"; Flags: ignoreversion

; INI config files (root) — user-writable so tune dialogs can persist settings under Program Files
Source: "{#SourcePath}\BISS.ini"; DestDir: "{app}"; Flags: ignoreversion; Permissions: users-modify
Source: "{#SourcePath}\diseqcU.ini"; DestDir: "{app}"; Flags: ignoreversion; Permissions: users-modify
Source: "{#SourcePath}\dvbt.ini"; DestDir: "{app}"; Flags: ignoreversion; Permissions: users-modify
Source: "{#SourcePath}\S2emu.ini"; DestDir: "{app}"; Flags: ignoreversion; Permissions: users-modify

; Data files
Source: "{#SourcePath}\ndscam.dat"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourcePath}\dvb.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourcePath}\LICENSE"; DestDir: "{app}"; DestName: "LICENSE.txt"; Flags: ignoreversion


; Memorial splash image
Source: "{#SourcePath}\rod_splash.png"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist
Source: "{#SourcePath}\rod_splash.jpg"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist
; List files (source presets) — user-writable so the tune dialog can persist new entries
Source: "{#SourcePath}\*.lst"; DestDir: "{app}"; Flags: ignoreversion; Permissions: users-modify

; Sample .tmc files
Source: "{#SourcePath}\*.tmc"; DestDir: "{app}"; Flags: ignoreversion

; FreeSat tables
Source: "{#SourcePath}\freesat.t1"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist
Source: "{#SourcePath}\freesat.t2"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist

; Source plugin DLLs
Source: "{#SourcePath}\Sources\*.dll"; DestDir: "{app}\Sources"; Flags: ignoreversion

; Source plugin DLLs from Rod's posthumous archive (2019-2022 builds, additive — no overlap with Sources\)
Source: "{#SourcePath}\Sources_Archive\*.dll"; DestDir: "{app}\Sources"; Flags: ignoreversion

; Forwarder DLLs
Source: "{#SourcePath}\Forwarders\*.dll"; DestDir: "{app}\Forwarders"; Flags: ignoreversion

; Satellite INI files — user-writable so the satellite editor can save new transponders
Source: "{#SourcePath}\Satellites\*.ini"; DestDir: "{app}\Satellites"; Flags: ignoreversion; Permissions: users-modify

[Icons]
Name: "{group}\TSReaderPro"; Filename: "{app}\TSReaderPro.exe"; IconFilename: "{app}\dvb.ico"; WorkingDir: "{app}"
Name: "{group}\Uninstall TSReaderPro"; Filename: "{uninstallexe}"
Name: "{autodesktop}\TSReaderPro"; Filename: "{app}\TSReaderPro.exe"; IconFilename: "{app}\dvb.ico"; WorkingDir: "{app}"; Tasks: desktopicon

[Run]
Filename: "{app}\TSReaderPro.exe"; Description: "Launch TSReaderPro"; Flags: nowait postinstall skipifsilent
