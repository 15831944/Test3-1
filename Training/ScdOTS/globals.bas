Attribute VB_Name = "globals"
Option Explicit


Public g_Engine As clsRunEngine
Public g_Files As clsFileSystem
Public g_User As ScdUserInfo
Public g_Registry As ScdRegistry
Public g_FileSystem As ScdFileSystem

Public Const ModelPath = "C:\SysCAD82\MosRef"

Public Const RegKeyRoot = "Software\Kenwalt\ScdOTS"

Public Const SnapShotPath = "\Application Data\Kenwalt\ScdOTS\Snapshots"
Public Const BacktrackPath = "\Application Data\Kenwalt\ScdOTS\BackTracks"

Public Const PLCSnapFolderFile = "\PLC"
Public Const DCSSnapFolderFile = "\DCS"
Public Const SimSnapFolderFile = "\SIM"

Public Function MyMsgBox(Msg As String, Optional Style As VbMsgBoxStyle = vbOKOnly) As VbMsgBoxResult
  MyMsgBox = MsgBox(Msg, Style)
End Function

