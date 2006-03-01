; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CKwaDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "kwa.h"

ClassCount=3
Class1=CKwaApp
Class2=CKwaDlg
Class3=CAboutDlg

ResourceCount=17
Resource1=IDD_KWA_DIALOG
Resource2=IDR_MAINFRAME
Resource3=IDD_KWA_DIALOG (English (U.S.))
Resource4=IDD_CK_STARTDLG
Resource5=IDD_ABOUTBOX
Resource6=IDD_CK_LICENSEINFODLG
Resource7=IDD_CK_TRANSFERDLG
Resource8=IDD_CK_LOCATIONDLG
Resource9=IDD_ABOUTBOX (English (U.S.))
Resource10=IDD_CK_STARTDLG (English (U.S.))
Resource11=IDR_LICMENU (English (U.S.))
Resource12=IDD_CK_AUTHDLG (English (U.S.))
Resource13=IDD_CK_TRANSFERDLG (English (U.S.))
Resource14=IDD_CK_LICENSEINFODLG (English (U.S.))
Resource15=IDD_CK_AUTHDLG
Resource16=IDR_LICMENU
Resource17=IDD_CK_LOCATIONDLG (English (U.S.))

[CLS:CKwaApp]
Type=0
HeaderFile=kwa.h
ImplementationFile=kwa.cpp
Filter=N

[CLS:CKwaDlg]
Type=0
HeaderFile=kwaDlg.h
ImplementationFile=kwaDlg.cpp
Filter=D
LastObject=IDCANCEL
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=kwaDlg.h
ImplementationFile=kwaDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_TXT_FULLVERSION,static,1342308480
Control3=IDC_TXT_COPYRIGHT,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_TXT_COMPANY,static,1342308352

[DLG:IDD_KWA_DIALOG]
Type=1
Class=CKwaDlg
ControlCount=9
Control1=IDC_TXT_VERSION,static,1342308352
Control2=IDC_TXT_LICENSE,static,1342308352
Control3=IDC_TXT_USERS,static,1342308352
Control4=IDC_AUTHORIZE,button,1342242816
Control5=IDC_INFORMATION,button,1342242816
Control6=IDC_MORE,button,1342242816
Control7=IDC_ABOUT,button,1342242816
Control8=IDOK,button,1342242817
Control9=IDCANCEL,button,1342242816

[DLG:IDD_CK_LICENSEINFODLG]
Type=1
Class=?
ControlCount=9
Control1=IDC_CK_TXT_DATE,static,1342308352
Control2=IDC_CK_TXT_COPIES,static,1342308352
Control3=IDC_CK_TXT_USERS,static,1342308352
Control4=IDC_CK_TXT_TIME,static,1342308352
Control5=IDC_CK_OPTIONSLIST,SysListView32,1350631425
Control6=IDC_CK_TXT_STANDALONE,static,1342308354
Control7=IDC_CK_TXT_LOCATION,static,1342308352
Control8=IDC_CK_TXT_CRYPKEYVER,static,1342308352
Control9=IDOK,button,1342242817

[DLG:IDD_CK_STARTDLG]
Type=1
Class=?
ControlCount=6
Control1=IDC_CK_APP_VERSION,static,1342373888
Control2=IDC_CK_TRIAL,button,1342242816
Control3=IDC_CK_ISSUE,button,1342242816
Control4=IDC_CK_REGTRANS,button,1342242816
Control5=IDC_CK_TRANSIN,button,1342242816
Control6=IDCANCEL,button,1342242817

[DLG:IDD_CK_AUTHDLG]
Type=1
Class=?
ControlCount=8
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308352
Control3=IDC_CK_SITECODE,edit,1350633608
Control4=IDC_STATIC,static,1342308352
Control5=IDC_CK_SITEKEY,edit,1350631560
Control6=IDOK,button,1342242817
Control7=IDCANCEL,button,1342242816
Control8=IDC_CK_TXT_LOCATION,static,1342308352

[DLG:IDD_CK_TRANSFERDLG]
Type=1
Class=?
ControlCount=6
Control1=IDC_CK_TXTCOUNT,static,1342308352
Control2=IDC_CK_TXTTRANSFEROPTION,static,1342308352
Control3=IDC_CK_PATH,edit,1350631552
Control4=IDOK,button,1342242817
Control5=IDCANCEL,button,1342242816
Control6=IDC_CK_BROWSE,button,1342242816

[MNU:IDR_LICMENU]
Type=1
Class=?
Command1=ID_CK_LIC_TRIAL
Command2=ID_CK_LIC_AUTH
Command3=ID_CK_LIC_KILL
Command4=ID_CK_LIC_TRANSOUT
Command5=ID_CK_LIC_DIRECTTRANS
Command6=ID_CK_LIC_INFO
Command7=ID_CK_LIC_REGTRANSFER
Command8=ID_CK_LIC_TRANSIN
CommandCount=8

[DLG:IDD_CK_LICENSEINFODLG (English (U.S.))]
Type=1
Class=?
ControlCount=9
Control1=IDC_CK_TXT_DATE,static,1342308352
Control2=IDC_CK_TXT_COPIES,static,1342308352
Control3=IDC_CK_TXT_USERS,static,1342308352
Control4=IDC_CK_TXT_TIME,static,1342308352
Control5=IDC_CK_OPTIONSLIST,SysListView32,1350631425
Control6=IDC_CK_TXT_STANDALONE,static,1342308354
Control7=IDC_CK_TXT_LOCATION,static,1342308352
Control8=IDC_CK_TXT_CRYPKEYVER,static,1342308352
Control9=IDOK,button,1342242817

[DLG:IDD_KWA_DIALOG (English (U.S.))]
Type=1
Class=?
ControlCount=9
Control1=IDC_TXT_VERSION,static,1342308352
Control2=IDC_TXT_LICENSE,static,1342308352
Control3=IDC_TXT_USERS,static,1342308352
Control4=IDC_AUTHORIZE,button,1342242816
Control5=IDC_INFORMATION,button,1342242816
Control6=IDC_MORE,button,1342242816
Control7=IDC_ABOUT,button,1342242816
Control8=IDOK,button,1342242817
Control9=IDCANCEL,button,1342242816

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_TXT_FULLVERSION,static,1342308480
Control3=IDC_TXT_COPYRIGHT,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_TXT_COMPANY,static,1342308352

[DLG:IDD_CK_STARTDLG (English (U.S.))]
Type=1
Class=?
ControlCount=6
Control1=IDC_CK_APP_VERSION,static,1342373888
Control2=IDC_CK_TRIAL,button,1342242816
Control3=IDC_CK_ISSUE,button,1342242816
Control4=IDC_CK_REGTRANS,button,1342242816
Control5=IDC_CK_TRANSIN,button,1342242816
Control6=IDCANCEL,button,1342242817

[DLG:IDD_CK_AUTHDLG (English (U.S.))]
Type=1
Class=?
ControlCount=7
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308352
Control3=IDC_CK_SITECODE,edit,1350631560
Control4=IDC_STATIC,static,1342308352
Control5=IDC_CK_SITEKEY,edit,1350631560
Control6=IDOK,button,1342242817
Control7=IDCANCEL,button,1342242816

[DLG:IDD_CK_TRANSFERDLG (English (U.S.))]
Type=1
Class=?
ControlCount=6
Control1=IDC_CK_TXTCOUNT,static,1342308352
Control2=IDC_CK_TXTTRANSFEROPTION,static,1342308352
Control3=IDC_CK_PATH,edit,1350631552
Control4=IDOK,button,1342242817
Control5=IDCANCEL,button,1342242816
Control6=IDC_CK_BROWSE,button,1342242816

[MNU:IDR_LICMENU (English (U.S.))]
Type=1
Class=?
Command1=ID_CK_LIC_TRIAL
Command2=ID_CK_LIC_AUTH
Command3=ID_CK_LIC_KILL
Command4=ID_CK_LIC_TRANSOUT
Command5=ID_CK_LIC_DIRECTTRANS
Command6=ID_CK_LIC_INFO
Command7=ID_CK_LIC_REGTRANSFER
Command8=ID_CK_LIC_TRANSIN
CommandCount=8

[DLG:IDD_CK_LOCATIONDLG]
Type=1
Class=?
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_CK_LOCATIONOPTION,button,1342308361
Control4=IDC_CK_LOCATIONOPTION2,button,1342177289
Control5=IDC_CK_APPPATH,edit,1350631552
Control6=IDC_CK_BROWSE,button,1342242816
Control7=IDC_STATIC,button,1342177287

[DLG:IDD_CK_LOCATIONDLG (English (U.S.))]
Type=1
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_CK_LOCATIONOPTION,button,1342308361
Control4=IDC_CK_LOCATIONOPTION2,button,1342177289
Control5=IDC_CK_APPPATH,edit,1350631552
Control6=IDC_CK_BROWSE,button,1342242816
Control7=IDC_STATIC,button,1342177287

