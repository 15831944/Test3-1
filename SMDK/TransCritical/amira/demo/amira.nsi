; example1.nsi
;
; This script is perhaps one of the simplest NSIs you can make. All of the
; optional settings are left to their default settings. The installer simply 
; prompts the user asking them where to install, and drops a copy of example1.nsi
; there. 

;--------------------------------

; The name of the installer
Name "MBayer"

; The file to write
OutFile "MBayer.exe"

; The default installation directory
InstallDir $PROGRAMFILES\MBayer
;--------------------------------

; Pages

Page directory
Page instfiles

;--------------------------------

; The stuff to install
Section "" ;No components page, name is not important

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File /r dist\*.*
  CreateShortCut $DESKTOP\MBayer.lnk $INSTDIR\MBayer.exe "" "$INSTDIR\mb.ico"
SectionEnd ; end the section
