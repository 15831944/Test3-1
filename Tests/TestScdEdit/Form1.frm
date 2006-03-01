VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   3090
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   ScaleHeight     =   3090
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command1 
      Caption         =   "Command1"
      Height          =   855
      Left            =   360
      TabIndex        =   0
      Top             =   360
      Width           =   1455
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Public WithEvents m_Cmd As SysCADLib.SysCADCmd
Attribute m_Cmd.VB_VarHelpID = -1
Public m_Edit As SysCADLib.ISysCADEdit
Attribute m_Edit.VB_VarHelpID = -1

Dim xx As ScdStopWatch
Function Squr(X As Double) As Double
    Squr = X * X
End Function
Sub Swap(ByRef X, ByRef Y)
    Dim t As Variant
    t = X
    X = Y
    Y = t
End Sub
Private Sub Command1_Click()
    Set xx = New ScdStopWatch
    xx.Start
    
    Set m_Cmd = CreateObject("SysCAD.SysCADCmd.1")
    m_Cmd.SyncCallsOn = False
    m_Cmd.WaitUpNAbout 20
    m_Cmd.SyncCallsTimeOut = 20 ' LongTO
    m_Cmd.EventsOn = True

    Set m_Edit = m_Cmd

    Dim Tok() As String
    Dim Ln As String
    
    ' Nodes ----------------------------------------------------------------------
    Dim NTag(1000), NClass(1000) As String
    Dim NX(1000), NY(1000), NZ(1000) As Double
    Dim NdNo As Long
    Dim fso As New FileSystemObject, Nds As TextStream
    Set Nds = fso.OpenTextFile("c:\gmsi\nodeexporttest.csv", ForReading)
    
    Ln = Nds.ReadLine ' skip header
    NdNo = 0
    While (Not Nds.AtEndOfStream)
      Ln = Nds.ReadLine
      Tok = Split(Ln, ",")
      NX(NdNo) = Val(Tok(0))
      NY(NdNo) = Val(Tok(1))
      NZ(NdNo) = Val(Tok(2))
      NTag(NdNo) = Mid(Tok(3), 2, Len(Tok(3)) - 2)
      NClass(NdNo) = Mid(Tok(4), 2, Len(Tok(4)) - 2)
      If (Right(NTag(NdNo), 1) = "?") Then
        NTag(NdNo) = Left(NTag(NdNo), Len(NTag(NdNo)) - 1)
        NTag(NdNo) = NTag(NdNo) + Format(NdNo, "####0")
      End If
    
      m_Edit.InsertNode NClass(NdNo), NTag(NdNo), NX(NdNo), NY(NdNo), NZ(NdNo), 1, 1, 0
      NdNo = NdNo + 1
    Wend

    ' Sections -------------------------------------------------------------------
    Dim STag(1000), SClass(1000) As String
    Dim SXs(1000), SYs(1000), SZs(1000) As Double
    Dim SXd(1000), SYd(1000), SZd(1000) As Double
    Dim SLen(1000) As Double
    Dim Src(1000), Dst(1000) As Long
    Dim ScNo As Long
    Dim Sects As TextStream
    Set Sects = fso.OpenTextFile("c:\gmsi\Sectexporttest.csv", ForReading)
    
    Ln = Sects.ReadLine ' skip header
    ScNo = 0
    While (Not Sects.AtEndOfStream)
      Ln = Sects.ReadLine
      Tok = Split(Ln, ",")
      SXs(ScNo) = Val(Tok(0))
      SYs(ScNo) = Val(Tok(1))
      SZs(ScNo) = Val(Tok(2))
      SXd(ScNo) = Val(Tok(3))
      SYd(ScNo) = Val(Tok(4))
      SZd(ScNo) = Val(Tok(5))
      STag(ScNo) = "S" + Format(ScNo, "####0")
      SClass(ScNo) = "Pipe-1"
      
      Dim I As Long
      Dim MinDist, Dist As Double
      
      MinDist = 1E+100
      Src(ScNo) = -1
      For I = 1 To NdNo
        Dist = Sqr(Squr(NX(I) - SXs(ScNo)) + Squr(NY(I) - SYs(ScNo)) + Squr(NZ(I) - SZs(ScNo)))
        If (Dist < MinDist) Then
          Src(ScNo) = I
          MinDist = Dist
        End If
      Next I
      MinDist = 1E+100
      Dst(ScNo) = -1
      For I = 1 To NdNo
        Dist = Sqr(Squr(NX(I) - SXd(ScNo)) + Squr(NY(I) - SYd(ScNo)) + Squr(NZ(I) - SZd(ScNo)))
        If (Dist < MinDist) Then
          Dst(ScNo) = I
          MinDist = Dist
        End If
      Next I
      
      Dim SrcClass, SrcTag, SrcIO, DstClass, DstTag, DstIO As String
      SrcClass = NClass(Src(ScNo))
      SrcTag = NTag(Src(ScNo))
      SrcIO = "IO"
      DstClass = NClass(Dst(ScNo))
      DstTag = NTag(Dst(ScNo))
      DstIO = "IO"
      
      Dim DoSwap As Boolean
      DoSwap = False
      If (Left(SrcClass, 8) = "PipeTerm") Then
        SrcIO = "In"
        DstIO = "IO"
        DoSwap = True
      End If
      If (Left(DstClass, 8) = "PipeTerm") Then
        DstIO = "In"
      End If
      If (Left(DstClass, 6) = "Feed_X") Then
        DstIO = "Src"
        SrcIO = "IO"
        DoSwap = True
      End If
      If (Left(SrcClass, 6) = "Feed_X") Then
        SrcIO = "Src"
      End If
      
      If (DoSwap) Then
        Swap SrcClass, DstClass
        Swap SrcTag, DstTag
        Swap SrcIO, DstIO
      End If
      
      SLen(ScNo) = Sqr(Squr(SXs(ScNo) - SXs(ScNo)) + Squr(SYs(ScNo) - SYs(ScNo)) + Squr(SZs(ScNo) - SZs(ScNo)))
            
      Debug.Print SClass(ScNo), STag(ScNo), SrcTag + "." + SrcIO, DstTag + "." + DstIO
      
      m_Edit.InsertLink SClass(ScNo), STag(ScNo), SrcTag, SrcIO, DstTag, DstIO, SXs(ScNo), SYs(ScNo), SZs(ScNo), SXd(ScNo), SYd(ScNo), SZd(ScNo)
     
      ScNo = ScNo + 1
    Wend

    ' ??? ----------------------------------------------------------------------
    
End Sub
