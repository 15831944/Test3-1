VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmScdOTS 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Operator Training System"
   ClientHeight    =   7965
   ClientLeft      =   10065
   ClientTop       =   780
   ClientWidth     =   8655
   Icon            =   "frmScdOTS.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   ScaleHeight     =   7965
   ScaleWidth      =   8655
   Begin VB.CommandButton cmdTmpStop 
      Caption         =   "Stop"
      Height          =   495
      Left            =   960
      TabIndex        =   16
      Top             =   3960
      Width           =   1215
   End
   Begin VB.CommandButton cmdTmpStep 
      Caption         =   "Step"
      Height          =   495
      Left            =   960
      TabIndex        =   15
      Top             =   3360
      Width           =   1215
   End
   Begin VB.CommandButton cmdTmpIdle 
      Caption         =   "Idle"
      Height          =   495
      Left            =   960
      TabIndex        =   14
      Top             =   2760
      Width           =   1215
   End
   Begin VB.TextBox Text2 
      BackColor       =   &H80000004&
      BorderStyle     =   0  'None
      Height          =   285
      Left            =   360
      TabIndex        =   13
      Text            =   "Snapshot:"
      Top             =   1560
      Width           =   855
   End
   Begin VB.TextBox Text1 
      BackColor       =   &H80000004&
      BorderStyle     =   0  'None
      Height          =   285
      Left            =   360
      TabIndex        =   12
      Text            =   "Model:"
      Top             =   1200
      Width           =   855
   End
   Begin VB.OptionButton m_Instructor 
      Caption         =   "Instructor"
      Height          =   195
      Left            =   1800
      TabIndex        =   10
      Top             =   6600
      Width           =   1455
   End
   Begin VB.OptionButton m_Trainee 
      Caption         =   "Trainee"
      Height          =   255
      Left            =   1800
      TabIndex        =   9
      Top             =   6360
      Width           =   1215
   End
   Begin VB.TextBox m_SnapLoaded 
      BackColor       =   &H80000004&
      BorderStyle     =   0  'None
      Enabled         =   0   'False
      Height          =   285
      Left            =   1200
      TabIndex        =   7
      Text            =   "<None>"
      Top             =   1560
      Width           =   3735
   End
   Begin VB.TextBox m_ModelLoaded 
      BackColor       =   &H80000004&
      BorderStyle     =   0  'None
      Enabled         =   0   'False
      Height          =   285
      Left            =   1200
      TabIndex        =   6
      Text            =   "<None>"
      Top             =   1200
      Width           =   3735
   End
   Begin MSComctlLib.ListView m_IVList 
      Height          =   4575
      Left            =   5280
      TabIndex        =   2
      Top             =   1320
      Width           =   3015
      _ExtentX        =   5318
      _ExtentY        =   8070
      View            =   3
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      _Version        =   393217
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      BorderStyle     =   1
      Appearance      =   1
      NumItems        =   4
      BeginProperty ColumnHeader(1) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         Text            =   "Tag"
         Object.Width           =   2117
      EndProperty
      BeginProperty ColumnHeader(2) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   1
         Text            =   "Type"
         Object.Width           =   794
      EndProperty
      BeginProperty ColumnHeader(3) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   2
         Text            =   "State"
         Object.Width           =   794
      EndProperty
      BeginProperty ColumnHeader(4) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   3
         Text            =   "Value"
         Object.Width           =   1411
      EndProperty
   End
   Begin MSComctlLib.ImageList m_ToolBarImg 
      Left            =   480
      Top             =   6600
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   18
      ImageHeight     =   17
      MaskColor       =   12632256
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   5
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmScdOTS.frx":0582
            Key             =   ""
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmScdOTS.frx":08A4
            Key             =   ""
         EndProperty
         BeginProperty ListImage3 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmScdOTS.frx":0AB6
            Key             =   ""
         EndProperty
         BeginProperty ListImage4 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmScdOTS.frx":0CDC
            Key             =   ""
         EndProperty
         BeginProperty ListImage5 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmScdOTS.frx":0F5E
            Key             =   ""
         EndProperty
      EndProperty
   End
   Begin MSComctlLib.Toolbar m_ToolBar 
      Align           =   1  'Align Top
      Height          =   585
      Left            =   0
      TabIndex        =   1
      Top             =   0
      Width           =   8655
      _ExtentX        =   15266
      _ExtentY        =   1032
      ButtonWidth     =   1535
      ButtonHeight    =   979
      AllowCustomize  =   0   'False
      Wrappable       =   0   'False
      Appearance      =   1
      Style           =   1
      ImageList       =   "m_ToolBarImg"
      _Version        =   393216
      BeginProperty Buttons {66833FE8-8583-11D1-B16A-00C0F0283628} 
         NumButtons      =   10
         BeginProperty Button1 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "Startup"
            ImageIndex      =   1
         EndProperty
         BeginProperty Button2 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "Shutdown"
         EndProperty
         BeginProperty Button3 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Style           =   3
         EndProperty
         BeginProperty Button4 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "Run"
            Key             =   "Run"
            Object.ToolTipText     =   "Starts and runs the Simulation"
            ImageIndex      =   2
         EndProperty
         BeginProperty Button5 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "Pause"
            Key             =   "Pause"
            ImageIndex      =   5
         EndProperty
         BeginProperty Button6 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "Step"
            Key             =   "Step"
            ImageIndex      =   3
         EndProperty
         BeginProperty Button7 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "Stop"
            Key             =   "Stop"
            ImageIndex      =   4
         EndProperty
         BeginProperty Button8 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Style           =   3
         EndProperty
         BeginProperty Button9 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "Load Snap"
         EndProperty
         BeginProperty Button10 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Caption         =   "Save Snap"
         EndProperty
      EndProperty
   End
   Begin MSComctlLib.StatusBar m_StatusBar 
      Align           =   2  'Align Bottom
      Height          =   375
      Left            =   0
      TabIndex        =   0
      Top             =   7590
      Width           =   8655
      _ExtentX        =   15266
      _ExtentY        =   661
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   3
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   1
            Bevel           =   0
            Object.Width           =   12515
            Text            =   "Initialized"
            TextSave        =   "Initialized"
            Key             =   "State"
            Object.Tag             =   "State"
         EndProperty
         BeginProperty Panel2 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Bevel           =   0
            Object.Width           =   882
            MinWidth        =   882
            Text            =   "Time"
            TextSave        =   "Time"
            Key             =   "TimeHd"
            Object.Tag             =   "TimeHd"
         EndProperty
         BeginProperty Panel3 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Alignment       =   2
            Bevel           =   0
            Object.Width           =   1764
            MinWidth        =   1764
            Text            =   "1:0:0"
            TextSave        =   "1:0:0"
            Key             =   "Time"
            Object.Tag             =   "Time"
         EndProperty
      EndProperty
   End
   Begin VB.Frame m_IVFrame 
      Caption         =   "Instructor Variables"
      Height          =   5415
      Left            =   5160
      TabIndex        =   3
      Top             =   1080
      Width           =   3255
      Begin VB.CommandButton m_cmdIVStop 
         Caption         =   "Stop"
         Height          =   375
         Left            =   1800
         TabIndex        =   5
         Top             =   4920
         Width           =   1215
      End
      Begin VB.CommandButton m_cmdIVStart 
         Caption         =   "Start"
         Height          =   375
         Left            =   240
         TabIndex        =   4
         Top             =   4920
         Width           =   1215
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Temporary Rights"
      Height          =   975
      Left            =   1680
      TabIndex        =   11
      Top             =   6120
      Width           =   1935
   End
   Begin MSComctlLib.TabStrip m_TabStrip1 
      Height          =   6735
      Left            =   120
      TabIndex        =   8
      Top             =   720
      Width           =   8415
      _ExtentX        =   14843
      _ExtentY        =   11880
      _Version        =   393216
      BeginProperty Tabs {1EFB6598-857C-11D1-B16A-00C0F0283628} 
         NumTabs         =   3
         BeginProperty Tab1 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "Status"
            ImageVarType    =   2
         EndProperty
         BeginProperty Tab2 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "Instructor Variables"
            ImageVarType    =   2
         EndProperty
         BeginProperty Tab3 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "Monitoring"
            ImageVarType    =   2
         EndProperty
      EndProperty
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuFileLoadModel 
         Caption         =   "&Load Model"
      End
      Begin VB.Menu mnuFileBrk_1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuFileConnect 
         Caption         =   "&Connect"
      End
      Begin VB.Menu mnuFileDisconnect 
         Caption         =   "&Disconnect"
      End
      Begin VB.Menu mnuFileBrk_2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuFileExit 
         Caption         =   "&Exit"
      End
   End
   Begin VB.Menu mnuSnapShot 
      Caption         =   "&SnapShot"
      Begin VB.Menu mnuSnapShotLoad 
         Caption         =   "Load Snap Shot"
      End
      Begin VB.Menu mnuSnapShotSave 
         Caption         =   "Save Snap Shot"
      End
   End
   Begin VB.Menu mnuLog 
      Caption         =   "&Log"
      Begin VB.Menu mnuLogSave 
         Caption         =   "&Save"
      End
      Begin VB.Menu mnuLogView 
         Caption         =   "&View"
      End
      Begin VB.Menu mnuLogReplay 
         Caption         =   "&Replay"
      End
   End
   Begin VB.Menu mnuSimulation 
      Caption         =   "&Simulation"
      Begin VB.Menu mnuSimulationPlay 
         Caption         =   "&Run"
      End
      Begin VB.Menu mnuSimulationPause 
         Caption         =   "&Pause"
      End
      Begin VB.Menu mnuSimulationStop 
         Caption         =   "&Stop"
      End
      Begin VB.Menu mnuSimulationBrk_1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuSimulationFast 
         Caption         =   "&Fast"
      End
      Begin VB.Menu mnuSimulationStep 
         Caption         =   "&Step"
      End
      Begin VB.Menu mnuSimulationRealTime 
         Caption         =   "Real &Time"
         Checked         =   -1  'True
      End
   End
   Begin VB.Menu mnuFailures 
      Caption         =   "Fai&lures"
      Begin VB.Menu mnuFailureEdit 
         Caption         =   "&Edit"
      End
      Begin VB.Menu mnuFailureNew 
         Caption         =   "&New"
      End
      Begin VB.Menu mnuFailureInvoke 
         Caption         =   "&Invoke"
      End
   End
   Begin VB.Menu mnuAts 
      Caption         =   "&ATS"
      Begin VB.Menu mnuAtsLoad 
         Caption         =   "&Load ATS"
      End
      Begin VB.Menu mnuAtsBrk_1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuAtsNew 
         Caption         =   "&New ATS"
      End
      Begin VB.Menu mnuAtsEdit 
         Caption         =   "&Edit ATS"
      End
      Begin VB.Menu mnuAtsBrk_2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuAtsStart 
         Caption         =   "&Start"
      End
      Begin VB.Menu mnuAtsStop 
         Caption         =   "&Stop"
      End
   End
   Begin VB.Menu mnuTools 
      Caption         =   "&Tools"
      Begin VB.Menu mnuToolsOptions 
         Caption         =   "&Options"
      End
      Begin VB.Menu mnuToolsStats 
         Caption         =   "&Stats"
      End
   End
   Begin VB.Menu mnuHelp 
      Caption         =   "&Help"
      Begin VB.Menu mnuHelpAbout 
         Caption         =   "&About"
      End
   End
End
Attribute VB_Name = "frmScdOTS"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
    Option Explicit

Private Sub Command1_Click()

End Sub

Private Sub cmdTmpIdle_Click()
  g_Engine.IdleAll True
End Sub
Private Sub cmdTmpStep_Click()
  g_Engine.StepAll True
End Sub
Private Sub cmdTmpStop_Click()
  g_Engine.StopAll
End Sub

Private Sub Form_Load()
On Error GoTo err:
  TagStripVisibility (1)
  Debug.Print "========================"
  
  ' Order important
  Set g_Registry = New ScdRegistry
  Set g_FileSystem = New ScdFileSystem
  Set g_User = New ScdUserInfo
  
  Set g_Files = New clsFileSystem
  Set g_Engine = New clsRunEngine
    
  m_Trainee.Value = True
  g_User.UserType = ScdUser_Trainee
  
  m_ModelLoaded.Text = "<Not Loaded>"
  m_SnapLoaded.Text = "<Not Loaded>"
  
  m_StatusBar.Panels("Time").Text = "0:0:0"

  Exit Sub
err:
  MyMsgBox err.Description
End Sub

Private Sub Form_Unload(Cancel As Integer)
  g_Engine.Terminate
End Sub

Private Sub m_Instructor_Click()
  g_User.UserType = ScdUser_Instructor
End Sub

Private Sub m_ToolBar_ButtonClick(ByVal Button As MSComctlLib.Button)
  Select Case Button.Index
  Case 1:
    dlgLoadAll.Show 1, Me
  Case 2:
    g_Files.UnloadModel
  Case 4 'run
    mnuSimulationPlay_Click
  Case 5 'pause
    mnuSimulationPause_Click
  Case 6 'step
    mnuSimulationStep_Click
  Case 7 'stop
    mnuSimulationStop_Click
  Case 9 'stop
    mnuSnapShotLoad_Click
  Case 10 'stop
    mnuSnapShotSave_Click
  Case Default
    MyMsgBox "Unknown index"
  End Select
End Sub

Private Sub m_Trainee_Click()
  g_User.UserType = ScdUser_Trainee
End Sub

Private Sub mnuFileConnect_Click()
  g_Engine.Initialize
End Sub

Private Sub mnuFileDisconnect_Click()
  g_Engine.Terminate
End Sub

Private Sub mnuFileExit_Click()
  Unload Me
End Sub

Private Sub mnuFileSwitchUser_Click()

End Sub

Private Sub mnuFileLoadModel_Click()
  dlgLoadAll.Show 1, Me
  Me.ZOrder
End Sub

Private Sub mnuHelpAbout_Click()
On Error GoTo err:
  frmAbout.Show vbModal, Me
  Exit Sub
err:
  MyMsgBox err.Description
End Sub

Private Sub mnuSimulationPause_Click()
  g_Engine.PauseExec
End Sub

Private Sub mnuSimulationPlay_Click()
On Error GoTo err:
  If Not g_Engine.EngineOK Then g_Engine.Initialize
  g_Engine.PlayExec
  Exit Sub
err:
  MyMsgBox err.Description
End Sub

Private Sub mnuSimulationStep_Click()
  g_Engine.StepAll False
End Sub

Private Sub mnuSimulationStop_Click()
On Error GoTo err:
  g_Engine.StopAll
  Exit Sub
err:
  MyMsgBox err.Description
End Sub

Private Sub mnuSnapShotLoad_Click()
  dlgLoadSnap.Show 1, Me
End Sub

Private Sub mnuSnapShotSave_Click()
  dlgSaveSnap.Show 1, Me
End Sub

Private Sub mnuToolsOptions_Click()
On Error GoTo err:
  frmOptions.Show vbModal, Me
  Exit Sub
err:
  MyMsgBox err.Description
End Sub

Private Sub mnuToolsStats_Click()
  frmStats.Show
End Sub

Private Sub m_TabStrip1_Click()
  TagStripVisibility (m_TabStrip1.SelectedItem.Index)
End Sub
  
Private Sub TagStripVisibility(Index As Long)
  Const N = 10
  Dim Vis(N) As Boolean
  Dim i As Long
  For i = 1 To N
    Vis(1) = False
  Next i
  
  Select Case Index
  Case 1:
    Vis(1) = True
    Vis(2) = True
  Case 2:
    Vis(3) = True
    Vis(4) = True
    Vis(5) = True
    Vis(6) = True
    Vis(7) = True
  End Select
  
  m_ModelLoaded.Visible = Vis(1)
  m_SnapLoaded.Visible = Vis(2)
  
  m_IVFrame.Visible = Vis(3)
  m_IVList.Visible = Vis(4)
  m_cmdIVStart = Vis(5)
  m_cmdIVStop = Vis(6)
 
End Sub
