!define softver "3.0.1"
!define publisher "M1Maker"
!define sname "NGT"
!define lname "New Game Toolkit"
!include "mui2.nsh"
!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING
Name "${sname}"
; The file to write
OutFile "${sname}.exe"
; Request application privileges for Windows Vista and higher
RequestExecutionLevel admin

; Build Unicode installer
Unicode True

brandingtext "${sname} (${lname}) installer ${softver}"

Function .onInit
SetRegView 64
FunctionEnd

; The default installation directory
InstallDir "$PROGRAMFILES64\\${sname}"
; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\${sname}" "Install_Dir"

;--------------------------------

; Pages

!insertmacro mui_page_welcome

!insertmacro mui_Page_components
!insertmacro mui_Page_directory
!insertmacro mui_Page_instfiles
!insertmacro mui_page_finish
!insertmacro mui_unpage_welcome
!insertmacro mui_unpage_confirm
!insertmacro mui_unpage_components
!insertmacro mui_unpage_instfiles
!insertmacro mui_unpage_finish
!insertmacro mui_language "english"
;--------------------------------

; The stuff to install
Section "${sname} (required)"

SectionIn RO

; Set output path to the installation directory.
SetOutPath $INSTDIR

; Put file there
File /r "build\*"

; Write the installation path into the registry

WriteRegStr HKLM SOFTWARE\${sname} "Install_Dir" "$INSTDIR"

; Write the uninstall keys for Windows
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${sname}" "DisplayName" "${sname}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${sname}" "Publisher" "${publisher}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${sname}" "UninstallString" '"$INSTDIR\uninstall.exe"'
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${sname}" "NoModify" 1
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${sname}" "NoRepair" 1
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${sname}" "DisplayVersion" "${softver}"

WriteUninstaller "$INSTDIR\uninstall.exe"

; Add registry entries for file association
WriteRegStr HKCR ".ngt" "" "NGT-File"
WriteRegStr HKCR "NGT-File" "" "NGT executable"
WriteRegStr HKCR "NGT-File\shell\Open" "" "Open with NGT Script"
WriteRegStr HKCR "NGT-File\shell\Open\command" "" '"$INSTDIR\NGTW.exe" -r "%1"'
WriteRegStr HKCR "NGT-File\shell\Compile" "" "Compile NGT Script"
WriteRegStr HKCR "NGT-File\shell\Compile\command" "" '"$INSTDIR\NGTW.exe" -c "%1"'
WriteRegStr HKCR "NGT-File\shell\Run" "" "Run NGT Script"
WriteRegStr HKCR "NGT-File\shell\Run\command" "" '"$INSTDIR\NGTW.exe" -r "%1"'
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

CreateDirectory "$SMPROGRAMS\${sname}"
CreateShortcut "$SMPROGRAMS\${sname}\Uninstall.lnk" "$INSTDIR\uninstall.exe"

SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
; Remove registry keys
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${sname}"
DeleteRegKey HKLM SOFTWARE\${sname}
DeleteRegKey HKCR ".ngt"
DeleteRegKey HKCR "NGT-File\shell\Open"
DeleteRegKey HKCR "NGT-File\shell\Compile"
DeleteRegKey HKCR "NGT-File\shell\Run"
; Remove shortcuts, if any
Delete "$SMPROGRAMS\${sname}\*.lnk"

; Remove directories
RMDir /r "$SMPROGRAMS\${sname}"
RMDir /r "$INSTDIR"

SectionEnd