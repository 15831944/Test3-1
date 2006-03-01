VERSION 5.00
Begin VB.Form dlgLoadSnap 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Load Snapshot"
   ClientHeight    =   3240
   ClientLeft      =   2760
   ClientTop       =   3750
   ClientWidth     =   4455
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3240
   ScaleWidth      =   4455
   ShowInTaskbar   =   0   'False
   StartUpPosition =   1  'CenterOwner
   Begin VB.ListBox m_UserSnaps 
      Height          =   2205
      Left            =   2280
      TabIndex        =   3
      Top             =   360
      Width           =   2055
   End
   Begin VB.ListBox m_AllUserSnaps 
      Height          =   2205
      Left            =   120
      TabIndex        =   2
      Top             =   360
      Width           =   2055
   End
   Begin VB.CommandButton m_CancelButton 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   3120
      TabIndex        =   1
      Top             =   2760
      Width           =   1215
   End
   Begin VB.CommandButton m_OKButton 
      Caption         =   "OK"
      Height          =   375
      Left            =   1800
      TabIndex        =   0
      Top             =   2760
      Width           =   1215
   End
   Begin VB.Label Label2 
      Caption         =   "User Snaphots:"
      Height          =   255
      Left            =   2280
      TabIndex        =   5
      Top             =   120
      Width           =   1695
   End
   Begin VB.Label Label1 
      Caption         =   "Library Snapshots:"
      Height          =   255
      Left            =   120
      TabIndex        =   4
      Top             =   120
      Width           =   1815
   End
End
Attribute VB_Name = "dlgLoadSnap"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim m_AllUserPath As String
Dim m_UserPath As String
Dim m_LoadPath As String
Dim m_LoadFile As String

Private Sub Form_QueryUnload(Cancel As Integer, UnloadMode As Integer)
  On Error GoTo err
  Dim F As String
  If (Len(m_LoadPath) > 0) Then
    F = m_LoadPath & "\" & m_LoadFile
    g_Engine.LoadSnapShot F
  End If
  Exit Sub
  
err:
  MyMsgBox "Load Snapshot '" & F & "'" & err.Description
  Cancel = 1

End Sub

Private Sub m_AllUserSnaps_Click()
  If (m_AllUserSnaps.ListIndex >= 0) Then
    m_LoadPath = m_AllUserPath
    m_LoadFile = m_AllUserSnaps.List(m_AllUserSnaps.ListIndex)
  End If
  m_UserSnaps.ListIndex = -1
End Sub

Private Sub m_AllUserSnaps_DblClick()
  If (m_AllUserSnaps.ListIndex >= 0) Then
    m_LoadPath = m_AllUserPath
    m_LoadFile = m_AllUserSnaps.List(m_AllUserSnaps.ListIndex)
  End If
  m_UserSnaps.ListIndex = -1
  Unload Me
End Sub
Private Sub m_UserSnaps_Click()
  If (m_UserSnaps.ListIndex >= 0) Then
    m_LoadPath = m_UserPath
    m_LoadFile = m_UserSnaps.List(m_UserSnaps.ListIndex)
  End If
  m_AllUserSnaps.ListIndex = -1
End Sub

Private Sub m_UserSnaps_DblClick()
  If (m_UserSnaps.ListIndex >= 0) Then
    m_LoadPath = m_UserPath
    m_LoadFile = m_UserSnaps.List(m_UserSnaps.ListIndex)
  End If
  m_AllUserSnaps.ListIndex = -1
  Unload Me
End Sub

Private Sub m_CancelButton_Click()
  m_LoadPath = ""
  Unload Me
End Sub

Private Sub Form_Load()
  On Error GoTo err
  
  Dim Path, Fn As String
  Dim Attr As ScdFileAttributes
  Dim FErr As Long
  
  m_LoadPath = ""
  
  ' All user snapshots
  m_AllUserPath = g_User.AllUsersProfile
  m_AllUserPath = m_AllUserPath & SnapShotPath
  FErr = g_FileSystem.FindFirstFile(m_AllUserPath & "\*.*", Fn, Attr)
  Do While FErr = 0
    If (((Attr And ScdFile_DIRECTORY) <> 0) And Left(Fn, 1) <> ".") Then
      m_AllUserSnaps.AddItem (Fn)
    End If
    FErr = g_FileSystem.FindNextFile(Fn, Attr)
  Loop
    
  ' user snapshots
  m_UserPath = g_User.UserProfile
    m_UserPath = m_UserPath & SnapShotPath
  FErr = g_FileSystem.FindFirstFile(m_UserPath & "\*.*", Fn, Attr)
  Do While FErr = 0
    If (((Attr And ScdFile_DIRECTORY) <> 0) And Left(Fn, 1) <> ".") Then
      m_UserSnaps.AddItem (Fn)
    End If
    FErr = g_FileSystem.FindNextFile(Fn, Attr)
  Loop

  Exit Sub
err:
  MyMsgBox "Load Snapshot " & err.Description

End Sub

Private Sub m_OKButton_Click()
 
  Unload Me
End Sub

