; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "modbusexplorer.h"
LastPage=0

ClassCount=14
Class1=CCopyTagDlg
Class2=DeviceDialog
Class3=CDoc
Class4=GroupDialog
Class5=CTagView
Class6=CMainFrame
Class7=CModbusExplorerApp
Class8=CAboutDlg
Class9=OPortDlg
Class10=CSplashWnd
Class11=ScalingPage
Class12=AlarmingPage
Class13=TagDialog
Class14=CDeviceView

ResourceCount=24
Resource1=IDD_ABOUTBOX
Resource2=IDR_DEVICE_INSPECTOR
Resource3=IDR_TAG_INSPECTOR
Resource4=IDR_MAINFRAME
Resource5=IDD_DEVICE
Resource6=IDD_GROUP
Resource7=IDD_TAG
Resource8=IDD_TAGANLGALARM
Resource9=IDD_TAGANLGDETAILS
Resource10=IDD_COPY_TAG
Resource11=IDD_PORTS
Resource12=IDR_TOOLBAR
Resource13=IDD_TAG (English (U.S.))
Resource14=IDR_DEVICE_INSPECTOR (English (U.S.))
Resource15=IDR_MAINFRAME (English (U.S.))
Resource16=IDD_COPY_TAG (English (U.S.))
Resource17=IDD_TAGANLGALARM (English (U.S.))
Resource18=IDD_GROUP (English (U.S.))
Resource19=IDD_DEVICE (English (U.S.))
Resource20=IDD_PORTS (English (U.S.))
Resource21=IDD_TAGANLGDETAILS (English (U.S.))
Resource22=IDR_TAG_INSPECTOR (English (U.S.))
Resource23=IDD_ABOUTBOX (English (U.S.))
Resource24=IDR_TOOLBAR (English (U.S.))

[CLS:CCopyTagDlg]
Type=0
BaseClass=CDialog
HeaderFile=CopyTagDlg.h
ImplementationFile=CopyTagDlg.cpp

[CLS:DeviceDialog]
Type=0
BaseClass=CDialog
HeaderFile=DeviceDialog.h
ImplementationFile=DeviceDialog.cpp
Filter=D
VirtualFilter=dWC
LastObject=ID_UPDATEALL

[CLS:CDoc]
Type=0
BaseClass=CDocument
HeaderFile=Doc.h
ImplementationFile=Doc.cpp
Filter=N
VirtualFilter=DC
LastObject=ID_UPDATEALL

[CLS:GroupDialog]
Type=0
BaseClass=CDialog
HeaderFile=GroupDialog.h
ImplementationFile=GroupDialog.cpp
Filter=D
VirtualFilter=dWC
LastObject=IDOK

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
Filter=T
VirtualFilter=fWC
LastObject=CMainFrame

[CLS:CModbusExplorerApp]
Type=0
BaseClass=CWinApp
HeaderFile=ModbusExplorer.h
ImplementationFile=ModbusExplorer.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=ModbusExplorer.cpp
ImplementationFile=ModbusExplorer.cpp

[CLS:OPortDlg]
Type=0
BaseClass=CDialog
HeaderFile=Ports.h
ImplementationFile=Ports.cpp

[CLS:CSplashWnd]
Type=0
BaseClass=CWnd
HeaderFile=Splash.h
ImplementationFile=Splash.cpp

[CLS:ScalingPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=Styles.h
ImplementationFile=Styles.cpp

[CLS:AlarmingPage]
Type=0
BaseClass=CPropertyPage
HeaderFile=Styles.h
ImplementationFile=Styles.cpp
LastObject=AlarmingPage

[CLS:TagDialog]
Type=0
BaseClass=CDialog
HeaderFile=TagDialog.h
ImplementationFile=TagDialog.cpp
LastObject=ID_SET
Filter=D
VirtualFilter=dWC

[CLS:CDeviceView]
Type=0
BaseClass=CTreeView
HeaderFile=TreeView.h
ImplementationFile=TreeView.cpp

[DLG:IDD_COPY_TAG]
Type=1
Class=CCopyTagDlg
ControlCount=8
Control1=IDC_RENAME,button,1342242817
Control2=IDC_OVERWRITE,button,1342242816
Control3=IDC_TITLE,static,1342308352
Control4=IDC_SKIP,button,1342242817
Control5=IDCANCEL,button,1342242817
Control6=IDC_CURRENT,static,1342308352
Control7=IDC_NAME,edit,1350631552
Control8=IDC_NEW,static,1342308352

[DLG:IDD_DEVICE]
Type=1
Class=DeviceDialog
ControlCount=15
Control1=IDC_NAME,edit,1350631552
Control2=IDC_PORT,combobox,1344340227
Control3=IDC_ADDRESS,edit,1350631552
Control4=IDC_TIMEOUT,edit,1350631552
Control5=IDC_WORDSWAP,button,1342243363
Control6=IDC_SIMULATE,button,1342242819
Control7=IDOK,button,1342242817
Control8=IDCANCEL,button,1342242816
Control9=IDC_STATIC,static,1342308354
Control10=IDC_STATIC,static,1342308354
Control11=IDC_STATIC,static,1342308354
Control12=IDC_STATIC,static,1342308354
Control13=IDC_STATIC,button,1342177287
Control14=IDC_MSGDELAY,edit,1350631552
Control15=IDC_STATIC,static,1342308354

[DLG:IDD_GROUP]
Type=1
Class=GroupDialog
ControlCount=4
Control1=IDC_NAME,edit,1350631552
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816
Control4=IDC_STATIC,static,1342308354

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=8
Control1=IDC_STATIC,static,1350565891
Control2=IDC_STATIC,static,1342308480
Control3=IDOK,button,1342373889
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308480
Control6=IDC_STATIC,static,1342308480
Control7=IDC_STATIC,static,1342308480
Control8=IDC_STATIC,static,1342308480

[DLG:IDD_PORTS]
Type=1
Class=OPortDlg
ControlCount=14
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_PORTS,combobox,1344340227
Control4=IDC_BAUD,combobox,1344339971
Control5=IDC_PORT_PARITY,combobox,1344339971
Control6=IDC_FLOW,combobox,1344339971
Control7=IDC_DATA,combobox,1344339971
Control8=IDC_STOP,combobox,1344339971
Control9=IDC_STATIC,static,1342308354
Control10=IDC_STATIC,static,1342308354
Control11=IDC_STATIC,static,1342308354
Control12=IDC_STATIC,static,1342308354
Control13=IDC_STATIC,static,1342308354
Control14=IDC_STATIC,static,1342308354

[DLG:IDD_TAGANLGDETAILS]
Type=1
Class=ScalingPage
ControlCount=20
Control1=IDC_MINRAW,edit,1350631552
Control2=IDC_MAXRAW,edit,1350631552
Control3=IDC_ENGUNITS,edit,1350631552
Control4=IDC_MINEU,edit,1350631552
Control5=IDC_MAXEU,edit,1350631552
Control6=IDC_LINEAR,button,1342308361
Control7=IDC_SQUARE,button,1342177289
Control8=IDC_DEBAND,edit,1350631552
Control9=IDC_LOGDEADBAND,edit,1350631552
Control10=IDC_STATIC,button,1342177287
Control11=IDC_SENGUNITS,static,1342308354
Control12=IDC_SMINRAW,static,1342308354
Control13=IDC_SMAXRAW,static,1342308354
Control14=IDC_SDEADBAND,static,1342308354
Control15=IDC_SLOGDEADBAND,static,1342308354
Control16=IDC_STATIC,static,1342308352
Control17=IDC_SMINRAW2,static,1342308354
Control18=IDC_SMAXRAW2,static,1342308354
Control19=IDC_STATIC,static,1342308352
Control20=IDC_EXAMPLE,static,1342308352

[DLG:IDD_TAGANLGALARM]
Type=1
Class=AlarmingPage
ControlCount=41
Control1=IDC_PRIORITY,edit,1350631552
Control2=IDC_ENABLELOLOALM,button,1342242819
Control3=IDC_LOLO,edit,1350631552
Control4=IDC_LOLOPRIOR,edit,1350631552
Control5=IDC_ENABLELOW,button,1342242819
Control6=IDC_LOW,edit,1350631552
Control7=IDC_LOWPRIOR,edit,1350631552
Control8=IDC_ENABLEHIGH,button,1342242819
Control9=IDC_HIGH,edit,1350631552
Control10=IDC_HIGHPRIOR,edit,1350631552
Control11=IDC_ENABLEHIHI,button,1342242819
Control12=IDC_HIHI,edit,1350631552
Control13=IDC_HIHIPRIOR,edit,1350631552
Control14=IDC_VALUEDEAD,edit,1350631552
Control15=IDC_DEVIATION,edit,1350631552
Control16=IDC_ENABLEMINOR,button,1342242819
Control17=IDC_MINOR,edit,1350631552
Control18=IDC_MINORPRIOR,edit,1350631552
Control19=IDC_ENABLEMAJOR,button,1342242819
Control20=IDC_MAJOR,edit,1350631552
Control21=IDC_MAJORPRIOR,edit,1350631552
Control22=IDC_DEVDEADBAND,edit,1350631552
Control23=IDC_ENABLERATE,button,1342242819
Control24=IDC_RATE,edit,1350631552
Control25=IDC_RATESEC,button,1342308361
Control26=IDC_RATEMIN,button,1342177289
Control27=IDC_RATEHOUR,button,1342177289
Control28=IDC_RATEPRIOR,edit,1350631552
Control29=IDC_STATIC,static,1342308352
Control30=IDC_STATIC,static,1342308352
Control31=IDC_SVALDEAD,static,1342308352
Control32=IDC_STATIC,button,1342177287
Control33=IDC_STATIC,static,1342308352
Control34=IDC_STATIC,static,1342308352
Control35=IDC_SDEVDEADBAND,static,1342308352
Control36=IDC_STATIC,static,1342308352
Control37=IDC_STATIC,button,1342177287
Control38=IDC_STATIC,button,1342177287
Control39=IDC_STATIC,static,1342308352
Control40=IDC_STATIC,static,1342308352
Control41=IDC_SPRIORITY,static,1342308352

[DLG:IDD_TAG]
Type=1
Class=TagDialog
ControlCount=17
Control1=IDC_NAME,edit,1350631552
Control2=IDC_DESCRIPTION,edit,1350631552
Control3=IDC_OFFSET,edit,1350631552
Control4=IDC_RANGE,combobox,1344339971
Control5=IDC_DATATYPE,combobox,1344339971
Control6=IDC_ENABLE_SCALING,button,1342242819
Control7=IDC_SCALING_SETTINGS,button,1342242816
Control8=IDC_SIMULATION_TYPE,combobox,1344339971
Control9=IDOK,button,1342242817
Control10=IDCANCEL,button,1342242816
Control11=IDC_STATIC,static,1342308354
Control12=IDC_STATIC,static,1342308354
Control13=IDC_STATIC,static,1342308354
Control14=IDC_STATIC,button,1342177287
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308354
Control17=IDC_SCALING_DESCRIPTION,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_ADD_NEWDEVICE
Command8=ID_ADD_NEWGROUP
Command9=ID_ADD_NEWTAG
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_EDIT_DELETE
Command14=ID_EDIT_PORTS
Command15=ID_EDIT_PROPERTIES
Command16=ID_VIEW_MONITOR
Command17=ID_VIEW_STATUS_BAR
Command18=ID_APP_ABOUT
CommandCount=18

[MNU:IDR_DEVICE_INSPECTOR]
Type=1
Class=?
Command1=ID_EDIT_PASTE
Command2=ID_EDIT_DELETE
Command3=ID_ADD_NEWGROUP
Command4=ID_ADD_NEWTAG
Command5=ID_EDIT_PROPERTIES
CommandCount=5

[MNU:IDR_TAG_INSPECTOR]
Type=1
Class=?
Command1=ID_EDIT_CUT
Command2=ID_EDIT_COPY
Command3=ID_EDIT_DELETE
Command4=ID_EDIT_PROPERTIES
CommandCount=4

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_ADD_NEWDEVICE
Command3=ID_ADD_NEWGROUP
Command4=ID_FILE_NEW
Command5=ID_FILE_OPEN
Command6=ID_FILE_SAVE
Command7=ID_ADD_NEWTAG
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_NEXT_PANE
Command12=ID_PREV_PANE
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_EDIT_CUT
Command16=ID_EDIT_UNDO
CommandCount=16

[TB:IDR_TOOLBAR]
Type=1
Class=?
Command1=ID_ADD_NEWDEVICE
Command2=ID_ADD_NEWGROUP
Command3=ID_ADD_NEWTAG
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
CommandCount=6

[DLG:IDD_COPY_TAG (English (U.S.))]
Type=1
Class=?
ControlCount=8
Control1=IDC_RENAME,button,1342242817
Control2=IDC_OVERWRITE,button,1342242816
Control3=IDC_TITLE,static,1342308352
Control4=IDC_SKIP,button,1342242817
Control5=IDCANCEL,button,1342242817
Control6=IDC_CURRENT,static,1342308352
Control7=IDC_NAME,edit,1350631552
Control8=IDC_NEW,static,1342308352

[DLG:IDD_DEVICE (English (U.S.))]
Type=1
Class=?
ControlCount=5
Control1=IDC_NAME,edit,1350631552
Control2=IDC_SIMULATE,button,1342242819
Control3=IDOK,button,1342242817
Control4=IDCANCEL,button,1342242816
Control5=IDC_STATIC,static,1342308354

[DLG:IDD_PORTS (English (U.S.))]
Type=1
Class=?
ControlCount=14
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_PORTS,combobox,1344340227
Control4=IDC_BAUD,combobox,1344339971
Control5=IDC_PORT_PARITY,combobox,1344339971
Control6=IDC_FLOW,combobox,1344339971
Control7=IDC_DATA,combobox,1344339971
Control8=IDC_STOP,combobox,1344339971
Control9=IDC_STATIC,static,1342308354
Control10=IDC_STATIC,static,1342308354
Control11=IDC_STATIC,static,1342308354
Control12=IDC_STATIC,static,1342308354
Control13=IDC_STATIC,static,1342308354
Control14=IDC_STATIC,static,1342308354

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=8
Control1=IDC_STATIC,static,1350565891
Control2=IDC_STATIC,static,1342308480
Control3=IDOK,button,1342373889
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308480
Control6=IDC_STATIC,static,1342308480
Control7=IDC_STATIC,static,1342308480
Control8=IDC_STATIC,static,1342308480

[DLG:IDD_GROUP (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_NAME,edit,1350631552
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816
Control4=IDC_STATIC,static,1342308354

[DLG:IDD_TAG (English (U.S.))]
Type=1
Class=TagDialog
ControlCount=13
Control1=IDC_NAME,edit,1484849280
Control2=IDC_DESCRIPTION,edit,1350631552
Control3=IDC_DATATYPE,edit,1484849280
Control4=IDC_VALUE,edit,1350631552
Control5=ID_SET,button,1342242817
Control6=IDC_SIMULATION_TYPE,combobox,1344339971
Control7=IDOK,button,1342242817
Control8=IDCANCEL,button,1342242816
Control9=IDC_STATIC,static,1342308354
Control10=IDC_STATIC,static,1342308354
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308354
Control13=IDC_STATIC,static,1342308354

[DLG:IDD_TAGANLGALARM (English (U.S.))]
Type=1
Class=?
ControlCount=41
Control1=IDC_PRIORITY,edit,1350631552
Control2=IDC_ENABLELOLOALM,button,1342242819
Control3=IDC_LOLO,edit,1350631552
Control4=IDC_LOLOPRIOR,edit,1350631552
Control5=IDC_ENABLELOW,button,1342242819
Control6=IDC_LOW,edit,1350631552
Control7=IDC_LOWPRIOR,edit,1350631552
Control8=IDC_ENABLEHIGH,button,1342242819
Control9=IDC_HIGH,edit,1350631552
Control10=IDC_HIGHPRIOR,edit,1350631552
Control11=IDC_ENABLEHIHI,button,1342242819
Control12=IDC_HIHI,edit,1350631552
Control13=IDC_HIHIPRIOR,edit,1350631552
Control14=IDC_VALUEDEAD,edit,1350631552
Control15=IDC_DEVIATION,edit,1350631552
Control16=IDC_ENABLEMINOR,button,1342242819
Control17=IDC_MINOR,edit,1350631552
Control18=IDC_MINORPRIOR,edit,1350631552
Control19=IDC_ENABLEMAJOR,button,1342242819
Control20=IDC_MAJOR,edit,1350631552
Control21=IDC_MAJORPRIOR,edit,1350631552
Control22=IDC_DEVDEADBAND,edit,1350631552
Control23=IDC_ENABLERATE,button,1342242819
Control24=IDC_RATE,edit,1350631552
Control25=IDC_RATESEC,button,1342308361
Control26=IDC_RATEMIN,button,1342177289
Control27=IDC_RATEHOUR,button,1342177289
Control28=IDC_RATEPRIOR,edit,1350631552
Control29=IDC_STATIC,static,1342308352
Control30=IDC_STATIC,static,1342308352
Control31=IDC_SVALDEAD,static,1342308352
Control32=IDC_STATIC,button,1342177287
Control33=IDC_STATIC,static,1342308352
Control34=IDC_STATIC,static,1342308352
Control35=IDC_SDEVDEADBAND,static,1342308352
Control36=IDC_STATIC,static,1342308352
Control37=IDC_STATIC,button,1342177287
Control38=IDC_STATIC,button,1342177287
Control39=IDC_STATIC,static,1342308352
Control40=IDC_STATIC,static,1342308352
Control41=IDC_SPRIORITY,static,1342308352

[DLG:IDD_TAGANLGDETAILS (English (U.S.))]
Type=1
Class=?
ControlCount=20
Control1=IDC_MINRAW,edit,1350631552
Control2=IDC_MAXRAW,edit,1350631552
Control3=IDC_ENGUNITS,edit,1350631552
Control4=IDC_MINEU,edit,1350631552
Control5=IDC_MAXEU,edit,1350631552
Control6=IDC_LINEAR,button,1342308361
Control7=IDC_SQUARE,button,1342177289
Control8=IDC_DEBAND,edit,1350631552
Control9=IDC_LOGDEADBAND,edit,1350631552
Control10=IDC_STATIC,button,1342177287
Control11=IDC_SENGUNITS,static,1342308354
Control12=IDC_SMINRAW,static,1342308354
Control13=IDC_SMAXRAW,static,1342308354
Control14=IDC_SDEADBAND,static,1342308354
Control15=IDC_SLOGDEADBAND,static,1342308354
Control16=IDC_STATIC,static,1342308352
Control17=IDC_SMINRAW2,static,1342308354
Control18=IDC_SMAXRAW2,static,1342308354
Control19=IDC_STATIC,static,1342308352
Control20=IDC_EXAMPLE,static,1342308352

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_ADD_NEWDEVICE
Command8=ID_ADD_NEWGROUP
Command9=ID_ADD_NEWTAG
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_EDIT_DELETE
Command14=ID_EDIT_PORTS
Command15=ID_EDIT_PROPERTIES
Command16=ID_VIEW_MONITOR
Command17=ID_VIEW_STATUS_BAR
Command18=ID_APP_ABOUT
Command19=ID_UPDATEALL
CommandCount=19

[MNU:IDR_DEVICE_INSPECTOR (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_PASTE
Command2=ID_EDIT_DELETE
Command3=ID_ADD_NEWGROUP
Command4=ID_ADD_NEWTAG
Command5=ID_EDIT_PROPERTIES
CommandCount=5

[MNU:IDR_TAG_INSPECTOR (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_CUT
Command2=ID_EDIT_COPY
Command3=ID_EDIT_DELETE
Command4=ID_EDIT_PROPERTIES
CommandCount=4

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_ADD_NEWDEVICE
Command3=ID_ADD_NEWGROUP
Command4=ID_FILE_NEW
Command5=ID_FILE_OPEN
Command6=ID_FILE_SAVE
Command7=ID_ADD_NEWTAG
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_NEXT_PANE
Command12=ID_PREV_PANE
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_EDIT_CUT
Command16=ID_EDIT_UNDO
CommandCount=16

[TB:IDR_TOOLBAR (English (U.S.))]
Type=1
Class=?
Command1=ID_ADD_NEWDEVICE
Command2=ID_ADD_NEWGROUP
Command3=ID_ADD_NEWTAG
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
CommandCount=6

