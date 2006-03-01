; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTestMarshalDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "TestMarshal.h"
ODLFile=TestMarshal.odl

ClassCount=4
Class1=CTestMarshalApp
Class2=CTestMarshalDlg
Class3=CAboutDlg
Class4=CTestMarshalDlgAutoProxy

ResourceCount=5
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_TESTMARSHAL_DIALOG
Resource4=IDD_ABOUTBOX (English (U.S.))
Resource5=IDD_TESTMARSHAL_DIALOG (English (U.S.))

[CLS:CTestMarshalApp]
Type=0
HeaderFile=TestMarshal.h
ImplementationFile=TestMarshal.cpp
Filter=N

[CLS:CTestMarshalDlg]
Type=0
HeaderFile=TestMarshalDlg.h
ImplementationFile=TestMarshalDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=TestMarshalDlg.h
ImplementationFile=TestMarshalDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg

[CLS:CTestMarshalDlgAutoProxy]
Type=0
HeaderFile=DlgProxy.h
ImplementationFile=DlgProxy.cpp
BaseClass=CCmdTarget
Filter=N

[DLG:IDD_TESTMARSHAL_DIALOG]
Type=1
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Class=CTestMarshalDlg

[DLG:IDD_TESTMARSHAL_DIALOG (English (U.S.))]
Type=1
Class=CTestMarshalDlg
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_CONNECT,button,1342242816
Control4=IDC_DISCONNECT,button,1342242816
Control5=IDC_QUERYTIME,button,1342242816

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

