; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CSysCADMarshalDoc
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "syscadmarshal.h"
LastPage=0

ClassCount=10
Class1=CChangeLog
Class2=CErrorView
Class3=CTagView
Class4=CMainFrame
Class5=CSysCADMarshalApp
Class6=CAboutDlg
Class7=CSysCADMarshalDoc
Class8=CSysCADMarshalView
Class9=CDeviceView

ResourceCount=3
Resource1=IDR_MAINFRAME (English (U.S.))
Resource2=IDD_ABOUTBOX (English (U.S.))
Class10=CSlotCfgDlg
Resource3=IDD_SLOTCONFIG

[CLS:CChangeLog]
Type=0
BaseClass=CDialog
HeaderFile=ChangeLog.h
ImplementationFile=ChangeLog.cpp

[CLS:CErrorView]
Type=0
BaseClass=CListView
HeaderFile=ErrorView.h
ImplementationFile=ErrorView.cpp

[CLS:CTagView]
Type=0
BaseClass=CListView
HeaderFile=ListView.h
ImplementationFile=ListView.cpp
Filter=C
VirtualFilter=VWC
LastObject=CTagView

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
LastObject=CMainFrame
Filter=T
VirtualFilter=fWC

[CLS:CSysCADMarshalApp]
Type=0
BaseClass=CWinApp
HeaderFile=SysCADMarshal.h
ImplementationFile=SysCADMarshal.cpp
LastObject=CSysCADMarshalApp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=SysCADMarshal.cpp
ImplementationFile=SysCADMarshal.cpp
LastObject=CAboutDlg

[CLS:CSysCADMarshalDoc]
Type=0
BaseClass=CDocument
HeaderFile=SysCADMarshalDoc.h
ImplementationFile=SysCADMarshalDoc.cpp
Filter=N
VirtualFilter=DC
LastObject=ID_CONFIGURATION_REFRESHDEVICES

[CLS:CSysCADMarshalView]
Type=0
BaseClass=CView
HeaderFile=SysCADMarshalView.h
ImplementationFile=SysCADMarshalView.cpp
LastObject=CSysCADMarshalView

[CLS:CDeviceView]
Type=0
BaseClass=CTreeView
HeaderFile=TreeView.h
ImplementationFile=TreeView.cpp

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_VIEW_CONFIGURATON
Command9=ID_VIEW_STATISTICS
Command10=ID_VIEW_HEXADECIMAL
Command11=ID_CONFIGURATION_COMMIT
Command12=ID_CONFIGURATION_LOADDB
Command13=ID_CONFIGURATION_SAVEDB
Command14=ID_MONITOR
Command15=ID_CHGLOG_TRK
Command16=ID_CHGLOG_ALL
Command17=ID_CONFIGURATION_REFRESHDEVICES
CommandCount=17

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_PRINT
Command6=ID_FILE_PRINT_PREVIEW
Command7=ID_FILE_PRINT_SETUP
Command8=ID_FILE_MRU_FILE1
Command9=ID_APP_EXIT
Command10=ID_EDIT_UNDO
Command11=ID_EDIT_CUT
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_VIEW_TOOLBAR
Command15=ID_VIEW_STATUS_BAR
Command16=ID_VIEW_STATISTICS
Command17=ID_VIEW_CONFIGURATON
Command18=ID_VIEW_HEXADECIMAL
Command19=ID_VIEW_REFRESH
Command20=ID_CONFIGURATION_COMMIT
Command21=ID_CONFIGURATION_LOADDB
Command22=ID_CONFIGURATION_SAVEDB
Command23=ID_CONFIGURATION_REFRESHDEVICES
Command24=ID_APP_ABOUT
CommandCount=24

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_FILE_NEW
Command3=ID_FILE_OPEN
Command4=ID_FILE_PRINT
Command5=ID_FILE_SAVE
Command6=ID_EDIT_PASTE
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_VIEW_REFRESH
Command10=ID_NEXT_PANE
Command11=ID_PREV_PANE
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_EDIT_CUT
Command15=ID_EDIT_UNDO
CommandCount=15

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_SLOTCONFIG]
Type=1
Class=CSlotCfgDlg
ControlCount=18
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_TAG,edit,1350631552
Control4=IDC_TYPE,edit,1350631552
Control5=IDC_CONNECTS,edit,1350631556
Control6=IDC_RANGE,edit,1350631552
Control7=IDC_MODIFIER,edit,1350631552
Control8=IDC_DESC,edit,1350631552
Control9=IDC_DEVICE,edit,1350631552
Control10=IDC_STATIC,static,1342308864
Control11=IDC_STATIC,static,1342308864
Control12=IDC_STATIC,static,1342308864
Control13=IDC_STATIC,static,1342308864
Control14=IDC_STATIC,static,1342308864
Control15=IDC_STATIC,static,1342308864
Control16=IDC_STATIC,static,1342308864
Control17=IDC_ADD,button,1342242816
Control18=IDC_DELETE,button,1342242816

[CLS:CSlotCfgDlg]
Type=0
HeaderFile=SlotCfgDlg.h
ImplementationFile=SlotCfgDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=ID_CONFIGURATION_COMMIT
VirtualFilter=dWC

