VERSION 5.00
Begin VB.Form dlgLoadAll 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Dialog Caption"
   ClientHeight    =   3240
   ClientLeft      =   2760
   ClientTop       =   3750
   ClientWidth     =   6600
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3240
   ScaleWidth      =   6600
   ShowInTaskbar   =   0   'False
   StartUpPosition =   1  'CenterOwner
   Begin VB.TextBox m_LoadUser 
      BackColor       =   &H8000000F&
      BorderStyle     =   0  'None
      Enabled         =   0   'False
      Height          =   285
      Left            =   120
      TabIndex        =   8
      Text            =   "Text1"
      Top             =   2760
      Width           =   3615
   End
   Begin VB.ListBox m_UserSnaps 
      Height          =   2205
      Left            =   4440
      TabIndex        =   4
      Top             =   360
      Width           =   2055
   End
   Begin VB.ListBox m_AllUserSnaps 
      Height          =   2205
      Left            =   2280
      TabIndex        =   3
      Top             =   360
      Width           =   2055
   End
   Begin VB.ListBox m_Models 
      Height          =   2205
      Left            =   120
      TabIndex        =   2
      Top             =   360
      Width           =   2055
   End
   Begin VB.CommandButton m_CancelButton 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   5280
      TabIndex        =   1
      Top             =   2760
      Width           =   1215
   End
   Begin VB.CommandButton m_OKButton 
      Caption         =   "OK"
      Height          =   375
      Left            =   3960
      TabIndex        =   0
      Top             =   2760
      Width           =   1215
   End
   Begin VB.Label Label3 
      Caption         =   "Models:"
      Height          =   255
      Left            =   120
      TabIndex        =   7
      Top             =   120
      Width           =   1695
   End
   Begin VB.Label Label2 
      Caption         =   "User Snapshots:"
      Height          =   255
      Left            =   4440
      TabIndex        =   6
      Top             =   120
      Width           =   1695
   End
   Begin VB.Label Label1 
      Caption         =   "Library Snapshots:"
      Height          =   255
      Left            =   2280
      TabIndex        =   5
      Top             =   120
      Width           =   1815
   End
End
Attribute VB_Name = "dlgLoadAll"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Option Explicit
Private Sub m_UserSnaps_Click()
  Dim i As Long
  i = m_UserSnaps.ListIndex
  m_AllUserSnaps.ListIndex = -1
  m_UserSnaps.ListIndex = i
End Sub

Private Sub m_AllUserSnaps_Click()
  Dim i As Long
  i = m_AllUserSnaps.ListIndex
  m_UserSnaps.ListIndex = -1
  m_AllUserSnaps.ListIndex = i
End Sub

'public
'Private TheFile As String

'Public Property Get MyFileName() As String

'MyFileName = TheFile

'End Property

Private Sub m_CancelButton_Click()
  Unload Me
End Sub

Private Sub Form_Load()
  'TheFile = ""
  
  Dim Path, Fn As String
  Dim A As ScdFileAttributes
  Dim FErr As Long
  
  m_LoadUser = "User : " & g_User.UserName
  
  ' All Models
  Path = ModelPath
  FErr = g_FileSystem.FindFirstFile(Path & "\*.spf", Fn, A)
  Do While FErr = 0
    If ((A And ScdFile_DIRECTORY) <> 0) Then
      m_Models.AddItem (Fn)
    End If
    FErr = g_FileSystem.FindNextFile(Fn, A)
  Loop
  
  ' All user snapshots
  Path = g_User.AllUsersProfile
  Path = Path & SnapShotPath
  FErr = g_FileSystem.FindFirstFile(Path & "\*.*", Fn, A)
  Do While FErr = 0
    If (((A And ScdFile_DIRECTORY) <> 0) And Left(Fn, 1) <> ".") Then
      m_AllUserSnaps.AddItem (Fn)
    End If
    FErr = g_FileSystem.FindNextFile(Fn, A)
  Loop
    
  ' user snapshots
  Path = g_User.UserProfile
  Path = Path & SnapShotPath
  FErr = g_FileSystem.FindFirstFile(Path & "\*.*", Fn, A)
  Do While FErr = 0
    If (((A And ScdFile_DIRECTORY) <> 0) And Left(Fn, 1) <> ".") Then
      m_UserSnaps.AddItem (Fn)
    End If
    FErr = g_FileSystem.FindNextFile(Fn, A)
  Loop
  
  'm_Models.Enabled = g_User.IsInstructor

End Sub

Private Sub m_OKButton_Click()
  On Error GoTo err
  
  'g_Files.DCSConfigPath
  Dim TheProject, TheSnap As String
  
  If Not g_Engine.EngineOK Then g_Engine.Initialize
  
  If m_Models.ListIndex >= 0 Then
    TheProject = ModelPath & "\" & m_Models & "\project.spj"
  End If
  If m_AllUserSnaps.ListIndex >= 0 Then
    TheSnap = g_User.AllUsersProfile & SnapShotPath & "\" & m_AllUserSnaps
  ElseIf m_UserSnaps.ListIndex >= 0 Then
    TheSnap = g_User.UserProfile & SnapShotPath & "\" & m_UserSnaps
  End If
  
  
  If Not g_Engine.ProjectOK Then
    g_Engine.LoadProject , , TheProject
    If Len(TheSnap) > 0 Then
      g_Engine.LoadSnapShot TheSnap
    End If
  End If
  'TheFile = "XXXX"
  Unload Me
  Exit Sub
  
err:
  MyMsgBox "Load Project " & err.Description
  
End Sub

