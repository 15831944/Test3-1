VERSION 5.00
Begin VB.Form dlgSaveSnap 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Save Snapshot"
   ClientHeight    =   4155
   ClientLeft      =   2760
   ClientTop       =   3750
   ClientWidth     =   2775
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4155
   ScaleWidth      =   2775
   ShowInTaskbar   =   0   'False
   StartUpPosition =   1  'CenterOwner
   Begin VB.ComboBox m_SnapList 
      Height          =   2910
      ItemData        =   "dlgSaveSnap.frx":0000
      Left            =   120
      List            =   "dlgSaveSnap.frx":0002
      Sorted          =   -1  'True
      Style           =   1  'Simple Combo
      TabIndex        =   4
      Text            =   "m_SnapList"
      Top             =   360
      Width           =   2535
   End
   Begin VB.CheckBox m_CheckAllUser 
      Caption         =   "Save for All Users"
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   3360
      Width           =   2055
   End
   Begin VB.CommandButton m_CancelButton 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   1440
      TabIndex        =   1
      Top             =   3720
      Width           =   1215
   End
   Begin VB.CommandButton m_OKButton 
      Caption         =   "OK"
      Height          =   375
      Left            =   120
      TabIndex        =   0
      Top             =   3720
      Width           =   1215
   End
   Begin VB.Label m_SaveSnapLabel 
      Caption         =   "User Snaphots:"
      Height          =   255
      Left            =   120
      TabIndex        =   2
      Top             =   120
      Width           =   1695
   End
End
Attribute VB_Name = "dlgSaveSnap"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Form_QueryUnload(Cancel As Integer, UnloadMode As Integer)
On Error GoTo err
  Dim Path As String
  
  If Len(m_SnapList.Text) > 0 Then
    If m_CheckAllUser.Value Then
      Path = g_User.AllUsersProfile
    Else
      Path = g_User.UserProfile
    End If
    Path = Path & SnapShotPath & "\" & m_SnapList.Text
    g_Engine.SaveSnapShot Path
  End If
  Exit Sub
  
err:
  MyMsgBox "Save Snapshot '" & Path & err.Description
  
End Sub

Private Sub m_CancelButton_Click()
  m_SnapList.Text = ""
  Unload Me
End Sub

Private Sub LoadDirList()
  On Error GoTo err
  
  Dim Path, Fn As String
  Dim A As ScdFileAttributes
  Dim FErr As Long
  
  m_SnapList.Clear
  
  ' All user snapshots
  If m_CheckAllUser.Value Then
    Path = g_User.AllUsersProfile
    m_SaveSnapLabel = "Library Snapshots:"
  Else
    Path = g_User.UserProfile
    m_SaveSnapLabel = "User Snapshots:"
  End If
  
  Path = Path & SnapShotPath
  FErr = g_FileSystem.FindFirstFile(Path & "\*.*", Fn, A)
  Do While FErr = 0
    If (((A And ScdFile_DIRECTORY) <> 0) And Left(Fn, 1) <> ".") Then
      m_SnapList.AddItem (Fn)
    End If
    FErr = g_FileSystem.FindNextFile(Fn, A)
  Loop
  Exit Sub
err:
  MyMsgBox "Load DirList " & err.Description
End Sub

Private Sub Form_Load()
  On Error GoTo err
  m_CheckAllUser.Value = False
    
  If g_User.IsInstructor Then
    m_CheckAllUser.Enabled = True
  Else
    m_CheckAllUser.Enabled = False
  End If
  
  LoadDirList
    
  Exit Sub
err:
  MyMsgBox "Load Snapshot " & err.Description

End Sub

Private Sub m_CheckAllUser_Click()
  'm_CheckAllUser.Value = Not m_CheckAllUser.Value
  'If g_User.IsInstructor Then
  '  m_CheckAllUser.Value = True
  'Else
  '  m_CheckAllUser.Value = False
  'End If
  LoadDirList

End Sub

Private Sub m_OKButton_Click()
  On Error GoTo err
  
  Unload Me
  Exit Sub
  
err:
  MyMsgBox "Load Snapshot " & err.Description
  
End Sub

