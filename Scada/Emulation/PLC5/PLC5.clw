; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CPLC5Dlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "plc5.h"
LastPage=0

ClassCount=4
Class1=CPLC5App
Class2=CAboutDlg
Class3=CPLC5Dlg

ResourceCount=10
Resource1=IDD_PLC5_DIALOG
Resource2=IDR_MENU1 (English (U.S.))
Resource3=IDD_PLC5_DIALOG (English (U.S.))
Resource4=IDR_MENU1
Resource5=IDR_ACCELERATOR1
Resource6=IDD_OPENPRJ_DLG (English (U.S.))
Class4=COpenDlg
Resource7=IDD_OPENPRJ_DLG
Resource8=IDD_ABOUTBOX
Resource9=IDD_ABOUTBOX (English (U.S.))
Resource10=IDR_ACCELERATOR1 (English (U.S.) (unknown sub-lang: 0x7))

[CLS:CPLC5App]
Type=0
BaseClass=CWinApp
HeaderFile=PLC5.h
ImplementationFile=PLC5.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=PLC5Dlg.cpp
ImplementationFile=PLC5Dlg.cpp

[CLS:CPLC5Dlg]
Type=0
BaseClass=CDialog
HeaderFile=PLC5Dlg.h
ImplementationFile=PLC5Dlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=CPLC5Dlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_PLC5_DIALOG]
Type=1
Class=CPLC5Dlg
ControlCount=35
Control1=IDC_TREE,SysTreeView32,1350635559
Control2=IDC_LDRSCROLLBAR,scrollbar,1342242817
Control3=IDC_LDRSTATIC,static,1342242825
Control4=IDC_DATASCROLLBAR,scrollbar,1342242817
Control5=IDC_DATASTATIC,static,1342242825
Control6=IDC_TBHEADER,static,1342308352
Control7=IDC_LDHEADER,static,1342308352
Control8=IDC_STATE,static,1342308864
Control9=IDC_RADIODEC,button,1342308361
Control10=IDC_RADIOHEX,button,1342177289
Control11=IDC_STATIC,static,1342308864
Control12=IDC_EDITITER,edit,1350631552
Control13=IDC_STATUSMSG,static,1342308864
Control14=IDC_EDITGOTO,edit,1350631552
Control15=IDC_STATIC,static,1342308354
Control16=IDC_REMOTE,button,1342246915
Control17=IDC_STATIC,button,1342177287
Control18=IDC_RESET,button,1342246915
Control19=IDC_STATIC,static,1342308864
Control20=IDC_EDITUPDATE,edit,1350631552
Control21=IDC_STATIC,static,1342308866
Control22=IDC_FINDINLADDER,button,1342242816
Control23=IDC_STATIC,static,1342308864
Control24=IDC_EDITSTEP,edit,1350631552
Control25=IDC_STATIC,static,1342308864
Control26=IDC_EDITRUNMULT,edit,1350631552
Control27=IDC_EXECTIME,static,1342308864
Control28=IDC_UPDTIME,static,1342308864
Control29=IDSTOP,button,1342250883
Control30=IDRUN,button,1342250883
Control31=IDSTEP,button,1342246784
Control32=IDC_GOTORUN,button,1342242816
Control33=IDC_GOTOSTOP,button,1342242816
Control34=IDC_GOTOIDLE,button,1342242816
Control35=IDC_CHANGE_MONITOR,button,1342242819

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_PLC5_DIALOG (English (U.S.))]
Type=1
Class=CPLC5Dlg
ControlCount=34
Control1=IDC_TREE,SysTreeView32,1350635559
Control2=IDC_LDRSCROLLBAR,scrollbar,1342242817
Control3=IDC_LDRSTATIC,static,1342242825
Control4=IDC_DATASCROLLBAR,scrollbar,1342242817
Control5=IDC_DATASTATIC,static,1342242825
Control6=IDC_TBHEADER,static,1342308352
Control7=IDC_LDHEADER,static,1342308352
Control8=IDC_STATE,static,1342308864
Control9=IDC_RADIODEC,button,1342308361
Control10=IDC_RADIOHEX,button,1342177289
Control11=IDC_STATIC,static,1342308864
Control12=IDC_EDITITER,edit,1350631552
Control13=IDC_STATUSMSG,static,1342308864
Control14=IDC_EDITGOTO,edit,1350631552
Control15=IDC_STATIC,static,1342308354
Control16=IDC_REMOTE,button,1342246915
Control17=IDC_STATIC,button,1342177287
Control18=IDC_RESET,button,1342246915
Control19=IDC_STATIC,static,1342308864
Control20=IDC_EDITUPDATE,edit,1350631552
Control21=IDC_STATIC,static,1342308866
Control22=IDC_FINDINLADDER,button,1342242816
Control23=IDC_STATIC,static,1342308864
Control24=IDC_EDITSTEP,edit,1350631552
Control25=IDC_STATIC,static,1342308864
Control26=IDC_EDITRUNMULT,edit,1350631552
Control27=IDC_EXECTIME,static,1342308864
Control28=IDC_UPDTIME,static,1342308864
Control29=IDSTOP,button,1342250883
Control30=IDRUN,button,1342250883
Control31=IDSTEP,button,1342246784
Control32=IDC_GOTORUN,button,1342242816
Control33=IDC_GOTOSTOP,button,1342242816
Control34=IDC_GOTOIDLE,button,1342242816

[ACL:IDR_ACCELERATOR1]
Type=1
Class=?
Command1=IDSTEP
CommandCount=1

[ACL:IDR_ACCELERATOR1 (English (U.S.) (unknown sub-lang: 0x7))]
Type=1
Class=?
Command1=IDSTEP
CommandCount=1

[DLG:IDD_OPENPRJ_DLG]
Type=1
Class=COpenDlg
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_PRJFOLDER,edit,1350631552
Control5=IDC_OPENDBGFILE,button,1342242819
Control6=IDBROWSE,button,1342242816
Control7=IDC_STATIC,static,1342308352

[CLS:COpenDlg]
Type=0
HeaderFile=Dlgs.h
ImplementationFile=Dlgs.cpp
BaseClass=CDialog
Filter=D
LastObject=IDBROWSE
VirtualFilter=dWC

[MNU:IDR_MENU1]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_SAVESNAP
Command3=ID_LOADSNAP
Command4=ID_SAVEBACK
Command5=ID_LOADBACK
Command6=IDC_GOTORUN
Command7=IDC_GOTOSTOP
Command8=IDC_GOTOIDLE
CommandCount=8

[DLG:IDD_OPENPRJ_DLG (English (U.S.))]
Type=1
Class=?
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_PRJFOLDER,edit,1350631552
Control5=IDC_OPENDBGFILE,button,1342242819
Control6=IDBROWSE,button,1342242816

[MNU:IDR_MENU1 (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_SAVESNAP
Command3=ID_LOADSNAP
Command4=ID_SAVEBACK
Command5=ID_LOADBACK
Command6=IDC_GOTORUN
Command7=IDC_GOTOSTOP
Command8=IDC_GOTOIDLE
CommandCount=8

