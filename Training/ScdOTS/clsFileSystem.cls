VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "clsFileSystem"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Attribute VB_Ext_KEY = "SavedWithClassBuilder6" ,"Yes"
Attribute VB_Ext_KEY = "Top_Level" ,"Yes"
Option Explicit

Private m_PLCConfigPath As String
Private m_SimConfigPath As String
Private m_DCSConfigPath As String

Public Property Get PLCConfigPath() As String
  PLCConfigPath = m_PLCConfigPath
End Property
Public Property Let PLCConfigPath(Path As String)
  m_PLCConfigPath = Path
  g_Registry.PutKey ScdKey_SZ, ScdHKEY_CURRENT_USER, RegKeyRoot, "PLCConfigPath", m_PLCConfigPath
End Property
Public Property Get SimConfigPath() As String
  SimConfigPath = m_SimConfigPath
End Property
Public Property Let SimConfigPath(Path As String)
  m_SimConfigPath = Path
  g_Registry.PutKey ScdKey_SZ, ScdHKEY_CURRENT_USER, RegKeyRoot, "SimConfigPath", m_SimConfigPath
End Property
Public Property Get DCSConfigPath() As String
  DCSConfigPath = m_DCSConfigPath
End Property
Public Property Let DCSConfigPath(Path As String)
  m_DCSConfigPath = Path
  g_Registry.PutKey ScdKey_SZ, ScdHKEY_CURRENT_USER, RegKeyRoot, "DCSConfigPath", m_DCSConfigPath
End Property

Public Function ListAvailableICs() As Collection
Attribute ListAvailableICs.VB_Description = "Return initial conditions from user, all user and instructor."
End Function

Private Sub Class_Initialize()
  'm_SimConfigPath = "C:\SysCAD82\MosRef\TestVCS-00.spf\Project.spj"
  m_SimConfigPath = "C:\SysCAD82\MosRef\Refinery-07.spf\Project.spj"
  m_PLCConfigPath = "C:\SysCAD82\MosRef\EsdPlc\Esd.ppj"
  m_DCSConfigPath = "C:\SysCAD82\MosRef\HBCfgFiles\Data"
  
  Dim SimP, DcsP, PlcP As Variant
  
  SimP = m_SimConfigPath
  DcsP = m_DCSConfigPath
  PlcP = m_PLCConfigPath
  
  g_Registry.GetKey ScdKey_SZ, ScdHKEY_CURRENT_USER, RegKeyRoot, "SimConfigPath", SimP
  g_Registry.GetKey ScdKey_SZ, ScdHKEY_CURRENT_USER, RegKeyRoot, "PLCConfigPath", PlcP
  g_Registry.GetKey ScdKey_SZ, ScdHKEY_CURRENT_USER, RegKeyRoot, "DCSConfigPath", DcsP
  
  m_SimConfigPath = SimP
  m_DCSConfigPath = DcsP
  m_PLCConfigPath = PlcP
  
End Sub
Public Sub LoadModel()
  g_Engine.Initialize
  g_Engine.LoadProject m_PLCConfigPath, m_DCSConfigPath, m_SimConfigPath
End Sub

Public Sub UnloadModel()
  g_Engine.CloseProject
  g_Engine.Terminate
End Sub

