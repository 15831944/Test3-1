; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=AddItemDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "OPC Client.h"
LastPage=0

ClassCount=11
Class1=OPCClientApp
Class2=OPCClientDoc
Class3=OPCClientView
Class4=CMainFrame

ResourceCount=10
Resource1=IDD_ITEM_PARAMS
Resource2=IDD_OPC_SERVER
Class5=CAboutDlg
Resource3=IDR_MAINFRAME
Class6=AddItemDlg
Resource4=IDD_ITEM_QUERY_PROPERTIES
Resource5=IDR_ITEM_MENU
Class7=OPCServerDlg
Class8=GroupParamsDlg
Resource6=IDD_ADD_ITEM
Class9=WriteItemDlg
Resource7=IDD_STATUS
Class10=CFindServersDlg
Resource8=IDD_GROUP_PARAMS
Class11=ItemParamsDlg
Resource9=IDD_WRITE_ITEM
Resource10=IDD_ABOUTBOX

[CLS:OPCClientApp]
Type=0
HeaderFile=OPC Client.h
ImplementationFile=OPC Client.cpp
Filter=N

[CLS:OPCClientDoc]
Type=0
HeaderFile=OPC ClientDoc.h
ImplementationFile=OPC ClientDoc.cpp
Filter=N
BaseClass=CDocument
VirtualFilter=DC
LastObject=ID_APP_ABOUT

[CLS:OPCClientView]
Type=0
HeaderFile=OPC ClientView.h
ImplementationFile=OPC ClientView.cpp
Filter=C
LastObject=OPCClientView
BaseClass=CListView
VirtualFilter=VWC

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=ID_OPC_REMOVEITEM



[CLS:CAboutDlg]
Type=0
HeaderFile=OPC Client.cpp
ImplementationFile=OPC Client.cpp
Filter=D
LastObject=CAboutDlg

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

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_APP_EXIT
Command2=ID_OPC_CONNECT
Command3=ID_OPC_DISCONNECT
Command4=ID_OPC_SERVERSTATUS
Command5=ID_OPC_GROUPPARAMETERS
Command6=ID_OPC_ADDITEM
Command7=ID_OPC_ITEMPARAMETERS
Command8=ID_OPC_WRITEVALUETOITEM
Command9=ID_OPC_READITEM
Command10=ID_OPC_REFRESH
Command11=ID_OPC_REMOVEITEM
Command12=ID_VIEW_TOOLBAR
Command13=ID_VIEW_STATUS_BAR
Command14=ID_APP_ABOUT
CommandCount=14

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_APP_ABOUT
CommandCount=1

[DLG:IDD_ADD_ITEM]
Type=1
Class=AddItemDlg
ControlCount=20
Control1=IDC_ITEMID,edit,1350631552
Control2=IDC_TREE,SysTreeView32,1350631463
Control3=IDC_TAGS,listbox,1352728835
Control4=IDC_FILTER,edit,1350631552
Control5=IDC_NATIVE,button,1342177289
Control6=IDC_BOOL,button,1342177289
Control7=IDC_SHORT,button,1342177289
Control8=IDC_LONG,button,1342177289
Control9=IDC_DOUBLE,button,1342177289
Control10=IDC_STRING,button,1342177289
Control11=ID_ADDITEM,button,1342242817
Control12=IDCANCEL,button,1342242816
Control13=IDC_ACCESS_PATH,edit,1350631552
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,button,1342177287
Control18=IDC_STATIC,static,1342308352
Control19=IDC_FLAT,listbox,1084293379
Control20=ID_OPC_ITEMPARAMETERS,button,1342242816

[CLS:AddItemDlg]
Type=0
HeaderFile=AddItemDlg.h
ImplementationFile=AddItemDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=ID_OPC_ITEMPARAMETERS

[DLG:IDD_STATUS]
Type=1
Class=?
ControlCount=14
Control1=IDOK,button,1342242817
Control2=IDC_START_TIME,edit,1350633600
Control3=IDC_STATIC,static,1342308354
Control4=IDC_STATIC,button,1342177287
Control5=IDC_LAST_TIME,edit,1350633600
Control6=IDC_STATIC,static,1342308354
Control7=IDC_CURRENT_TIME,edit,1350633600
Control8=IDC_STATIC,static,1342308354
Control9=IDC_GROUPS,edit,1350633600
Control10=IDC_STATIC,static,1342308354
Control11=IDC_BANDWIDTH,edit,1350633600
Control12=IDC_STATIC,static,1342308354
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATUS,static,1342308352

[DLG:IDD_OPC_SERVER]
Type=1
Class=OPCServerDlg
ControlCount=11
Control1=IDC_SERVER,edit,1350631552
Control2=IDC_NODE,combobox,1344340226
Control3=IDC_REFRESH,button,1342242816
Control4=IDC_SERVERS10,button,1342324745
Control5=IDC_SERVERS20,button,1342193673
Control6=IDC_SERVER_LIST,listbox,1352728579
Control7=IDOK,button,1342242817
Control8=IDCANCEL,button,1342242816
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352

[CLS:OPCServerDlg]
Type=0
HeaderFile=OPCServerDlg.h
ImplementationFile=OPCServerDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_SERVERS10
VirtualFilter=dWC

[DLG:IDD_GROUP_PARAMS]
Type=1
Class=GroupParamsDlg
ControlCount=17
Control1=IDC_NAME,edit,1350631552
Control2=IDC_RATE,edit,1350631552
Control3=IDC_ACTIVE,button,1342243107
Control4=IDC_DEADBAND,edit,1350631552
Control5=IDOK,button,1342242817
Control6=IDCANCEL,button,1342242816
Control7=IDAPPLY,button,1342242816
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_TIMEBIAS,edit,1350631552
Control14=IDC_LCID,edit,1350631552
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352

[CLS:GroupParamsDlg]
Type=0
HeaderFile=GroupParamsDlg.h
ImplementationFile=GroupParamsDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC

[DLG:IDD_WRITE_ITEM]
Type=1
Class=WriteItemDlg
ControlCount=4
Control1=IDC_VALUE,edit,1350631552
Control2=IDC_ASYNC,button,1342242819
Control3=IDOK,button,1342242817
Control4=IDCANCEL,button,1342242816

[CLS:WriteItemDlg]
Type=0
HeaderFile=WriteItemDlg.h
ImplementationFile=WriteItemDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_ASYNC

[CLS:CFindServersDlg]
Type=0
HeaderFile=FindServersDlg.h
ImplementationFile=FindServersDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CFindServersDlg

[DLG:IDD_ITEM_PARAMS]
Type=1
Class=ItemParamsDlg
ControlCount=12
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_ITEMID,static,1342308352
Control4=IDC_ACCESS_PATH,static,1342308352
Control5=IDC_CLIENT_HANDLE,static,1342308352
Control6=IDC_SERVER_HANDLE,static,1342308352
Control7=IDC_ACCESS_RIGHTS,static,1342308352
Control8=IDC_REQUESTED_DATA,static,1342308352
Control9=IDC_NATIVE_DATA,static,1342308352
Control10=IDC_ACTIVE,button,1342242819
Control11=IDC_EU_TYPE,static,1342308352
Control12=IDC_EU_VALUE,static,1342308352

[CLS:ItemParamsDlg]
Type=0
HeaderFile=ItemParamsDlg.h
ImplementationFile=ItemParamsDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_ACCESS_PATH
VirtualFilter=dWC

[MNU:IDR_ITEM_MENU]
Type=1
Class=OPCClientDoc
Command1=ID_OPC_ITEMPARAMETERS
Command2=ID_OPC_ITEMATTRIBUTES
Command3=ID_OPC_WRITEVALUETOITEM
Command4=ID_OPC_READITEM
Command5=ID_OPC_REFRESH
Command6=ID_OPC_REMOVEITEM
CommandCount=6

[DLG:IDD_ITEM_QUERY_PROPERTIES]
Type=1
Class=?
ControlCount=3
Control1=IDCANCEL,button,1342242817
Control2=IDC_LIST,SysListView32,1350631433
Control3=IDC_REFRESH,button,1342242816

