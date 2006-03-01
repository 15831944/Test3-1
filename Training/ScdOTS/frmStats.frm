VERSION 5.00
Begin VB.Form frmStats 
   Caption         =   "Form1"
   ClientHeight    =   4710
   ClientLeft      =   9315
   ClientTop       =   6855
   ClientWidth     =   5775
   LinkTopic       =   "Form1"
   ScaleHeight     =   4710
   ScaleWidth      =   5775
   Begin VB.TextBox txtCycleTime 
      Height          =   285
      Left            =   1440
      TabIndex        =   3
      Text            =   "Text1"
      Top             =   360
      Width           =   1215
   End
   Begin VB.Frame Frame1 
      Caption         =   "Real Time stats"
      Height          =   3855
      Left            =   120
      TabIndex        =   1
      Top             =   120
      Width           =   5535
      Begin VB.Label Label1 
         Caption         =   "CycleTime"
         Height          =   255
         Left            =   240
         TabIndex        =   2
         Top             =   240
         Width           =   1095
      End
   End
   Begin VB.CommandButton cmdClose 
      Caption         =   "&Close"
      Height          =   495
      Left            =   4440
      TabIndex        =   0
      Top             =   4080
      Width           =   1215
   End
End
Attribute VB_Name = "frmStats"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit


Public Property Let CycleTime(newVal As Long)
    txtCycleTime.Text = newVal
End Property




Private Sub cmdClose_Click()
    Unload Me
End Sub












