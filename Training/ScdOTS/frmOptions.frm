VERSION 5.00
Begin VB.Form frmOptions 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   ")"
   ClientHeight    =   3195
   ClientLeft      =   2760
   ClientTop       =   3750
   ClientWidth     =   6030
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3195
   ScaleWidth      =   6030
   ShowInTaskbar   =   0   'False
   Begin VB.CheckBox m_PLCRqd 
      Caption         =   "PLC"
      Height          =   255
      Left            =   4680
      TabIndex        =   5
      Top             =   2760
      Width           =   1215
   End
   Begin VB.CheckBox m_DCSRqd 
      Caption         =   "DCS "
      Height          =   255
      Left            =   4680
      TabIndex        =   4
      Top             =   2400
      Width           =   1215
   End
   Begin VB.CheckBox m_SimRqd 
      Caption         =   "Simulator"
      Height          =   255
      Left            =   4680
      TabIndex        =   3
      Top             =   2040
      Width           =   1215
   End
   Begin VB.CommandButton m_CancelButton 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   4680
      TabIndex        =   1
      Top             =   600
      Width           =   1215
   End
   Begin VB.CommandButton m_OKButton 
      Caption         =   "OK"
      Height          =   375
      Left            =   4680
      TabIndex        =   0
      Top             =   120
      Width           =   1215
   End
   Begin VB.Label lblDefaultCondition 
      Caption         =   "Default Initial Condition"
      Height          =   735
      Left            =   0
      TabIndex        =   2
      Top             =   360
      Width           =   2295
   End
End
Attribute VB_Name = "frmOptions"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Option Explicit

Private Function Test(Val As Boolean, ValFalse As Long, ValTrue As Long) As Long
  If Val Then
    Test = ValTrue
  Else
    Test = ValFalse
  End If
End Function
Private Sub Form_Load()
  m_SimRqd.Value = Test(g_Engine.SimEngineReqd, 0, 1)
  m_DCSRqd.Value = Test(g_Engine.DCSEngineReqd, 0, 1)
  m_PLCRqd.Value = Test(g_Engine.PLCEngineReqd, 0, 1)
End Sub

Private Sub m_CancelButton_Click()
  Visible = False
End Sub

Private Sub m_OKButton_Click()
  g_Engine.SimEngineReqd = (m_SimRqd = 1)
  g_Engine.DCSEngineReqd = (m_DCSRqd = 1)
  g_Engine.PLCEngineReqd = (m_PLCRqd = 1)
  Visible = False
 End Sub

