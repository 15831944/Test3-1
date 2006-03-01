VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "clsRunEngine"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Attribute VB_Ext_KEY = "SavedWithClassBuilder6" ,"Yes"
Attribute VB_Ext_KEY = "Top_Level" ,"Yes"
Option Explicit

Private m_bSimEngineReqd As Boolean
Private m_bDCSEngineReqd As Boolean
Private m_bPLCEngineReqd As Boolean

Private WithEvents m_ExecTmr As ScdVBLib.ScdTimer
Attribute m_ExecTmr.VB_VarHelpID = -1
Private WithEvents m_EventRelay As ScdVBLib.ScdEventRelay
Attribute m_EventRelay.VB_VarHelpID = -1

'Private WithEvents m_LoadTmr As ScdVBLib.ScdTimer
Private WithEvents m_Sim As SysCADLib.SysCADCmd
Attribute m_Sim.VB_VarHelpID = -1
Private WithEvents m_DCS As SysCADLib.SysCADCmd
Attribute m_DCS.VB_VarHelpID = -1
Private WithEvents m_PLC As SysCADLib.SysCADCmd
Attribute m_PLC.VB_VarHelpID = -1

Public Enum CurrentRunMode
    Stopped = 1
    Paused = 2
    Stepping = 3
    Running = 4
End Enum
Private m_CurrentMode As CurrentRunMode

Private m_bTimeToStop As Boolean
Private m_LoadDone As Long
Private m_LoadRqst As Long
Private m_StepMark As Long

Private m_bStarting As Boolean
Private m_bWaitingForSim As Boolean
Private m_bWaitingForDCS As Boolean
Private m_bWaitingForPLC As Boolean
Private m_bWaitingForTimer As Boolean
Private m_bManual As Boolean

Private m_TotalTimeWaitedForStep As Long
Private m_CurrentCycleTime As Long
Private m_CurrentTime As Double

Private m_EngineOK As Boolean
Private m_ProjectOK As Boolean
Private m_ErrorCount As Integer
'Private m_Ready As Boolean
Private m_RealTime As Boolean
Const LongTO = 60
Const const_StepSize = 0.5 ' Should maybe come from Sim - NB Sim does not yet Set its stepsize
Const const_TimeOut = 100

Private m_bWasWaitingForDCS As Boolean
Private m_bWasWaitingForPLC As Boolean
Private m_bWasWaitingForSim As Boolean
Private m_bWasWaitingForTimer As Boolean

Private Sub Class_Initialize()
  On Error GoTo err:
  
  m_bSimEngineReqd = True
  m_bDCSEngineReqd = True
  m_bPLCEngineReqd = True
  
  m_EngineOK = False
  m_ProjectOK = False
  m_RealTime = True
  m_bTimeToStop = False
  m_CurrentMode = Stopped
  m_StepMark = 0
  On Error Resume Next
  Set m_ExecTmr = New ScdVBLib.ScdTimer
  Set m_EventRelay = New ScdVBLib.ScdEventRelay
  
 ' Set m_LoadTmr = New ScdVBLib.ScdTimer
  If err.Number <> 0 Then err.Raise 1, , "Unable to talk to vb helper dll"
  m_ExecTmr.Timeout = const_TimeOut
  Exit Sub
err:
  MyMsgBox "RunEngine: " & err.Description
End Sub
  
Private Sub Class_Terminate()
  StopAll
  Set m_ExecTmr = Nothing
  Set m_EventRelay = Nothing
End Sub
  
Public Property Get CurrentCycleTime() As Integer
  CurrentCycleTime = m_CurrentCycleTime
End Property
  
Public Property Get EngineOK() As Boolean
  EngineOK = m_EngineOK
End Property
  
Public Property Get ProjectOK() As Boolean
  ProjectOK = m_ProjectOK
End Property
  
Public Property Get RealTime() As Boolean
  RealTime = m_RealTime
End Property
  
Public Property Let RealTime(ByVal vNewValue As Boolean)
  m_RealTime = vNewValue
End Property
Public Property Get SimEngineReqd() As Boolean
  SimEngineReqd = m_bSimEngineReqd
End Property
Public Property Let SimEngineReqd(ByVal vNewValue As Boolean)
  m_bSimEngineReqd = vNewValue
End Property

Public Property Get DCSEngineReqd() As Boolean
  DCSEngineReqd = m_bDCSEngineReqd
End Property
Public Property Let DCSEngineReqd(ByVal vNewValue As Boolean)
  m_bDCSEngineReqd = vNewValue
End Property

Public Property Get PLCEngineReqd() As Boolean
  PLCEngineReqd = m_bPLCEngineReqd
End Property
Public Property Let PLCEngineReqd(ByVal vNewValue As Boolean)
  m_bPLCEngineReqd = vNewValue
End Property
  
Private Sub TryToConnect()
  ' order is important
  m_EngineOK = False
  m_ErrorCount = 0
  CreateAndInitializePLC
  CreateAndInitializeDCS
  CreateAndInitializeSim
  m_EngineOK = (m_ErrorCount = 0)
  m_ProjectOK = False
End Sub
  
Private Sub TryToDisconnect()
  ' order is important
  CloseAndTerminateSim
  CloseAndTerminateDCS
  CloseAndTerminatePLC
  m_ErrorCount = 0
  m_ProjectOK = False
  m_EngineOK = False
End Sub
  
Private Sub DoLoadOne()
  If m_LoadRqst = 1 And m_LoadDone = 0 Then
    m_LoadRqst = 2
    If Not m_PLC Is Nothing Then
      m_PLC.LoadProject g_Files.PLCConfigPath
    Else
      m_LoadDone = 1
    End If
  End If
  
  If m_LoadRqst = 2 And m_LoadDone = 1 Then
    m_LoadRqst = 3
    If Not m_DCS Is Nothing Then
      m_DCS.LoadProject g_Files.DCSConfigPath
    Else
      m_LoadDone = 2
    End If
  End If
  
  If m_LoadRqst = 3 And m_LoadDone = 2 Then
    m_LoadRqst = 4
    If Not m_Sim Is Nothing Then
      m_Sim.LoadProject g_Files.SimConfigPath
    Else
      m_LoadDone = 3
    End If
  End If
  
End Sub

Public Sub LoadProject(Optional ByVal PLCPath As String, Optional ByVal DCSPath As String, Optional ByVal SimPath As String)
  m_ErrorCount = 0
  ' Config Paths
  If Len(PLCPath) = 0 Then
    PLCPath = g_Files.PLCConfigPath
  Else
    g_Files.PLCConfigPath = PLCPath
  End If
  If Len(DCSPath) = 0 Then
    DCSPath = g_Files.DCSConfigPath
  Else
    g_Files.DCSConfigPath = DCSPath
  End If
  If Len(SimPath) = 0 Then
    SimPath = g_Files.SimConfigPath
  Else
    g_Files.SimConfigPath = SimPath
  End If
  ' Do The Load
  m_LoadDone = 0
  m_LoadRqst = 1
  'm_LoadTmr.Interval = 500
  'm_LoadTmr.Start
  
  Do While m_LoadDone < 3
    DoLoadOne
    DoEvents
  Loop
  m_ProjectOK = (m_ErrorCount = 0)
End Sub
  
'Private Sub m_LoadTmr_Expired()
'  If m_LoadDone < 3 Then
'    DoLoadOne
'  Else
'    m_LoadTmr.Stop
'    m_ProjectOK = (m_ErrorCount = 0)
'  End If
'End Sub

Private Sub m_PLC_OnProjectLoaded(ByVal result As Long)
  Debug.Print "PLC_OnProjectLoaded"
  m_LoadDone = m_LoadDone + 1
End Sub
Private Sub m_DCS_OnProjectLoaded(ByVal result As Long)
  Debug.Print "DCS_OnProjectLoaded"
  m_LoadDone = m_LoadDone + 1
End Sub
Private Sub m_Sim_OnProjectLoaded(ByVal result As Long)
  Debug.Print "Sim_OnProjectLoaded"
  m_LoadDone = m_LoadDone + 1
End Sub
  
Public Sub CloseProject()
  m_ErrorCount = 0
  
  If Not m_PLC Is Nothing Then
    m_PLC.CloseProject
    If err.Number <> 0 Then m_ErrorCount = m_ErrorCount + 1
  End If
  
  If Not m_DCS Is Nothing Then
    m_DCS.CloseProject
    If err.Number <> 0 Then m_ErrorCount = m_ErrorCount + 1
  End If
  
  If Not m_Sim Is Nothing Then
    m_Sim.CloseProject
    If err.Number <> 0 Then m_ErrorCount = m_ErrorCount + 1
  End If
  
End Sub
  
Private Sub CreateAndInitializeSim()
  On Error GoTo err:
  If m_bSimEngineReqd Then
    Set m_Sim = CreateObject("SysCAD.SysCADCmd.1")
    m_Sim.SyncCallsOn = False
    m_Sim.WaitUpNAbout 20
    m_Sim.SyncCallsTimeOut = LongTO
    m_Sim.EventsOn = True
    m_Sim.StepSize = const_StepSize
    'm_Sim.SetAppWndState (4)
  End If
  Exit Sub
err:
  MyMsgBox err.Description
  m_ErrorCount = m_ErrorCount + 1
  Set m_Sim = Nothing
End Sub
  
Private Sub CreateAndInitializeDCS()
  On Error GoTo err:
  If m_bDCSEngineReqd Then
    Set m_DCS = CreateObject("CPSimulator.SysCADCmd.1")
    m_DCS.SyncCallsOn = False
    m_DCS.WaitUpNAbout 20
    m_DCS.SyncCallsTimeOut = LongTO
    m_DCS.EventsOn = True
    m_DCS.StepSize = const_StepSize
    'm_DCS.SetAppWndState (4)
  End If
  
  Exit Sub
err:
  MyMsgBox err.Description
  m_ErrorCount = m_ErrorCount + 1
  Set m_DCS = Nothing
End Sub
  
Private Sub CreateAndInitializePLC()
  On Error GoTo err:
  If m_bPLCEngineReqd Then
    Set m_PLC = CreateObject("PLC5.SysCADCmd.1")
    m_PLC.SyncCallsOn = False
    m_PLC.WaitUpNAbout 10
    m_PLC.SyncCallsTimeOut = LongTO
    m_PLC.EventsOn = True
    m_PLC.StepSize = const_StepSize
    'm_PLC.SetAppWndState (4)
    End If
  Exit Sub
err:
  MyMsgBox err.Description
  m_ErrorCount = m_ErrorCount + 1
  Set m_PLC = Nothing
End Sub
  
Private Sub CloseAndTerminateSim()
  On Error GoTo err:
  If Not m_Sim Is Nothing Then
    m_Sim.Stop
    m_Sim.Exit
    Set m_Sim = Nothing
  End If
  Exit Sub
err:
  MyMsgBox err.Description
  m_ErrorCount = m_ErrorCount + 1
  Set m_Sim = Nothing
End Sub
  
Private Sub CloseAndTerminateDCS()
  On Error GoTo err:
  If Not m_DCS Is Nothing Then
    m_DCS.Stop
    m_DCS.Exit
    Set m_DCS = Nothing
  End If
  Exit Sub
err:
  MyMsgBox err.Description
  m_ErrorCount = m_ErrorCount + 1
  Set m_DCS = Nothing
End Sub
  
Private Sub CloseAndTerminatePLC()
  On Error GoTo err:
  If Not m_PLC Is Nothing Then
    m_PLC.Stop
    m_PLC.Exit
    Set m_PLC = Nothing
  End If
  Exit Sub
err:
  MyMsgBox err.Description
  m_ErrorCount = m_ErrorCount + 1
  Set m_PLC = Nothing
End Sub
  
Public Function Initialize() As Boolean
  On Error GoTo err:
  TryToConnect
  '    TryToLoad
  
  Initialize = True
  Exit Function
err:
  m_ProjectOK = False
  m_EngineOK = False
  Initialize = False
  End Function
  
  Public Function Terminate() As Boolean
  On Error GoTo err:
  TryToDisconnect
  '    TryToLoad
  
  Terminate = True
  Exit Function
err:
  m_ProjectOK = False
  m_EngineOK = False
  Terminate = False
End Function
  
Private Sub m_DCS_OnStepDynamic(ByVal Flags As Long)
  Debug.Print "DCS_OnStepDynamic " & Flags
  On Error Resume Next
  If Not m_bManual Then
    m_bWaitingForDCS = False
    Test4StepEnd False, "DCS"
  End If
End Sub
  
Private Sub m_PLC_OnStepDynamic(ByVal Flags As Long)
  Debug.Print "PLC_OnStepDynamic " & Flags
  On Error Resume Next
  If Not m_bManual Then
    m_bWaitingForPLC = False
    Test4StepEnd False, "PLC"
  End If
End Sub

Private Sub m_Sim_OnSnapshotLoaded(ByVal result As Long)
  Debug.Print "Sim_OnSnapshotLoaded"

End Sub

Private Sub m_Sim_OnSnapshotSaved(ByVal result As Long)
  Debug.Print "Sim_OnSnapshotSaved"

End Sub

Private Sub m_DCS_OnSnapshotLoaded(ByVal result As Long)
  Debug.Print "DCS_OnSnapshotLoaded"

End Sub

Private Sub m_DCS_OnSnapshotSaved(ByVal result As Long)
  Debug.Print "DCS_OnSnapshotSaved"

End Sub

Private Sub m_PLC_OnSnapshotLoaded(ByVal result As Long)
  Debug.Print "PLC_OnSnapshotLoaded"

End Sub

Private Sub m_PLC_OnSnapshotSaved(ByVal result As Long)
  Debug.Print "PLC_OnSnapshotSaved"

End Sub

Private Sub m_Sim_OnStepDynamic(ByVal Flags As Long)
  Debug.Print "Sim_OnStepDynamic"
  On Error Resume Next
  If Not m_bManual Then
    m_bWaitingForSim = False
    Test4StepEnd False, "Sim"
  End If
End Sub

Private Sub m_DCS_OnStop(ByVal Flags As Long)
  Debug.Print "DCS_OnStop"
  On Error Resume Next
End Sub
  
Private Sub m_PLC_OnStop(ByVal Flags As Long)
  Debug.Print "PLC_OnStop"
  On Error Resume Next
End Sub

Private Sub m_Sim_OnStop(ByVal Flags As Long)
  Debug.Print "Sim_OnStop"
  On Error Resume Next
End Sub

Private Sub m_DCS_OnIdle(ByVal Flags As Long)
  Debug.Print "DCS_OnIdle"
  On Error Resume Next
  If Not m_bManual And m_bStarting Then
    m_bWaitingForDCS = False
    Test4StepEnd False, "DCS"
  End If
End Sub
  
Private Sub m_PLC_OnIdle(ByVal Flags As Long)
  Debug.Print "PLC_OnIdle"
  On Error Resume Next
  If Not m_bManual And m_bStarting Then
    m_bWaitingForPLC = False
    Test4StepEnd False, "Plc"
  End If
End Sub

Private Sub m_Sim_OnIdle(ByVal Flags As Long)
  Debug.Print "Sim_OnIdle"
  On Error Resume Next
  If Not m_bManual And m_bStarting Then
    m_bWaitingForSim = False
    Test4StepEnd False, "Sim"
  End If
End Sub
  
Private Sub m_ExecTmr_Expired()
  On Error Resume Next
  m_TotalTimeWaitedForStep = m_TotalTimeWaitedForStep + const_TimeOut
  m_bWaitingForTimer = False
  Test4StepEnd True, "Tim"
End Sub

Private Sub SetAllWaiting(Wait4Timer As Boolean)
  If Not m_Sim Is Nothing Then m_bWaitingForSim = True
  If Not m_DCS Is Nothing Then m_bWaitingForDCS = True
  If Not m_PLC Is Nothing Then m_bWaitingForPLC = True
  m_bWaitingForTimer = Wait4Timer
End Sub

Private Sub m_EventRelay_YourNotifier(ByVal i_lCookie As Long)
  Debug.Print "EventRelay_YourNotifier"
  If i_lCookie = 2 Then
    StartNewStep
  End If
End Sub

Private Sub StartNewStep()
  Debug.Print "StartNewStep"
  On Error Resume Next
  SetAllWaiting True
  
  m_StepMark = m_StepMark + 1
  m_Sim.Step m_StepMark
  m_DCS.Step m_StepMark
  m_PLC.Step m_StepMark
  
  m_TotalTimeWaitedForStep = 0
  m_ExecTmr.Start
  m_CurrentTime = m_CurrentTime + const_StepSize
  
  frmScdOTS.m_StatusBar.Panels("Time").Text = m_CurrentTime & " [" & m_StepMark & "]"
End Sub

Private Sub Test4StepEnd(fromTimer As Boolean, Where As String)
  On Error Resume Next
  If True Then
    If (m_bWasWaitingForDCS <> m_bWaitingForDCS Or m_bWasWaitingForPLC <> m_bWaitingForPLC Or m_bWasWaitingForSim <> m_bWaitingForSim Or m_bWasWaitingForTimer <> m_bWaitingForTimer) Then
        Debug.Print "                             " & Where & " D:";
        Debug.Print m_bWaitingForDCS & " P:";
        Debug.Print m_bWaitingForPLC & " S:";
        Debug.Print m_bWaitingForSim & " T:";
        Debug.Print m_bWaitingForTimer
    Else
        Debug.Print ".";
    End If
    m_bWasWaitingForDCS = m_bWaitingForDCS
    m_bWasWaitingForPLC = m_bWaitingForPLC
    m_bWasWaitingForSim = m_bWaitingForSim
    m_bWasWaitingForTimer = m_bWaitingForTimer
  End If
  Dim bWaiting As Boolean
  bWaiting = m_bWaitingForDCS Or m_bWaitingForPLC Or m_bWaitingForSim Or m_bWaitingForTimer
  If Not bWaiting Then
    m_bStarting = False
'    If Not m_bTimeToStop And fromTimer Then
    If Not m_bTimeToStop Then
      m_CurrentCycleTime = m_TotalTimeWaitedForStep
      frmStats.CycleTime = m_CurrentCycleTime
      
      m_EventRelay.NotifyMe (2)  'StartNewStep
    End If
  End If

End Sub

Public Sub IdleAll(Manual As Boolean)
  On Error Resume Next
  Debug.Print "IdleAll"
  m_bManual = Manual
  m_Sim.Idle
  m_DCS.Idle
  m_PLC.Idle
End Sub

Public Sub StepAll(Manual As Boolean)
  On Error Resume Next
  Debug.Print "StepAll"
  m_bManual = Manual
  m_StepMark = m_StepMark + 1
  m_Sim.Step m_StepMark
  m_DCS.Step m_StepMark
  m_PLC.Step m_StepMark
End Sub
Public Sub StopAll()
  On Error Resume Next
  Debug.Print "StopAll"
  m_bTimeToStop = True
  m_ExecTmr.Stop
  
  m_Sim.Stop
  m_DCS.Stop
  m_PLC.Stop
  
End Sub

Public Sub PlayExec()
  Debug.Print "PlayExec"
  On Error Resume Next
  m_bTimeToStop = False
  m_CurrentTime = 0
  If m_RealTime Then
    m_bStarting = True
    SetAllWaiting True
    m_Sim.StepSize = const_StepSize
    m_DCS.StepSize = const_StepSize
    m_PLC.StepSize = const_StepSize
    m_DCS.Start
    m_PLC.Start
    IdleAll False
    m_TotalTimeWaitedForStep = 0
    m_ExecTmr.Start
    'StartNewStep
  Else
    m_Sim.Start
    m_DCS.Start
    m_PLC.Start
  End If
End Sub

Public Sub PauseExec()
  m_bTimeToStop = True
End Sub

Public Sub LoadSnapShot(LoadPath As String)
  Dim S, D, P As String
  
  P = LoadPath & PLCSnapFolderFile
  D = LoadPath & DCSSnapFolderFile
  S = LoadPath & SimSnapFolderFile

  m_ErrorCount = 0
  
  If Not m_PLC Is Nothing Then
    m_PLC.LoadSnapShot P
    If err.Number <> 0 Then m_ErrorCount = m_ErrorCount + 1
  End If
  
  If Not m_DCS Is Nothing Then
    m_DCS.LoadSnapShot D
    If err.Number <> 0 Then m_ErrorCount = m_ErrorCount + 1
  End If
  
  If Not m_Sim Is Nothing Then
    m_Sim.LoadSnapShot S
    If err.Number <> 0 Then m_ErrorCount = m_ErrorCount + 1
  End If
  
  'm_ProjectOK = (m_ErrorCount = 0)

End Sub

Public Sub SaveSnapShot(SavePath As String)
  Dim S, D, P As String
  
  P = SavePath & PLCSnapFolderFile
  D = SavePath & DCSSnapFolderFile
  S = SavePath & SimSnapFolderFile

  m_ErrorCount = 0
  
  If Not m_PLC Is Nothing Then
    m_PLC.SaveSnapShot P
    If err.Number <> 0 Then m_ErrorCount = m_ErrorCount + 1
  End If
  
  If Not m_DCS Is Nothing Then
    m_DCS.SaveSnapShot D
    If err.Number <> 0 Then m_ErrorCount = m_ErrorCount + 1
  End If
  
  If Not m_Sim Is Nothing Then
    m_Sim.SaveSnapShot S
    If err.Number <> 0 Then m_ErrorCount = m_ErrorCount + 1
  End If
  
  'm_ProjectOK = (m_ErrorCount = 0)

End Sub











