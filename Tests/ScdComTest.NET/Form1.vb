Option Strict Off
Option Explicit On
Imports VB = Microsoft.VisualBasic
Imports ScdMdl
Imports ScdSlv
Imports ScdApp

Friend Class Form1
	Inherits System.Windows.Forms.Form
#Region "Windows Form Designer generated code "
	Public Sub New()
		MyBase.New()
		If m_vb6FormDefInstance Is Nothing Then
			If m_InitializingDefInstance Then
				m_vb6FormDefInstance = Me
			Else
				Try 
					'For the start-up form, the first instance created is the default instance.
					If System.Reflection.Assembly.GetExecutingAssembly.EntryPoint.DeclaringType Is Me.GetType Then
						m_vb6FormDefInstance = Me
					End If
				Catch
				End Try
			End If
		End If
		'This call is required by the Windows Form Designer.
		InitializeComponent()
	End Sub
	'Form overrides dispose to clean up the component list.
	Protected Overloads Overrides Sub Dispose(ByVal Disposing As Boolean)
		If Disposing Then
			If Not components Is Nothing Then
				components.Dispose()
			End If
		End If
		MyBase.Dispose(Disposing)
	End Sub
	'Required by the Windows Form Designer
	Private components As System.ComponentModel.IContainer
	Public ToolTip1 As System.Windows.Forms.ToolTip
	Public WithEvents m_sPress As System.Windows.Forms.TextBox
	Public WithEvents m_sTemp As System.Windows.Forms.TextBox
	Public WithEvents Label4 As System.Windows.Forms.Label
	Public WithEvents Label3 As System.Windows.Forms.Label
	Public WithEvents Frame5 As System.Windows.Forms.GroupBox
	Public WithEvents DoSleep As System.Windows.Forms.Button
	Public WithEvents List1 As System.Windows.Forms.ListBox
	Public WithEvents WideList As System.Windows.Forms.Button
	Public WithEvents ScdOpn As System.Windows.Forms.CheckBox
	Public WithEvents OpenCloseLic As System.Windows.Forms.Button
	Public WithEvents CfgFileBox As System.Windows.Forms.TextBox
	Public WithEvents RootPath As System.Windows.Forms.TextBox
	Public WithEvents SpecieDefn As System.Windows.Forms.Button
	Public WithEvents ClearList As System.Windows.Forms.Button
	Public WithEvents CreateFlwSheet As System.Windows.Forms.Button
	Public WithEvents Demo As System.Windows.Forms.Button
	Public WithEvents LoadTestFlwSheet As System.Windows.Forms.Button
	Public WithEvents VecDefn As System.Windows.Forms.Button
	Public WithEvents Conversions As System.Windows.Forms.Button
	Public WithEvents Frame1 As System.Windows.Forms.GroupBox
	Public WithEvents VecOther As System.Windows.Forms.Button
	Public WithEvents ABayer As System.Windows.Forms.Button
	Public WithEvents Standard As System.Windows.Forms.Button
	Public WithEvents Frame4 As System.Windows.Forms.GroupBox
	Public WithEvents List2 As System.Windows.Forms.ListBox
	Public WithEvents CreateElec As System.Windows.Forms.Button
	Public WithEvents CloseNoSave As System.Windows.Forms.Button
	Public WithEvents QueryInfo As System.Windows.Forms.Button
	Public WithEvents CloseSave As System.Windows.Forms.Button
	Public WithEvents LicOpn As System.Windows.Forms.CheckBox
	Public WithEvents OpenCloseApp As System.Windows.Forms.Button
	Public WithEvents ReleaseAll As System.Windows.Forms.Button
	Public WithEvents Frame2 As System.Windows.Forms.GroupBox
	Public WithEvents Frame3 As System.Windows.Forms.GroupBox
	Public WithEvents Label1 As System.Windows.Forms.Label
	Public WithEvents Label2 As System.Windows.Forms.Label
	'NOTE: The following procedure is required by the Windows Form Designer
	'It can be modified using the Windows Form Designer.
	'Do not modify it using the code editor.
  Friend WithEvents FlwsheetExport As System.Windows.Forms.Button
  Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
  Friend WithEvents ImportUser As System.Windows.Forms.Button
  Friend WithEvents FlwsheetImport As System.Windows.Forms.Button
  Friend WithEvents ExportBlank As System.Windows.Forms.Button
  Public WithEvents LdAndRunMineServe As System.Windows.Forms.Button
  <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
    Me.components = New System.ComponentModel.Container
    Me.ToolTip1 = New System.Windows.Forms.ToolTip(Me.components)
    Me.Frame5 = New System.Windows.Forms.GroupBox
    Me.m_sPress = New System.Windows.Forms.TextBox
    Me.m_sTemp = New System.Windows.Forms.TextBox
    Me.Label4 = New System.Windows.Forms.Label
    Me.Label3 = New System.Windows.Forms.Label
    Me.DoSleep = New System.Windows.Forms.Button
    Me.List1 = New System.Windows.Forms.ListBox
    Me.WideList = New System.Windows.Forms.Button
    Me.ScdOpn = New System.Windows.Forms.CheckBox
    Me.OpenCloseLic = New System.Windows.Forms.Button
    Me.CfgFileBox = New System.Windows.Forms.TextBox
    Me.RootPath = New System.Windows.Forms.TextBox
    Me.SpecieDefn = New System.Windows.Forms.Button
    Me.ClearList = New System.Windows.Forms.Button
    Me.CreateFlwSheet = New System.Windows.Forms.Button
    Me.Demo = New System.Windows.Forms.Button
    Me.LoadTestFlwSheet = New System.Windows.Forms.Button
    Me.Frame1 = New System.Windows.Forms.GroupBox
    Me.VecDefn = New System.Windows.Forms.Button
    Me.Conversions = New System.Windows.Forms.Button
    Me.Frame4 = New System.Windows.Forms.GroupBox
    Me.VecOther = New System.Windows.Forms.Button
    Me.ABayer = New System.Windows.Forms.Button
    Me.Standard = New System.Windows.Forms.Button
    Me.List2 = New System.Windows.Forms.ListBox
    Me.Frame3 = New System.Windows.Forms.GroupBox
    Me.CreateElec = New System.Windows.Forms.Button
    Me.CloseNoSave = New System.Windows.Forms.Button
    Me.QueryInfo = New System.Windows.Forms.Button
    Me.CloseSave = New System.Windows.Forms.Button
    Me.LicOpn = New System.Windows.Forms.CheckBox
    Me.OpenCloseApp = New System.Windows.Forms.Button
    Me.Frame2 = New System.Windows.Forms.GroupBox
    Me.ReleaseAll = New System.Windows.Forms.Button
    Me.GroupBox1 = New System.Windows.Forms.GroupBox
    Me.ExportBlank = New System.Windows.Forms.Button
    Me.FlwsheetImport = New System.Windows.Forms.Button
    Me.FlwsheetExport = New System.Windows.Forms.Button
    Me.ImportUser = New System.Windows.Forms.Button
    Me.Label1 = New System.Windows.Forms.Label
    Me.Label2 = New System.Windows.Forms.Label
    Me.LdAndRunMineServe = New System.Windows.Forms.Button
    Me.Frame5.SuspendLayout()
    Me.Frame1.SuspendLayout()
    Me.Frame4.SuspendLayout()
    Me.Frame3.SuspendLayout()
    Me.Frame2.SuspendLayout()
    Me.GroupBox1.SuspendLayout()
    Me.SuspendLayout()
    '
    'Frame5
    '
    Me.Frame5.BackColor = System.Drawing.SystemColors.Control
    Me.Frame5.Controls.Add(Me.m_sPress)
    Me.Frame5.Controls.Add(Me.m_sTemp)
    Me.Frame5.Controls.Add(Me.Label4)
    Me.Frame5.Controls.Add(Me.Label3)
    Me.Frame5.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.Frame5.ForeColor = System.Drawing.SystemColors.ControlText
    Me.Frame5.Location = New System.Drawing.Point(672, 48)
    Me.Frame5.Name = "Frame5"
    Me.Frame5.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.Frame5.Size = New System.Drawing.Size(145, 73)
    Me.Frame5.TabIndex = 22
    Me.Frame5.TabStop = False
    Me.Frame5.Text = "Conditions"
    '
    'm_sPress
    '
    Me.m_sPress.AcceptsReturn = True
    Me.m_sPress.AutoSize = False
    Me.m_sPress.BackColor = System.Drawing.SystemColors.Window
    Me.m_sPress.Cursor = System.Windows.Forms.Cursors.IBeam
    Me.m_sPress.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.m_sPress.ForeColor = System.Drawing.SystemColors.WindowText
    Me.m_sPress.Location = New System.Drawing.Point(80, 40)
    Me.m_sPress.MaxLength = 0
    Me.m_sPress.Name = "m_sPress"
    Me.m_sPress.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.m_sPress.Size = New System.Drawing.Size(57, 19)
    Me.m_sPress.TabIndex = 26
    Me.m_sPress.Text = "0"
    '
    'm_sTemp
    '
    Me.m_sTemp.AcceptsReturn = True
    Me.m_sTemp.AutoSize = False
    Me.m_sTemp.BackColor = System.Drawing.SystemColors.Window
    Me.m_sTemp.Cursor = System.Windows.Forms.Cursors.IBeam
    Me.m_sTemp.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.m_sTemp.ForeColor = System.Drawing.SystemColors.WindowText
    Me.m_sTemp.Location = New System.Drawing.Point(80, 16)
    Me.m_sTemp.MaxLength = 0
    Me.m_sTemp.Name = "m_sTemp"
    Me.m_sTemp.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.m_sTemp.Size = New System.Drawing.Size(57, 19)
    Me.m_sTemp.TabIndex = 25
    Me.m_sTemp.Text = "25"
    '
    'Label4
    '
    Me.Label4.BackColor = System.Drawing.SystemColors.Control
    Me.Label4.Cursor = System.Windows.Forms.Cursors.Default
    Me.Label4.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.Label4.ForeColor = System.Drawing.SystemColors.ControlText
    Me.Label4.Location = New System.Drawing.Point(8, 40)
    Me.Label4.Name = "Label4"
    Me.Label4.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.Label4.Size = New System.Drawing.Size(57, 17)
    Me.Label4.TabIndex = 24
    Me.Label4.Text = "Press kPag"
    '
    'Label3
    '
    Me.Label3.BackColor = System.Drawing.SystemColors.Control
    Me.Label3.Cursor = System.Windows.Forms.Cursors.Default
    Me.Label3.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.Label3.ForeColor = System.Drawing.SystemColors.ControlText
    Me.Label3.Location = New System.Drawing.Point(8, 16)
    Me.Label3.Name = "Label3"
    Me.Label3.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.Label3.Size = New System.Drawing.Size(57, 17)
    Me.Label3.TabIndex = 23
    Me.Label3.Text = "Temp C"
    '
    'DoSleep
    '
    Me.DoSleep.BackColor = System.Drawing.SystemColors.Control
    Me.DoSleep.Cursor = System.Windows.Forms.Cursors.Default
    Me.DoSleep.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.DoSleep.ForeColor = System.Drawing.SystemColors.ControlText
    Me.DoSleep.Location = New System.Drawing.Point(744, 128)
    Me.DoSleep.Name = "DoSleep"
    Me.DoSleep.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.DoSleep.Size = New System.Drawing.Size(73, 25)
    Me.DoSleep.TabIndex = 21
    Me.DoSleep.Text = "Sleep"
    '
    'List1
    '
    Me.List1.BackColor = System.Drawing.SystemColors.Window
    Me.List1.Cursor = System.Windows.Forms.Cursors.Default
    Me.List1.Font = New System.Drawing.Font("Lucida Console", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.List1.ForeColor = System.Drawing.SystemColors.WindowText
    Me.List1.ItemHeight = 11
    Me.List1.Location = New System.Drawing.Point(8, 280)
    Me.List1.Name = "List1"
    Me.List1.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.List1.Size = New System.Drawing.Size(401, 521)
    Me.List1.TabIndex = 2
    '
    'WideList
    '
    Me.WideList.BackColor = System.Drawing.SystemColors.Control
    Me.WideList.Cursor = System.Windows.Forms.Cursors.Default
    Me.WideList.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.WideList.ForeColor = System.Drawing.SystemColors.ControlText
    Me.WideList.Location = New System.Drawing.Point(664, 160)
    Me.WideList.Name = "WideList"
    Me.WideList.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.WideList.Size = New System.Drawing.Size(73, 25)
    Me.WideList.TabIndex = 20
    Me.WideList.Text = "WideList"
    '
    'ScdOpn
    '
    Me.ScdOpn.BackColor = System.Drawing.SystemColors.Control
    Me.ScdOpn.Cursor = System.Windows.Forms.Cursors.Default
    Me.ScdOpn.Enabled = False
    Me.ScdOpn.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.ScdOpn.ForeColor = System.Drawing.SystemColors.ControlText
    Me.ScdOpn.Location = New System.Drawing.Point(272, 56)
    Me.ScdOpn.Name = "ScdOpn"
    Me.ScdOpn.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.ScdOpn.Size = New System.Drawing.Size(17, 25)
    Me.ScdOpn.TabIndex = 18
    '
    'OpenCloseLic
    '
    Me.OpenCloseLic.BackColor = System.Drawing.SystemColors.Control
    Me.OpenCloseLic.Cursor = System.Windows.Forms.Cursors.Default
    Me.OpenCloseLic.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.OpenCloseLic.ForeColor = System.Drawing.SystemColors.ControlText
    Me.OpenCloseLic.Location = New System.Drawing.Point(288, 88)
    Me.OpenCloseLic.Name = "OpenCloseLic"
    Me.OpenCloseLic.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.OpenCloseLic.Size = New System.Drawing.Size(97, 25)
    Me.OpenCloseLic.TabIndex = 17
    Me.OpenCloseLic.Text = "Open/Close Lic"
    '
    'CfgFileBox
    '
    Me.CfgFileBox.AcceptsReturn = True
    Me.CfgFileBox.AutoSize = False
    Me.CfgFileBox.BackColor = System.Drawing.SystemColors.Window
    Me.CfgFileBox.Cursor = System.Windows.Forms.Cursors.IBeam
    Me.CfgFileBox.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.CfgFileBox.ForeColor = System.Drawing.SystemColors.WindowText
    Me.CfgFileBox.Location = New System.Drawing.Point(416, 8)
    Me.CfgFileBox.MaxLength = 0
    Me.CfgFileBox.Name = "CfgFileBox"
    Me.CfgFileBox.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.CfgFileBox.Size = New System.Drawing.Size(297, 19)
    Me.CfgFileBox.TabIndex = 15
    Me.CfgFileBox.Text = "\SysCAD82\xxx\xxx.cfg"
    '
    'RootPath
    '
    Me.RootPath.AcceptsReturn = True
    Me.RootPath.AutoSize = False
    Me.RootPath.BackColor = System.Drawing.SystemColors.Window
    Me.RootPath.Cursor = System.Windows.Forms.Cursors.IBeam
    Me.RootPath.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.RootPath.ForeColor = System.Drawing.SystemColors.WindowText
    Me.RootPath.Location = New System.Drawing.Point(48, 8)
    Me.RootPath.MaxLength = 0
    Me.RootPath.Name = "RootPath"
    Me.RootPath.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.RootPath.Size = New System.Drawing.Size(313, 19)
    Me.RootPath.TabIndex = 11
    Me.RootPath.Text = "D:"
    '
    'SpecieDefn
    '
    Me.SpecieDefn.BackColor = System.Drawing.SystemColors.Control
    Me.SpecieDefn.Cursor = System.Windows.Forms.Cursors.Default
    Me.SpecieDefn.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.SpecieDefn.ForeColor = System.Drawing.SystemColors.ControlText
    Me.SpecieDefn.Location = New System.Drawing.Point(16, 88)
    Me.SpecieDefn.Name = "SpecieDefn"
    Me.SpecieDefn.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.SpecieDefn.Size = New System.Drawing.Size(105, 25)
    Me.SpecieDefn.TabIndex = 5
    Me.SpecieDefn.Text = "Specie Defn"
    '
    'ClearList
    '
    Me.ClearList.BackColor = System.Drawing.SystemColors.Control
    Me.ClearList.Cursor = System.Windows.Forms.Cursors.Default
    Me.ClearList.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.ClearList.ForeColor = System.Drawing.SystemColors.ControlText
    Me.ClearList.Location = New System.Drawing.Point(744, 160)
    Me.ClearList.Name = "ClearList"
    Me.ClearList.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.ClearList.Size = New System.Drawing.Size(73, 25)
    Me.ClearList.TabIndex = 4
    Me.ClearList.Text = "Clear Lists"
    '
    'CreateFlwSheet
    '
    Me.CreateFlwSheet.BackColor = System.Drawing.SystemColors.Control
    Me.CreateFlwSheet.Cursor = System.Windows.Forms.Cursors.Default
    Me.CreateFlwSheet.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.CreateFlwSheet.ForeColor = System.Drawing.SystemColors.ControlText
    Me.CreateFlwSheet.Location = New System.Drawing.Point(392, 88)
    Me.CreateFlwSheet.Name = "CreateFlwSheet"
    Me.CreateFlwSheet.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.CreateFlwSheet.Size = New System.Drawing.Size(113, 25)
    Me.CreateFlwSheet.TabIndex = 3
    Me.CreateFlwSheet.Text = "Create Flwsheet"
    '
    'Demo
    '
    Me.Demo.BackColor = System.Drawing.SystemColors.Control
    Me.Demo.Cursor = System.Windows.Forms.Cursors.Default
    Me.Demo.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.Demo.ForeColor = System.Drawing.SystemColors.ControlText
    Me.Demo.Location = New System.Drawing.Point(144, 88)
    Me.Demo.Name = "Demo"
    Me.Demo.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.Demo.Size = New System.Drawing.Size(105, 25)
    Me.Demo.TabIndex = 1
    Me.Demo.Text = "Demo"
    '
    'LoadTestFlwSheet
    '
    Me.LoadTestFlwSheet.BackColor = System.Drawing.SystemColors.Control
    Me.LoadTestFlwSheet.Cursor = System.Windows.Forms.Cursors.Default
    Me.LoadTestFlwSheet.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.LoadTestFlwSheet.ForeColor = System.Drawing.SystemColors.ControlText
    Me.LoadTestFlwSheet.Location = New System.Drawing.Point(392, 56)
    Me.LoadTestFlwSheet.Name = "LoadTestFlwSheet"
    Me.LoadTestFlwSheet.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.LoadTestFlwSheet.Size = New System.Drawing.Size(113, 25)
    Me.LoadTestFlwSheet.TabIndex = 0
    Me.LoadTestFlwSheet.Text = "Load Test Flwsheet"
    '
    'Frame1
    '
    Me.Frame1.BackColor = System.Drawing.SystemColors.Control
    Me.Frame1.Controls.Add(Me.VecDefn)
    Me.Frame1.Controls.Add(Me.Conversions)
    Me.Frame1.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.Frame1.ForeColor = System.Drawing.SystemColors.ControlText
    Me.Frame1.Location = New System.Drawing.Point(8, 32)
    Me.Frame1.Name = "Frame1"
    Me.Frame1.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.Frame1.Size = New System.Drawing.Size(121, 153)
    Me.Frame1.TabIndex = 7
    Me.Frame1.TabStop = False
    Me.Frame1.Text = "Basic"
    '
    'VecDefn
    '
    Me.VecDefn.BackColor = System.Drawing.SystemColors.Control
    Me.VecDefn.Cursor = System.Windows.Forms.Cursors.Default
    Me.VecDefn.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.VecDefn.ForeColor = System.Drawing.SystemColors.ControlText
    Me.VecDefn.Location = New System.Drawing.Point(8, 88)
    Me.VecDefn.Name = "VecDefn"
    Me.VecDefn.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.VecDefn.Size = New System.Drawing.Size(105, 25)
    Me.VecDefn.TabIndex = 33
    Me.VecDefn.Text = "VecDefn"
    '
    'Conversions
    '
    Me.Conversions.BackColor = System.Drawing.SystemColors.Control
    Me.Conversions.Cursor = System.Windows.Forms.Cursors.Default
    Me.Conversions.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.Conversions.ForeColor = System.Drawing.SystemColors.ControlText
    Me.Conversions.Location = New System.Drawing.Point(8, 24)
    Me.Conversions.Name = "Conversions"
    Me.Conversions.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.Conversions.Size = New System.Drawing.Size(105, 25)
    Me.Conversions.TabIndex = 31
    Me.Conversions.Text = "Conversions"
    '
    'Frame4
    '
    Me.Frame4.BackColor = System.Drawing.SystemColors.Control
    Me.Frame4.Controls.Add(Me.VecOther)
    Me.Frame4.Controls.Add(Me.ABayer)
    Me.Frame4.Controls.Add(Me.Standard)
    Me.Frame4.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.Frame4.ForeColor = System.Drawing.SystemColors.ControlText
    Me.Frame4.Location = New System.Drawing.Point(136, 32)
    Me.Frame4.Name = "Frame4"
    Me.Frame4.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.Frame4.Size = New System.Drawing.Size(121, 153)
    Me.Frame4.TabIndex = 12
    Me.Frame4.TabStop = False
    Me.Frame4.Text = "Specie Vector"
    '
    'VecOther
    '
    Me.VecOther.BackColor = System.Drawing.SystemColors.Control
    Me.VecOther.Cursor = System.Windows.Forms.Cursors.Default
    Me.VecOther.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.VecOther.ForeColor = System.Drawing.SystemColors.ControlText
    Me.VecOther.Location = New System.Drawing.Point(8, 120)
    Me.VecOther.Name = "VecOther"
    Me.VecOther.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.VecOther.Size = New System.Drawing.Size(105, 25)
    Me.VecOther.TabIndex = 32
    Me.VecOther.Text = "Other"
    '
    'ABayer
    '
    Me.ABayer.BackColor = System.Drawing.SystemColors.Control
    Me.ABayer.Cursor = System.Windows.Forms.Cursors.Default
    Me.ABayer.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.ABayer.ForeColor = System.Drawing.SystemColors.ControlText
    Me.ABayer.Location = New System.Drawing.Point(8, 88)
    Me.ABayer.Name = "ABayer"
    Me.ABayer.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.ABayer.Size = New System.Drawing.Size(105, 25)
    Me.ABayer.TabIndex = 16
    Me.ABayer.Text = "ABayer"
    '
    'Standard
    '
    Me.Standard.BackColor = System.Drawing.SystemColors.Control
    Me.Standard.Cursor = System.Windows.Forms.Cursors.Default
    Me.Standard.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.Standard.ForeColor = System.Drawing.SystemColors.ControlText
    Me.Standard.Location = New System.Drawing.Point(8, 24)
    Me.Standard.Name = "Standard"
    Me.Standard.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.Standard.Size = New System.Drawing.Size(105, 25)
    Me.Standard.TabIndex = 13
    Me.Standard.Text = "Standard"
    '
    'List2
    '
    Me.List2.BackColor = System.Drawing.SystemColors.Window
    Me.List2.Cursor = System.Windows.Forms.Cursors.Default
    Me.List2.Font = New System.Drawing.Font("Lucida Console", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.List2.ForeColor = System.Drawing.SystemColors.WindowText
    Me.List2.ItemHeight = 11
    Me.List2.Location = New System.Drawing.Point(424, 280)
    Me.List2.Name = "List2"
    Me.List2.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.List2.Size = New System.Drawing.Size(393, 521)
    Me.List2.TabIndex = 6
    '
    'Frame3
    '
    Me.Frame3.BackColor = System.Drawing.SystemColors.Control
    Me.Frame3.Controls.Add(Me.CreateElec)
    Me.Frame3.Controls.Add(Me.CloseNoSave)
    Me.Frame3.Controls.Add(Me.QueryInfo)
    Me.Frame3.Controls.Add(Me.CloseSave)
    Me.Frame3.Controls.Add(Me.LicOpn)
    Me.Frame3.Controls.Add(Me.OpenCloseApp)
    Me.Frame3.Controls.Add(Me.Frame2)
    Me.Frame3.Controls.Add(Me.GroupBox1)
    Me.Frame3.Controls.Add(Me.LdAndRunMineServe)
    Me.Frame3.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.Frame3.ForeColor = System.Drawing.SystemColors.ControlText
    Me.Frame3.Location = New System.Drawing.Point(264, 40)
    Me.Frame3.Name = "Frame3"
    Me.Frame3.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.Frame3.Size = New System.Drawing.Size(385, 232)
    Me.Frame3.TabIndex = 8
    Me.Frame3.TabStop = False
    Me.Frame3.Text = "Application"
    '
    'CreateElec
    '
    Me.CreateElec.BackColor = System.Drawing.SystemColors.Control
    Me.CreateElec.Cursor = System.Windows.Forms.Cursors.Default
    Me.CreateElec.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.CreateElec.ForeColor = System.Drawing.SystemColors.ControlText
    Me.CreateElec.Location = New System.Drawing.Point(128, 80)
    Me.CreateElec.Name = "CreateElec"
    Me.CreateElec.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.CreateElec.Size = New System.Drawing.Size(113, 25)
    Me.CreateElec.TabIndex = 35
    Me.CreateElec.Text = "Create Elec Flwsht"
    '
    'CloseNoSave
    '
    Me.CloseNoSave.BackColor = System.Drawing.SystemColors.Control
    Me.CloseNoSave.Cursor = System.Windows.Forms.Cursors.Default
    Me.CloseNoSave.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.CloseNoSave.ForeColor = System.Drawing.SystemColors.ControlText
    Me.CloseNoSave.Location = New System.Drawing.Point(320, 96)
    Me.CloseNoSave.Name = "CloseNoSave"
    Me.CloseNoSave.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.CloseNoSave.Size = New System.Drawing.Size(57, 25)
    Me.CloseNoSave.TabIndex = 29
    Me.CloseNoSave.Text = "NoSave"
    '
    'QueryInfo
    '
    Me.QueryInfo.BackColor = System.Drawing.SystemColors.Control
    Me.QueryInfo.Cursor = System.Windows.Forms.Cursors.Default
    Me.QueryInfo.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.QueryInfo.ForeColor = System.Drawing.SystemColors.ControlText
    Me.QueryInfo.Location = New System.Drawing.Point(248, 16)
    Me.QueryInfo.Name = "QueryInfo"
    Me.QueryInfo.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.QueryInfo.Size = New System.Drawing.Size(113, 25)
    Me.QueryInfo.TabIndex = 28
    Me.QueryInfo.Text = "Query Info"
    '
    'CloseSave
    '
    Me.CloseSave.BackColor = System.Drawing.SystemColors.Control
    Me.CloseSave.Cursor = System.Windows.Forms.Cursors.Default
    Me.CloseSave.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.CloseSave.ForeColor = System.Drawing.SystemColors.ControlText
    Me.CloseSave.Location = New System.Drawing.Point(320, 72)
    Me.CloseSave.Name = "CloseSave"
    Me.CloseSave.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.CloseSave.Size = New System.Drawing.Size(57, 25)
    Me.CloseSave.TabIndex = 27
    Me.CloseSave.Text = "Save"
    '
    'LicOpn
    '
    Me.LicOpn.BackColor = System.Drawing.SystemColors.Control
    Me.LicOpn.Cursor = System.Windows.Forms.Cursors.Default
    Me.LicOpn.Enabled = False
    Me.LicOpn.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.LicOpn.ForeColor = System.Drawing.SystemColors.ControlText
    Me.LicOpn.Location = New System.Drawing.Point(8, 48)
    Me.LicOpn.Name = "LicOpn"
    Me.LicOpn.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.LicOpn.Size = New System.Drawing.Size(17, 25)
    Me.LicOpn.TabIndex = 19
    '
    'OpenCloseApp
    '
    Me.OpenCloseApp.BackColor = System.Drawing.SystemColors.Control
    Me.OpenCloseApp.Cursor = System.Windows.Forms.Cursors.Default
    Me.OpenCloseApp.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.OpenCloseApp.ForeColor = System.Drawing.SystemColors.ControlText
    Me.OpenCloseApp.Location = New System.Drawing.Point(24, 16)
    Me.OpenCloseApp.Name = "OpenCloseApp"
    Me.OpenCloseApp.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.OpenCloseApp.Size = New System.Drawing.Size(97, 25)
    Me.OpenCloseApp.TabIndex = 9
    Me.OpenCloseApp.Text = "Open/Close Scd"
    '
    'Frame2
    '
    Me.Frame2.BackColor = System.Drawing.SystemColors.Control
    Me.Frame2.Controls.Add(Me.ReleaseAll)
    Me.Frame2.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.Frame2.ForeColor = System.Drawing.SystemColors.ControlText
    Me.Frame2.Location = New System.Drawing.Point(312, 56)
    Me.Frame2.Name = "Frame2"
    Me.Frame2.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.Frame2.Size = New System.Drawing.Size(73, 97)
    Me.Frame2.TabIndex = 30
    Me.Frame2.TabStop = False
    Me.Frame2.Text = "Close"
    '
    'ReleaseAll
    '
    Me.ReleaseAll.BackColor = System.Drawing.SystemColors.Control
    Me.ReleaseAll.Cursor = System.Windows.Forms.Cursors.Default
    Me.ReleaseAll.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.ReleaseAll.ForeColor = System.Drawing.SystemColors.ControlText
    Me.ReleaseAll.Location = New System.Drawing.Point(8, 64)
    Me.ReleaseAll.Name = "ReleaseAll"
    Me.ReleaseAll.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.ReleaseAll.Size = New System.Drawing.Size(57, 25)
    Me.ReleaseAll.TabIndex = 34
    Me.ReleaseAll.Text = "Release"
    '
    'GroupBox1
    '
    Me.GroupBox1.Controls.Add(Me.ExportBlank)
    Me.GroupBox1.Controls.Add(Me.FlwsheetImport)
    Me.GroupBox1.Controls.Add(Me.FlwsheetExport)
    Me.GroupBox1.Controls.Add(Me.ImportUser)
    Me.GroupBox1.Location = New System.Drawing.Point(8, 144)
    Me.GroupBox1.Name = "GroupBox1"
    Me.GroupBox1.Size = New System.Drawing.Size(200, 80)
    Me.GroupBox1.TabIndex = 38
    Me.GroupBox1.TabStop = False
    Me.GroupBox1.Text = "Neutral DB"
    '
    'ExportBlank
    '
    Me.ExportBlank.Location = New System.Drawing.Point(8, 48)
    Me.ExportBlank.Name = "ExportBlank"
    Me.ExportBlank.Size = New System.Drawing.Size(88, 24)
    Me.ExportBlank.TabIndex = 41
    Me.ExportBlank.Text = "Export Blank"
    '
    'FlwsheetImport
    '
    Me.FlwsheetImport.Location = New System.Drawing.Point(104, 16)
    Me.FlwsheetImport.Name = "FlwsheetImport"
    Me.FlwsheetImport.Size = New System.Drawing.Size(88, 24)
    Me.FlwsheetImport.TabIndex = 40
    Me.FlwsheetImport.Text = "Import "
    '
    'FlwsheetExport
    '
    Me.FlwsheetExport.Location = New System.Drawing.Point(8, 16)
    Me.FlwsheetExport.Name = "FlwsheetExport"
    Me.FlwsheetExport.Size = New System.Drawing.Size(88, 24)
    Me.FlwsheetExport.TabIndex = 36
    Me.FlwsheetExport.Text = "Build/Export"
    '
    'ImportUser
    '
    Me.ImportUser.Location = New System.Drawing.Point(104, 48)
    Me.ImportUser.Name = "ImportUser"
    Me.ImportUser.Size = New System.Drawing.Size(88, 24)
    Me.ImportUser.TabIndex = 39
    Me.ImportUser.Text = "Import User"
    '
    'Label1
    '
    Me.Label1.BackColor = System.Drawing.SystemColors.Control
    Me.Label1.Cursor = System.Windows.Forms.Cursors.Default
    Me.Label1.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.Label1.ForeColor = System.Drawing.SystemColors.ControlText
    Me.Label1.Location = New System.Drawing.Point(368, 8)
    Me.Label1.Name = "Label1"
    Me.Label1.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.Label1.Size = New System.Drawing.Size(41, 17)
    Me.Label1.TabIndex = 14
    Me.Label1.Text = "CfgFile:"
    '
    'Label2
    '
    Me.Label2.BackColor = System.Drawing.SystemColors.Control
    Me.Label2.Cursor = System.Windows.Forms.Cursors.Default
    Me.Label2.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.Label2.ForeColor = System.Drawing.SystemColors.ControlText
    Me.Label2.Location = New System.Drawing.Point(8, 8)
    Me.Label2.Name = "Label2"
    Me.Label2.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.Label2.Size = New System.Drawing.Size(33, 17)
    Me.Label2.TabIndex = 10
    Me.Label2.Text = "Path:"
    '
    'LdAndRunMineServe
    '
    Me.LdAndRunMineServe.BackColor = System.Drawing.SystemColors.Control
    Me.LdAndRunMineServe.Cursor = System.Windows.Forms.Cursors.Default
    Me.LdAndRunMineServe.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.LdAndRunMineServe.ForeColor = System.Drawing.SystemColors.ControlText
    Me.LdAndRunMineServe.Location = New System.Drawing.Point(256, 192)
    Me.LdAndRunMineServe.Name = "LdAndRunMineServe"
    Me.LdAndRunMineServe.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.LdAndRunMineServe.Size = New System.Drawing.Size(120, 25)
    Me.LdAndRunMineServe.TabIndex = 23
    Me.LdAndRunMineServe.Text = "Ld && Rn MineServe"
    '
    'Form1
    '
    Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
    Me.BackColor = System.Drawing.SystemColors.Control
    Me.ClientSize = New System.Drawing.Size(827, 806)
    Me.Controls.Add(Me.Frame5)
    Me.Controls.Add(Me.DoSleep)
    Me.Controls.Add(Me.List1)
    Me.Controls.Add(Me.WideList)
    Me.Controls.Add(Me.ScdOpn)
    Me.Controls.Add(Me.OpenCloseLic)
    Me.Controls.Add(Me.CfgFileBox)
    Me.Controls.Add(Me.RootPath)
    Me.Controls.Add(Me.SpecieDefn)
    Me.Controls.Add(Me.ClearList)
    Me.Controls.Add(Me.CreateFlwSheet)
    Me.Controls.Add(Me.Demo)
    Me.Controls.Add(Me.LoadTestFlwSheet)
    Me.Controls.Add(Me.Frame1)
    Me.Controls.Add(Me.Frame4)
    Me.Controls.Add(Me.List2)
    Me.Controls.Add(Me.Frame3)
    Me.Controls.Add(Me.Label1)
    Me.Controls.Add(Me.Label2)
    Me.Cursor = System.Windows.Forms.Cursors.Default
    Me.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
    Me.Location = New System.Drawing.Point(144, 53)
    Me.Name = "Form1"
    Me.RightToLeft = System.Windows.Forms.RightToLeft.No
    Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
    Me.Text = "Scd COM Test"
    Me.Frame5.ResumeLayout(False)
    Me.Frame1.ResumeLayout(False)
    Me.Frame4.ResumeLayout(False)
    Me.Frame3.ResumeLayout(False)
    Me.Frame2.ResumeLayout(False)
    Me.GroupBox1.ResumeLayout(False)
    Me.ResumeLayout(False)

  End Sub
#End Region 
#Region "Upgrade Support "
	Private Shared m_vb6FormDefInstance As Form1
	Private Shared m_InitializingDefInstance As Boolean
	Public Shared Property DefInstance() As Form1
		Get
			If m_vb6FormDefInstance Is Nothing OrElse m_vb6FormDefInstance.IsDisposed Then
				m_InitializingDefInstance = True
				m_vb6FormDefInstance = New Form1()
				m_InitializingDefInstance = False
			End If
			DefInstance = m_vb6FormDefInstance
		End Get
		Set
			m_vb6FormDefInstance = Value
		End Set
	End Property
#End Region 
	
	
	Private Declare Sub Sleep Lib "Kernel32" (ByVal dwMilliseconds As Integer)
	
	'Private CfgFile As String
	Private LdPrjFile As String
	Private CreateFlwPrj As String
	Private CreateElecPrj As String
  Private ReportFile As String
  Private NeutralFiles As String
	
	Private m_Temp As Double
	Private m_Press As Double
	
  Private Lic As ScdLicenseApp
  Private Solver As ScdSolver
  Private WithEvents App As ScdApplication
  Private WithEvents Reports As ScdReports
  Private Prj As ScdProject
  Private MsgLog As ScdMessages
  'Private WithEvents MsgLog   As ScdMessages
  'Private WithEvents Prj      As ScdProject
  '
  '

  Private Sub SetAllNothing()
    'UPGRADE_NOTE: Object Solver may not be destroyed until it is garbage collected. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1029"'
    Solver = Nothing
    App = Nothing
    Reports = Nothing
    Prj = Nothing
    MsgLog = Nothing
    Lic = Nothing
  End Sub

  Private Sub CloseNoSave_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles CloseNoSave.Click
    App.CloseProject(False)
  End Sub

  Private Sub CloseSave_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles CloseSave.Click
    App.CloseProject(True)
  End Sub

  Private Function CmplxStrng(ByVal V As IScdComplex) As String
    Dim s As String
    s = Format(V.Magnitude, "F").PadLeft(7) & "<" & Format(V.Phase * 180 / 3.1459, "F").PadLeft(7) & ">"
    Return s
  End Function

  Private Sub DumpNodeElectrics(ByVal U As ScdNode)
    Dim TS As ScdTermStrip
    Dim T As ScdTerminal
    Dim W As ScdWire
    Dim C As ScdComponent

    On Error GoTo ErrorHandler

    List1.Items.Add("------------------------------------------------------")
    List1.Items.Add(U.Tag)
    List1.Items.Add("TermStrip Count:" & U.Electrics.TermStrips.Count)
    For Each TS In U.Electrics.TermStrips
      List1.Items.Add("  " & TS.Tag & " has " & TS.Count & " Terminals")
      For Each T In TS
        List1.Items.Add("    " & T.Tag)
      Next
    Next
    List1.Items.Add("  TermStrip(0):" & U.Electrics.TermStrips.TermStrip(0).Tag)
    List1.Items.Add("  TermStrip(PwrOut)" & U.Electrics.TermStrips.TermStrip("PwrOut").Tag)
    List1.Items.Add(" ")
    List1.Items.Add("Terminal Count:" & U.Electrics.Terminals.Count)
    For Each T In U.Electrics.Terminals
      List1.Items.Add("  " & T.TermStrip.Node.Tag & ":" & T.TermStrip.Tag & ":" & T.Tag)
    Next
    List1.Items.Add("  Terminal(0):" & U.Electrics.Terminals.Terminal(0).Tag)
    List1.Items.Add("  Terminal(N)" & U.Electrics.Terminals.Terminal("N").Tag)
    List1.Items.Add(" ")
    List1.Items.Add("Wires Count:" & U.Electrics.Wires.Count)
    For Each W In U.Electrics.Wires
      List1.Items.Add("  " & W.Tag)
    Next
    List1.Items.Add("  Wire(0):" & U.Electrics.Wires.Wire(0).Tag)
    List1.Items.Add(" ")
    List1.Items.Add("Components Count:" & U.Electrics.Components.Count)
    For Each C In U.Electrics.Components
      Dim s, s1, s2 As String
      s1 = C.SrcTerminal.Node.Tag & ":" & C.SrcTerminal.TermStrip.Tag & ":" & C.SrcTerminal.Tag
      s2 = C.DstTerminal.Node.Tag & ":" & C.DstTerminal.TermStrip.Tag & ":" & C.DstTerminal.Tag
      List1.Items.Add("  " & s1.PadRight(20) & " > " & s2.PadRight(20) & " V:" & CmplxStrng(C.Source) & " Z:" & CmplxStrng(C.Impedance) & " I:" & CmplxStrng(C.Current))
    Next
    List1.Items.Add("  Component(0):" & U.Electrics.Components.Component(0).SrcTerminal.Tag)
    List1.Items.Add("  Component(N)" & U.Electrics.Components.Component("N").SrcTerminal.Tag)

    List1.Items.Add("------------------------------------------------------")
    Exit Sub

ErrorHandler:
    List1.Items.Add("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    System.Diagnostics.Debug.WriteLine("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    Resume Next
  End Sub

  Private Sub CreateElec_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles CreateElec.Click

    ShiftLists()

    Const GoodTagSetPlace = 1

    Dim RetCode As Integer
    Dim I As Integer
    Dim Grfs As ScdGraphics
    Dim Trends As ScdTrends
    Dim Tags As ScdAppTags
    Dim Historian As ScdHistorian
    Dim Snapshot As ScdSnapshot
    Dim Dbg As ScdDebug
    Dim Test As ScdTest
    Dim Messages As ScdMessages
    Dim TagStr As String

    Dim STest As ScdTest
    Dim Test1 As ScdTest
    Dim Nodes As ScdNodes
    Dim Units As ScdNodes
    Dim Links As ScdNodes
    Dim Grf1 As ScdGraphic
    Dim Grf2 As ScdGraphic
    Dim GrfI As ScdGraphic
    Dim Dynamic As ScdDynamic
    Dim Trm As ScdTerminal
    Dim TStrip As ScdTermStrip
    Dim Wire As ScdWire
    Dim Unit As ScdNode
    Dim Link As ScdNode

    On Error GoTo ErrorHandler

    If App Is Nothing Then
      App = New ScdApplication
      ScdOpn.CheckState = System.Windows.Forms.CheckState.Checked
    End If

    If Prj Is Nothing Then
      Prj = App.CreateProject(RootPath.Text & CfgFileBox.Text, RootPath.Text & CreateElecPrj)
    End If

    Solver = Prj.Solver
    Solver.RunMode = eScdMode.eScdNet_Dynamic Or eScdMode.eScdHeat_Full

    MsgLog = Solver.Messages

    Nodes = Solver.Nodes
    Units = Solver.Nodes(eScdNodeTypes.eScdNodeType_Units)
    Links = Solver.Nodes(eScdNodeTypes.eScdNodeType_Links)

    Dim U1 As ScdNode
    Dim U2 As ScdNode
    Dim U3 As ScdNode
    Dim L1 As ScdNode
    Dim L2 As ScdNode

    U1 = Nodes.AddUnit("E_Incomer", "In")
    U2 = Nodes.AddUnit("E_Isolator", "Iso")
    U3 = Nodes.AddUnit("E_Socket", "Load")
    'U3 = Nodes.AddUnit("Pump-1", "Pmp")

    System.Diagnostics.Debug.WriteLine(U1.Tag)

    If (True) Then
      L1 = Nodes.AddCable("E_Cable", "C1", "In", "Out", "PwrOut", "Iso", "In", "PwrIn")
      L2 = Nodes.AddCable("E_Cable", "C2", "Iso", "Out", "PwrOut", "Load", "In", "PwrIn")
    Else
      L1 = Nodes.AddLink("E_Cable", "C1", "In", "Out", "Iso", "In")
      L2 = Nodes.AddLink("E_Cable", "C2", "Iso", "Out", "Load", "In")

      If (True) Then
        L1.Electrics.Wires.AutoWire("PwrOut", "PwrIn")
        L2.Electrics.Wires.AutoWire("PwrOut", "PwrIn")
      Else
        L1.Electrics.Wires.SetWire(0, "R", "PwrOut", "R", "PwrIn", "R")
        L1.Electrics.Wires.SetWire(1, "W", "PwrOut", "W", "PwrIn", "W")
        L1.Electrics.Wires.SetWire(2, "B", "PwrOut", "B", "PwrIn", "B")
        L1.Electrics.Wires.SetWire(3, "N", "PwrOut", "N", "PwrIn", "N")
        L2.Electrics.Wires.SetWire(0, "R", "PwrOut", "R", "PwrIn", "R")
        L2.Electrics.Wires.SetWire(1, "W", "PwrOut", "W", "PwrIn", "W")
        L2.Electrics.Wires.SetWire(2, "B", "PwrOut", "B", "PwrIn", "B")
        L2.Electrics.Wires.SetWire(3, "N", "PwrOut", "N", "PwrIn", "N")
      End If
    End If

    If (GoodTagSetPlace = 0) Then
      Prj.Tags.TagValue("Iso.On") = 1
      Prj.Tags.TagValue("Load.On") = 1
      Prj.Tags.TagValue("Load.Socket.LclLoad.ShaftPower (kW)") = 1.5
    End If

    For Each Unit In Solver.Nodes
      List1.Items.Add(Unit.Tag & " TermStrips.Count " & Unit.Electrics.TermStrips.Count)
    Next

    Grfs = Prj.Graphics
    Grf1 = Grfs.Add("ABC.scg")

    List1.Items.Add("Grf 1 Page = " & Grf1.Name)

    Grf1.AddUnit("In", 0, 60, 0, "", 1, 1, 0)
    Grf1.AddUnit("Iso", 0, 30, 0, "", 1, 1, 0)
    Grf1.AddUnit("Load", 0, 0, 0, "", 1, 1, 0)
    Grf1.Zoom()

    Dim Pts(5) As Double
    Pts(0) = 0
    Pts(1) = 60
    Pts(2) = 0
    Pts(3) = 0
    Pts(4) = 30
    Pts(5) = 0

    Grf1.AddLinkA("C1", Pts)
    Grf1.AddLinkM("C2", eScdLinkDrawMethods.eScdLinkDrawMethod_Direct)

    BuildFromTemplates(Grf1, Nodes)

    Grf1.Zoom(eScdZooms.eScdZoom_Full)

    RunDynamic(Solver)

    If (GoodTagSetPlace = 1) Then
      Prj.Tags.TagValue("Iso.On") = 1
      Prj.Tags.TagValue("Load.On") = 1
      Prj.Tags.TagValue("Load.Socket.LclLoad.ShaftPower (kW)") = 1.5
    End If

    DumpNodeElectrics(U1)
    DumpNodeElectrics(U2)
    DumpNodeElectrics(U3)
    DumpNodeElectrics(L1)
    DumpNodeElectrics(L2)

    SetAllNothing()

Done:
    Exit Sub

ErrorHandler:
    List1.Items.Add("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    System.Diagnostics.Debug.WriteLine("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    Resume Next
  End Sub


  Private Sub ReleaseAll_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles ReleaseAll.Click
    If Not App Is Nothing Then
      App.CloseProject(False) ' in case
    End If
    SetAllNothing()
  End Sub

  Private Function ValStr(ByVal X As Double) As String
    Dim l As Object
    Dim I As Integer
    ValStr = VB6.Format(X, "+0.0000E+00")
    '  ValStr = Format(x, "0.00000000000")
    '  l = InStr(ValStr, ".")
    '  ValStr = Left("               ", 10 - l) & ValStr
    '  l = Len(ValStr)
    '  ValStr = ValStr & Left("             ", 20 - i)
  End Function

  Private Sub DumpCnv(ByRef Cnv As ScdConversion, ByRef No1 As Boolean)
    Dim S As String
    If No1 Then
      S = "Cnv "
    Else
      S = "    "
    End If
    If Cnv.IsSI Then
      S = S & "SI "
    Else
      S = S & "   "
    End If
    S = S & Cnv.Family & VB.Left("               ", 15 - Len(Cnv.Family))
    S = S & " " & Cnv.Name & VB.Left("               ", 15 - Len(Cnv.Name))
    S = S & " " & Cnv.Formula & VB.Left("               ", 8 - Len(Cnv.Formula))
    S = S & " Scl:" & ValStr(Cnv.Scale)
    S = S & " Off:" & ValStr(Cnv.Offset)
    S = S & " " & Cnv.Description
    List1.Items.Add(S)
  End Sub

  Private Sub Conversions_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles Conversions.Click

    ShiftLists()

    Dim Solver As ScdSolver
    Dim Cnvs As ScdConversions
    Dim Cnv As ScdConversion
    Dim N As Object
    Dim Names() As String
    Dim I, Index As Object
    Dim IndexA() As Integer

    Solver = New ScdSolver

    Dim fso As New Scripting.FileSystemObject
    Dim CfgFileName As String
    CfgFileName = RootPath.Text & CfgFileBox.Text
    If Not fso.FileExists(CfgFileName) Then
      MsgBox("File not found (" & CfgFileName & ")")
      GoTo Done
    End If
    Solver.InitialiseCfg((CfgFileName))

    Cnvs = Solver.Conversions

    List1.Items.Add("ConvCount " & Cnvs.Count)
    Dim SCnv As ScdConversion
    For Each Cnv In Cnvs
      DumpCnv(Cnv, True)

      For Each SCnv In Cnv
        DumpCnv(SCnv, False)
      Next SCnv
    Next Cnv

    Cnv = Cnvs.Item(3).Item(0)
    DumpCnv(Cnv, True)
    Cnv = Cnvs.Item("T").Item("C")
    DumpCnv(Cnv, True)
Done:
  End Sub

  Private Sub DoSleep_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles DoSleep.Click
    System.Diagnostics.Debug.WriteLine("Start Sleep")
    'UPGRADE_WARNING: Screen property Screen.MousePointer has a new behavior. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup2065"'
    System.Windows.Forms.Cursor.Current = System.Windows.Forms.Cursors.WaitCursor
    Sleep((3000))
    'UPGRADE_WARNING: Screen property Screen.MousePointer has a new behavior. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup2065"'
    System.Windows.Forms.Cursor.Current = System.Windows.Forms.Cursors.Default
    System.Diagnostics.Debug.WriteLine("Done!")
  End Sub

  Private Sub Form1_Load(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles MyBase.Load
    RootPath.Text = "d:\Projects\_Tests\COMTests\"
    CfgFileBox.Text = "COMTests.cfg"
    CreateFlwPrj = "CF-01.spf\"
    CreateElecPrj = "EL-01.spf\"
    LdPrjFile = "3Feeders.spf\Project.spj"
    ReportFile = "TestAC.xls"
    NeutralFiles = "NeutralFiles\"

    m_sTemp.Text = "100"
    m_sPress.Text = "0"
    m_Temp = CDbl(m_sTemp.Text) + 273.16
    m_Press = CDbl(m_sPress.Text) + 101.325

    WideList_Click(WideList, New System.EventArgs)   ' One wide list
  End Sub

  'UPGRADE_WARNING: Event m_sTemp.TextChanged may fire when form is initialized. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup2075"'
  Private Sub m_sTemp_TextChanged(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles m_sTemp.TextChanged
    m_sTemp.Text = LTrim(m_sTemp.Text)
    If (Len(m_sTemp.Text) > 0) Then
      m_Temp = Val(m_sTemp.Text) + 273.16
    End If
    m_sTemp.Text = CStr(m_Temp - 273.16)
  End Sub

  'UPGRADE_WARNING: Event m_sPress.TextChanged may fire when form is initialized. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup2075"'
  Private Sub m_sPress_TextChanged(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles m_sPress.TextChanged
    m_sPress.Text = LTrim(m_sPress.Text)
    If (Len(m_sPress.Text) > 0) Then
      m_Press = Val(m_sPress.Text) + 101.325
    End If
    m_sPress.Text = CStr(m_Press - 101.325)
  End Sub

  Private Sub OpenCloseLic_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles OpenCloseLic.Click
    If Lic Is Nothing Then
      Lic = New ScdLicenseApp
      LicOpn.CheckState = System.Windows.Forms.CheckState.Checked
    Else
      'UPGRADE_NOTE: Object Lic may not be destroyed until it is garbage collected. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1029"'
      Lic = Nothing
      LicOpn.CheckState = System.Windows.Forms.CheckState.Unchecked
    End If
  End Sub

  Private Sub OpenCloseApp_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles OpenCloseApp.Click
    If App Is Nothing Then
      App = New ScdApplication
      ScdOpn.CheckState = System.Windows.Forms.CheckState.Checked
    Else
      'UPGRADE_NOTE: Object App may not be destroyed until it is garbage collected. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1029"'
      App = Nothing
      ScdOpn.CheckState = System.Windows.Forms.CheckState.Unchecked
    End If
  End Sub

  Private Sub ShiftLists()
    Dim I As Integer

    'List3.Clear
    'For I = 0 To List2.ListCount
    'List3.AddItem List2.List(I)
    'Next I

    List2.Items.Clear()
    For I = 0 To List1.Items.Count
      List2.Items.Add(VB6.GetItemString(List1, I))
    Next I

    List1.Items.Clear()

  End Sub

  'Private Sub App_OnProjectOpened(ByVal xReturn As Long)
  'List1.AddItem "App_OnProjectOpened" & xReturn
  'End Sub

  Private Sub ClearList_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles ClearList.Click
    List1.Items.Clear()
    List2.Items.Clear()
    'List3.Clear
  End Sub


  Private Sub QueryInfo_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles QueryInfo.Click

    ShiftLists()

    'UPGRADE_NOTE: RNd was upgraded to RNd_Renamed. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1061"'
    Dim Nd, RNd_Renamed As ScdNode
    Dim Cls As ScdNodeClass
    Dim Conn As ScdConnect
    Dim ConnPt As ScdConnectPoint
    Dim I As Integer
    Dim S As String

    On Error GoTo ErrorHandler

    Solver = Prj.Solver

    For Each Nd In Solver.Nodes
      Cls = Nd.Class
      List1.Items.Add(Nd.Tag & "  " & Nd.Type & " " & Nd.TypeString)
      List1.Items.Add("ClassName         " & Cls.Name)
      List1.Items.Add("Library/Category  " & Cls.Library & " " & Hex(Cls.Category))
      List1.Items.Add("ShortDesc         " & Cls.ShortDescription)
      List1.Items.Add("Description       " & Cls.Description)
      If Not Nd.SrcNode Is Nothing Then
        List1.Items.Add("SrcNode:" & Nd.SrcNode.Tag)
      End If
      If Not Nd.DstNode Is Nothing Then
        List1.Items.Add("DstNode:" & Nd.DstNode.Tag)
      End If
      List1.Items.Add("RmtNodes:")
      For Each RNd_Renamed In Nd.RmtNodes(eScdDirections.eScdDirection_All)
        If RNd_Renamed Is Nothing Then
          List1.Items.Add("  ?")
        Else
          List1.Items.Add("  " & RNd_Renamed.Tag)
        End If
      Next RNd_Renamed
      List1.Items.Add("ConnectPoints:")
      For Each ConnPt In Nd.ConnectPoints
        List1.Items.Add("  " & ConnPt.index & " " & ConnPt.Type & " " & ConnPt.TypeString & " " & ConnPt.Name & " (" & ConnPt.Description & ")")
      Next ConnPt
      List1.Items.Add("IOPaths:")
      For I = 0 To 2
        List1.Items.Add("  " & I & " " & Nd.ConnectPoints.InputPathName(eScdLinkTypes.eScdLinkType_All, I) & " -> " & Nd.ConnectPoints.OutputPathName(eScdLinkTypes.eScdLinkType_All, I))
      Next I
      List1.Items.Add("Connects:")
      For Each Conn In Nd.Connects
        List1.Items.Add("  Connect Tags   :" & Conn.index & " " & Conn.Node.Tag & " -> " & Conn.RmtNode.Tag)
        List1.Items.Add("          IDs    :" & Conn.ConnectPoint.ID & " -> " & Conn.RmtConnectPoint.ID)
        List1.Items.Add("          Strings:" & Conn.ConnectString & " -> " & Conn.RmtConnectString)
      Next Conn

    Next Nd
    Nd = Solver.Nodes.Node("F3")
    List1.Items.Add("ByTag " & Nd.Tag & "  " & Nd.Type & " " & Nd.TypeString)

    Dim Msg As IScdMessage
    'Set Msg = App.Messages '.Item(1)
    MsgLog.Refresh()
    For Each Msg In MsgLog
      If ((Msg.Flags And eScdMsg_Flags.eScdMsg_Error) <> 0) Then
        S = "Error"
      ElseIf ((Msg.Flags And eScdMsg_Flags.eScdMsg_Warning) <> 0) Then
        S = "Warn "
      Else
        S = "     "
      End If
      List1.Items.Add("Msg:" & S & Msg.Flags & " " & Msg.IDNo & " " & Msg.IterationNo & " " & Msg.SequenceNo & " " & Msg.CallNo & " " & Msg.Source & " " & Msg.Description)
    Next Msg
    MsgLog.Flush((10))
    Exit Sub

ErrorHandler:
    List1.Items.Add("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    System.Diagnostics.Debug.WriteLine("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    Resume Next
  End Sub


  Private Sub SpecieDefn_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles SpecieDefn.Click

    ShiftLists()

    Dim Solver As ScdSolver
    Dim SpDefns As ScdSpecieDefns
    Dim SpDefn As ScdSpecieDefn
    Dim N As Object
    Dim Names() As String
    Dim I, Index As Object
    Dim IndexA() As Integer

    Solver = New ScdSolver

    Dim fso As New Scripting.FileSystemObject
    Dim CfgFileName As String
    CfgFileName = RootPath.Text & CfgFileBox.Text
    If Not fso.FileExists(CfgFileName) Then
      MsgBox("File not found (" & CfgFileName & ")")
      GoTo Done
    End If
    Solver.InitialiseCfg((CfgFileName))

    List1.Items.Add("For Each ----------------------")

    SpDefns = Solver.SpecieDefns
    Dim Ps As ScdProperties
    Dim P As ScdProperty
    For Each SpDefn In SpDefns
      List1.Items.Add("Specie : " & SpDefn.Index & " " & SpDefn.Symbol & " " & SpDefn.Tag)
      Ps = SpDefn.Properties
      For Each P In Ps
        List1.Items.Add("  Prop[" & P.Index & "]: " & P.Tag & " " & P.Symbol & " " & P.Value(-1, m_Temp, m_Press) & " (" & P.Cnv.Name & ")")
      Next P
    Next SpDefn
Done:
  End Sub

  Function NonBlank(ByRef S As String) As String
    If Len(S) = 0 Then
      NonBlank = "-"
    Else
      NonBlank = S
    End If
  End Function

  Private Sub DisplaySpecieVector(ByRef ReqdModel As String)
    On Error Resume Next
    ShiftLists()
    Dim Solver As ScdSolver
    Dim Spcs As ScdSpecieDefns
    'Dim VecDefn As ScdSpVectorDefn
    'Dim SpMdl As ScdSpModel  'Standard Specie Properties Model
    'Dim ASpMdl As ScdSpModel  'DemoSM 'Demo Specie Properties Model
    Dim Props As IScdProperties
    Dim Prop As IScdProperty
    Dim Vec As IScdSpVector

    Dim fso As New Scripting.FileSystemObject
    Dim CfgFileName As String
    CfgFileName = RootPath.Text & CfgFileBox.Text
    If Not fso.FileExists(CfgFileName) Then
      MsgBox("File not found (" & CfgFileName & ")")
      GoTo Done
    End If
    Solver = New ScdSolver
    Solver.InitialiseCfg((CfgFileName))
    Vec = New ScdSpVector
    Props = Vec.Properties
    Vec.Model = ReqdModel
    Spcs = Vec.SpecieDefns

    Dim iH2O_l, iNaOH_l As Short
    Dim iSiO2_l, iCaO_s As Short
    iH2O_l = Spcs.Specie("H2O(l)").Index
    iNaOH_l = Spcs.Specie("NaOH(l)").Index
    iSiO2_l = Spcs.Specie("SiO2(l)").Index
    iCaO_s = Spcs.Specie("CaO(s)").Index

    Dim Mass() As Double

    Mass = VB6.CopyArray(Vec.Masses) 'get array of masses
    Mass(iH2O_l) = 1 'set water mass
    Vec.Masses = VB6.CopyArray(Mass) 'set array of masses

    List1.Items.Add("ReqdModel : " & ReqdModel)
    List1.Items.Add("Set Mass H20(l)=1")
    List1.Items.Add("Mass = " & Vec.Mass(-1)) 'Vec.Mass(-1) is same as Vec.Mass(Spcs.SomAll)

    Dim I, J As Short
    For J = 0 To 1
      If (J = 0) Then
        Vec.SpecieMass(iH2O_l) = 800
        Vec.SpecieMass(iNaOH_l) = 200
      Else
        Vec.SpecieMass(iH2O_l) = 100
        Vec.SpecieMass(iNaOH_l) = 10
        Vec.SpecieMass(iSiO2_l) = 15
        Vec.SpecieMass(iCaO_s) = 5
      End If
      List1.Items.Add("------ Example " & J + 1 & " ------")
      'List1.AddItem "Set Vector Mass"

      If (True) Then
        Mass = VB6.CopyArray(Vec.Masses) 'get array of masses
        For I = 0 To UBound(Mass)
          If Mass(I) > 0 Then List1.Items.Add("  " & Spcs.Specie(I).Tag & " = " & Mass(I))
        Next I
      Else
        For I = 0 To UBound(Mass)
          If Vec.SpecieMass(I) > 0 Then List1.Items.Add("  " & Spcs.Specie(I).Tag & " = " & Vec.SpecieMass(I))
        Next I
      End If

      Props = Vec.Properties
      List1.Items.Add("Model        : ------------------ " & Vec.Model)
      List1.Items.Add("Values at T(C),P(kPag)=" & m_Temp - 273.16 & "," & m_Press - 101.325)
      List1.Items.Add("Mass         = " & Vec.Mass(Spcs.SomAll))
      List1.Items.Add("Dens         = " & Vec.Density(-1, m_Temp, m_Press))
      List1.Items.Add("DensL        = " & Vec.Density(Spcs.SomLiq, m_Temp, m_Press))
      List1.Items.Add("DensS        = " & Vec.Density(Spcs.SomSol, m_Temp, m_Press))
      List1.Items.Add("Volume       = " & Vec.Volume(-1, m_Temp, m_Press))
      List1.Items.Add("Enthalpy     = " & Vec.msEnthalpy(, , m_Temp, m_Press))
      List1.Items.Add("Entropy      = " & Vec.msEntropy(, , m_Temp, m_Press))
      List1.Items.Add("Cp           = " & Vec.msCp(, , m_Temp, m_Press))
      List1.Items.Add("Conc(NaOH)   = " & Vec.Concentration(iNaOH_l, Spcs.SomLiq, m_Temp, m_Press))
      List1.Items.Add("SatP@T       = " & Vec.SaturationP(m_Temp))
      List1.Items.Add("SatT@P       = " & Vec.SaturationT(m_Press))
      List1.Items.Add("Mass(liq)    = " & Vec.Mass(Spcs.SomLiq))
      List1.Items.Add("Mass(sol)    = " & Vec.Mass(Spcs.SomSol))

      For Each Prop In Props
        List1.Items.Add(Prop.Index & "]: " & NonBlank((Prop.Tag)) & " " & NonBlank((Prop.Symbol)) & " " & Prop.Value(-1, m_Temp, m_Press) & " (" & Prop.Cnv.Name & ")")
      Next Prop
      Prop = Props.Property("DemoPropSet")
      If (Not Prop Is Nothing) Then
        Prop.SetValue(1)
        List1.Items.Add(Prop.Index & "]: " & NonBlank((Prop.Tag)) & " " & NonBlank((Prop.Symbol)) & " " & Prop.Value(-1, m_Temp, m_Press) & " (" & Prop.Cnv.Name & ")")
      End If
      Prop = Props.Property("DemoPropFix")
      If (Not Prop Is Nothing) Then
        Prop.SetValue(2)
        List1.Items.Add(Prop.Index & "]: " & NonBlank((Prop.Tag)) & " " & NonBlank((Prop.Symbol)) & " " & Prop.Value(-1, m_Temp, m_Press) & " (" & Prop.Cnv.Name & ")")
      End If
    Next J
Done:
    'UPGRADE_NOTE: Object Vec may not be destroyed until it is garbage collected. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1029"'
    Vec = Nothing
  End Sub

  Private Sub Standard_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles Standard.Click
    DisplaySpecieVector("Standard")
  End Sub
  Private Sub Demo_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles Demo.Click
    DisplaySpecieVector("DemoSpUsr*DemoSM")
  End Sub
  Private Sub ABayer_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles ABayer.Click
    DisplaySpecieVector("Bayer")
  End Sub

  Private Sub LoadTestFlwSheet_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles LoadTestFlwSheet.Click

    ShiftLists()

    Dim RetCode As Integer

    Dim Grfs As ScdGraphics
    Dim Trends As ScdTrends
    Dim Tags As ScdAppTags
    Dim Historian As ScdHistorian
    Dim Report As ScdReport
    Dim Snapshot As ScdSnapshot
    Dim Species As ScdSpecieDefns
    Dim Specie As ScdSpecieDefn
    Dim SpDef As ScdSpecieDefn
    Dim Dbg As ScdDebug
    Dim Test As ScdTest
    Dim Messages As ScdMessages

    Dim SpVec As IScdSpVector
    Dim SDbg As ScdDebug
    Dim STest, Test1 As ScdTest

    On Error GoTo ErrorHandler

    'Debug.Assert False

    App = New ScdApplication
    MsgLog = App.Messages
    List1.Items.Add("MsgLogCnt=" & MsgLog.Count)
    Prj = App.OpenProject(RootPath.Text & LdPrjFile)

    Dbg = App.Debug
    Grfs = Prj.Graphics

    Solver = Prj.Solver
    Dbg = Solver
    MsgLog.Refresh()
    List1.Items.Add("MsgLogCnt=" & MsgLog.Count)

    Dim TestErrs As Boolean
    TestErrs = False
    If (TestErrs) Then
      Test = Solver
      Test.ReturnHResult(3, 1001)
      Test.ThrowException(3, 1002)
      Test1 = New ScdTest
      Test1.ReturnHResult(3, 1001)
      Test1.ThrowException(3, 1002)
      'UPGRADE_NOTE: Object Test may not be destroyed until it is garbage collected. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1029"'
      Test = Nothing
    End If

    MsgLog.LogFileOpen(("XXX.csv"))
        MsgLog.Options = ScdSlv.eScdMsg_Options.eScdMsgOpt_COMCalls
    Dbg.FileOpen(("XXX.Dbg"))

    '-------------------------------------------
    Dim fso As New Scripting.FileSystemObject
    Dim TestInitCfg As Boolean
    TestInitCfg = False
    Dim CfgFileName As String
    If TestInitCfg Then
      CfgFileName = RootPath.Text & CfgFileBox.Text
      If Not fso.FileExists(CfgFileName) Then
        MsgBox("File not found (" & CfgFileName & ")")
        GoTo Done
      End If
      Solver.InitialiseCfg((CfgFileName))
    End If

    Species = Solver.SpecieDefns

    Dim Cnt, I As Object
    Dim J As Integer
    'UPGRADE_WARNING: Couldn't resolve default property of object Cnt. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
    Cnt = Species.Count
    System.Diagnostics.Debug.WriteLine("SpecieCnt=" & Cnt)
    'UPGRADE_WARNING: Couldn't resolve default property of object Cnt. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
    List1.Items.Add("SpecieCnt=" & Cnt)

    'Dim s As String
    For Each Specie In Species
      System.Diagnostics.Debug.WriteLine(VB6.TabLayout(Specie.Symbol, Specie.Tag))
      List1.Items.Add(Specie.Symbol & " " & Specie.Tag)
      J = 0
    Next Specie

    'Dim iH2O As Long
    'iH2O = Species("H2O(l)")

    'Debug.Print SpMdl.Mass(-1, 290, 100)
    Dim M As Object
    Dim Ma() As Double
    Dim Mx(20) As Double

    Tags = Prj.Tags

    'UPGRADE_WARNING: Couldn't resolve default property of object Tags.TagRecording(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
        Tags.TagRecording("P_3.NQv (L/s)") = eScdTagRecState.eScdTRS_HistRecording

        If (Tags.TagRecording("P_3.NQv (L/s)") And eScdTagRecState.eScdTRS_HistRecording) Then

        End If


        'GoTo Done

        List1.Items.Add("MessageLog:" & MsgLog.ViewLimit & ", " & MsgLog.Count)

        Dim Msg As IScdMessage
        Msg = MsgLog.Item(1)

        For Each Msg In MsgLog
            List1.Items.Add("Error:" & Msg.Flags & " " & Msg.IDNo & " " & Msg.IterationNo & " " & Msg.SequenceNo & " " & Msg.CallNo & " " & Msg.Source & " " & Msg.Description)
            If ((Msg.Flags And eScdMsg_Flags.eScdMsg_Error) <> 0) Then
                List1.Items.Add("--------Error")
            ElseIf ((Msg.Flags And eScdMsg_Flags.eScdMsg_Warning) <> 0) Then
                List1.Items.Add("--------Warning")
            End If
        Next Msg
Done:
        'Exit Sub
        'App.CloseProject

        'Set Solver = Nothing
        'Set App = Nothing
        'Set Species = Nothing
        'Set Tags = Nothing
        'Set Msg = Nothing
        '  SetAllNothing

        Exit Sub

ErrorHandler:
        List1.Items.Add("-->" & Hex(Err.Number) & " - " & Err.Source & " - " & Err.Description)
        System.Diagnostics.Debug.WriteLine("-->" & Hex(Err.Number) & " - " & Err.Source & " - " & Err.Description)

        Resume Next
  End Sub

  Private Sub CreateFlwSheet_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles CreateFlwSheet.Click

    ShiftLists()

    Dim RetCode, I As Integer
    Dim Grfs As ScdGraphics
    Dim Trends As ScdTrends
    Dim Tags As ScdAppTags
    Dim Historian As ScdHistorian
    Dim Snapshot As ScdSnapshot
    Dim Dbg As ScdDebug
    Dim Test As ScdTest
    Dim Messages As ScdMessages
    Dim TagStr As String

    Dim STest As ScdTest
    Dim Test1 As ScdTest
    Dim Nodes As ScdNodes
    'Dim Links As ScdNodes
    Dim Grf1 As ScdGraphic
    Dim Grf2 As ScdGraphic
    Dim GrfI As ScdGraphic
    Dim Dynamic As ScdDynamic

    On Error GoTo ErrorHandler

    If App Is Nothing Then
      App = New ScdApplication
      ScdOpn.CheckState = System.Windows.Forms.CheckState.Checked
    End If


    If Prj Is Nothing Then
      Prj = App.CreateProject(RootPath.Text & CfgFileBox.Text, RootPath.Text & CreateFlwPrj)
    End If

    Solver = Prj.Solver
    MsgLog = Solver.Messages

    Nodes = Solver.Nodes
    'Links = Solver.Links

    Nodes.AddUnit("Feed_XPG-1", "F1")
    List1.Items.Add("Good Unit:Add Ret Code=" & Nodes.ReturnCode)
    Nodes.AddUnit("Valve-1", "V2")
    List1.Items.Add("Good Unit:Add Ret Code=" & Nodes.ReturnCode)
    Nodes.AddUnit("Feed_XPG-1", "F3")
    List1.Items.Add("Good Unit:Add Ret Code=" & Nodes.ReturnCode)
    Nodes.AddUnit("Tank-1", "T1")
    List1.Items.Add("Good Unit:Add Ret Code=" & Nodes.ReturnCode)

    'Nodes.AddLink "Pipe-1", "YYYY", "F1", "O", "F2", "I"
    'List1.AddItem "Bad Link:Add Ret Code=" & Nodes.ReturnCode

    Nodes.AddLink("Pipe-1", "P1", "F1", "O", "V2", "I")
    List1.Items.Add("Good Link:Add Ret Code=" & Nodes.ReturnCode)
    Nodes.AddLink("Pipe-1", "P2", "V2", "O", "T1", "Base")
    List1.Items.Add("Good Link:Add Ret Code=" & Nodes.ReturnCode)
    Nodes.AddLink("Pipe-1", "P3", "T1", "Base", "F3", "I")
    List1.Items.Add("Good Link:Add Ret Code=" & Nodes.ReturnCode)

    Grfs = Prj.Graphics
    Grf1 = Grfs.Add("ABC.scg")
    Grf2 = Grfs.Add("DEF")

    'Set Grf1 = Prj.Graphics.Page(1)
    List1.Items.Add("Grf 1 Page = " & Grf1.Name)
    List1.Items.Add("Grf 2 Page = " & Grf2.Name)

    If False Then
      Grf1.AddUnit("F1", 0, 60, 0, "", 1, 1, 0)
      Grf2.AddUnit("F1", 0, 60, 0, "", 1, 1, 0)
      GoTo Done
    End If

    Dim P(5) As Double
    For Each GrfI In Grfs
      GrfI.AddUnit("F1", 0, 60, 0, "", 1, 1, 0)
      'GoTo Done1

      GrfI.AddUnit("V2", 30, 60, 0, "", 1, 1, 0)
      GrfI.AddUnit("T1", 60, 0, 0, "", 1, 1, 0)
      GrfI.AddUnit("F3", 90, 0, 0, "", 1, 1, 0)

      GrfI.Zoom()

      P(0) = 10
      P(1) = 60
      P(2) = 0
      P(3) = 25
      P(4) = 60
      P(5) = 0

      GrfI.AddLinkA("P1", P)
      GrfI.AddLinkM("P2", eScdLinkDrawMethods.eScdLinkDrawMethod_Direct)
      GrfI.AddLinkM("P3", eScdLinkDrawMethods.eScdLinkDrawMethod_Direct)

      Tags = Prj.Tags
      TagStr = "F1.T_Rqd"
      'UPGRADE_WARNING: Couldn't resolve default property of object Tags.TagValue(TagStr). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
      'UPGRADE_WARNING: Couldn't resolve default property of object Tags.TagType(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
      List1.Items.Add(TagStr & "=" & "[" & Tags.TagType(TagStr) & "] " & Tags.TagValue(TagStr))
      'UPGRADE_WARNING: Couldn't resolve default property of object Tags.TagValue(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
      Tags.TagValue(TagStr) = 300
      'UPGRADE_WARNING: Couldn't resolve default property of object Tags.TagValue(TagStr). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
      'UPGRADE_WARNING: Couldn't resolve default property of object Tags.TagType(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
      List1.Items.Add(TagStr & "=" & "[" & Tags.TagType(TagStr) & "] " & Tags.TagValue(TagStr))
Done1:
    Next GrfI ' GrfI

    'GoTo Done


    GrfI = Grfs.Page("abc.scg")
    GrfI = Grfs.Page("abc")
    GrfI = Grfs.Page("def")
    GrfI = Grfs.Page("def.scg")

    Dynamic = Solver.Dynamic

    Solver.RunMode = eScdMode.eScdNet_Dynamic Or eScdMode.eScdHeat_Full


    Tags.TagValue("F1.Mode") = "Source_ConstP"
    Tags.TagValue("F1.P_Rqd (kPag)") = 200
    'Tags.TagValue("F1.T_Rqd (C)") = -10 ' force an error message
    Tags.TagValue("V2.Posn.N/C") = 0
    Tags.TagValue("F3.Mode") = "Source_ConstP"

    'Prj.Save
    'List1.AddItem "Save Done"
    'Prj.Save , True
    'List1.AddItem "Save Done"

    RunDynamic(Solver)

    SetAllNothing()

Done:
    Exit Sub

ErrorHandler:
    List1.Items.Add("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    System.Diagnostics.Debug.WriteLine("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    Resume Next
  End Sub

  Private Sub Cmd_OnProjectLoaded(ByVal result As Integer)
    List1.Items.Add("Cmd_OnProjectLoaded" & result)
  End Sub


  Private Sub Reports_OnReportProcessed(ByVal RetCode As Integer) Handles Reports.OnReportProcessed
    List1.Items.Add("Report Processed" & RetCode)
  End Sub
  Sub xxx()
    List1.Items.Add("Keith's Test Start")

    Dim Solver As ScdSolver
    Dim SpDefns As ScdSpecieDefns 'collection of specie definitions
    Dim SpDefn As ScdSpecieDefn 'a specie definition
    Dim Specie As ScdSpecieDefn


    Const CfgFileName As String = "d:\syscad90\examples\ss_alumina\bayer.cfg"

    Solver = New ScdSolver

    ' Solver.Debug.FileOpen ("d:\syscad90\examples\ss_alumina\debug.txt")

    Solver.InitialiseCfg((CfgFileName))

    SpDefns = Solver.SpecieDefns
    Specie = SpDefns.Specie(0)

    Dim tg As String
    tg = Specie.Tag
    ' MsgBox tg
    List1.Items.Add("Spc(0) " & tg)
    List1.Items.Add("Keith's Test Start")

  End Sub

  Private Sub VecDefn_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles VecDefn.Click

    ShiftLists()

    Dim Solver As ScdSolver
    Dim Vec As IScdSpVector
    Dim Defn As IScdSpVectorDefn
    Dim I As Short
    Dim fso As New Scripting.FileSystemObject
    Dim CfgFileName As String
    CfgFileName = RootPath.Text & CfgFileBox.Text
    If Not fso.FileExists(CfgFileName) Then
      MsgBox("File not found (" & CfgFileName & ")")
      GoTo Done
    End If
    Solver = New ScdSolver
    Solver.InitialiseCfg((CfgFileName))
    Vec = New ScdSpVector
    Defn = Vec.Defn
    For I = 0 To Defn.Count - 1
      List1.Items.Add(("VecDefn: " & I & " " & Defn.Tag(I) & " " & Defn.Symbol(I) & " [" & Defn.Cnv(I).Name & "]"))
    Next I
Done:
  End Sub

  Private Sub VecOther_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles VecOther.Click
    'On Error Resume Next
    ShiftLists()

    xxx()

    Exit Sub

    Dim Solver As ScdSolver
    Dim Spcs As ScdSpecieDefns
    'Dim VecDefn As ScdSpVectorDefn
    'Dim SpMdl As ScdSpModel  'Standard Specie Properties Model
    'Dim ASpMdl As ScdSpModel  'DemoSM 'Demo Specie Properties Model
    Dim Props As IScdProperties
    Dim Prop As IScdProperty
    Dim Vec As IScdSpVector

    Dim fso As New Scripting.FileSystemObject
    Dim CfgFileName As String
    CfgFileName = RootPath.Text & CfgFileBox.Text
    If Not fso.FileExists(CfgFileName) Then
      MsgBox("File not found (" & CfgFileName & ")")
      GoTo Done
    End If
    Solver = New ScdSolver
    Solver.InitialiseCfg((CfgFileName))
    Spcs = Solver.SpecieDefns
    Vec = New ScdSpVector

    Props = Vec.Properties
    List1.Items.Add("Model     " & Vec.Model)
    List1.Items.Add("PropCount " & Props.Count)


    Vec.Model = "DemoSM"
    'Set Props = Vec.Properties
    List1.Items.Add("Model     " & Vec.Model)
    List1.Items.Add("PropCount " & Props.Count)
Done:

  End Sub

  Private Sub WideList_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles WideList.Click
    If VB6.PixelsToTwipsX(List1.Width) < 8000 Then
      List1.Width = VB6.TwipsToPixelsX(6015 * 2 + 120)
    Else
      List1.Width = VB6.TwipsToPixelsX(6015)
    End If
  End Sub

  Private Sub FlwshtExport(ByVal Blank As Boolean, ByVal DBFileName As String)

    ShiftLists()

    Const GoodTagSetPlace = 1

    Dim RetCode As Integer
    Dim I As Integer
    Dim Grfs As ScdGraphics
    Dim Trends As ScdTrends
    Dim Tags As ScdAppTags
    Dim Historian As ScdHistorian
    Dim Snapshot As ScdSnapshot
    Dim Dbg As ScdDebug
    Dim Test As ScdTest
    Dim Messages As ScdMessages
    Dim TagStr As String

    Dim STest As ScdTest
    Dim Test1 As ScdTest
    Dim Nodes As ScdNodes
    Dim Units As ScdNodes
    Dim Links As ScdNodes
    Dim Grf1 As ScdGraphic
    Dim Grf2 As ScdGraphic
    Dim GrfI As ScdGraphic
    Dim Dynamic As ScdDynamic
    Dim Trm As ScdTerminal
    Dim TStrip As ScdTermStrip
    Dim Wire As ScdWire
    Dim Unit As ScdNode
    Dim Link As ScdNode

    On Error GoTo ErrorHandler

    If App Is Nothing Then
      App = New ScdApplication
      ScdOpn.CheckState = System.Windows.Forms.CheckState.Checked
    End If

    If Prj Is Nothing Then
      Prj = App.CreateProject(RootPath.Text & CfgFileBox.Text, RootPath.Text & CreateElecPrj)
    End If

    Solver = Prj.Solver
    Solver.RunMode = ScdSlv.eScdMode.eScdNet_Dynamic Or ScdSlv.eScdMode.eScdHeat_Full

    MsgLog = Solver.Messages

    If (Not Blank) Then

      Grfs = Prj.Graphics
      Grf1 = Grfs.Add("ABC.scg")

      BuildFromTemplates(Grf1, Solver.Nodes)
      Grf1.Zoom(eScdZooms.eScdZoom_Full)
      Dynamic = Solver.Dynamic
      RunDynamic(Solver)

    End If


    List1.Items.Add("Export - Start")
    Dim ExFn As String
    ExFn = RootPath.Text & NeutralFiles & DBFileName
    If (Blank) Then
      Prj.ExportNeutralDB(eScdNDBOptions.eNDB_StdExport Or eScdNDBOptions.eNDB_BlankTables Or eScdNDBOptions.eNDB_GroupInserts, ExFn, ExFn)
    Else
      Prj.ExportNeutralDB(eScdNDBOptions.eNDB_StdExport, ExFn, ExFn)
    End If
    List1.Items.Add("Export - Done : " & ExFn)

    SetAllNothing()

Done:
    Exit Sub

ErrorHandler:
    List1.Items.Add("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    System.Diagnostics.Debug.WriteLine("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    Resume Next

  End Sub

  Private Sub FlwshtImport(ByVal GroupInserts As Boolean, ByVal DBFileName As String)

    ShiftLists()

    Const GoodTagSetPlace = 1

    Dim RetCode As Integer
    Dim I As Integer
    Dim Grfs As ScdGraphics
    Dim Trends As ScdTrends
    Dim Tags As ScdAppTags
    Dim Historian As ScdHistorian
    Dim Snapshot As ScdSnapshot
    Dim Dbg As ScdDebug
    Dim Test As ScdTest
    Dim Messages As ScdMessages

    Dim STest As ScdTest
    Dim Test1 As ScdTest
    Dim Dynamic As ScdDynamic

    On Error GoTo ErrorHandler

    If App Is Nothing Then
      App = New ScdApplication
      ScdOpn.CheckState = System.Windows.Forms.CheckState.Checked
    End If

    If Prj Is Nothing Then
      Prj = App.CreateProject(RootPath.Text & CfgFileBox.Text, RootPath.Text & CreateElecPrj)
    End If

    Solver = Prj.Solver
    Solver.RunMode = eScdMode.eScdNet_Dynamic Or eScdMode.eScdHeat_Full

    MsgLog = Solver.Messages


    Dynamic = Solver.Dynamic

    ' still need this
    Dim TemplateLibFolder As String
    Dim FileName As String
    ' Copy the required ECB's from the TemplateLib to the project folder
    ' NBNB FUTURES: This needs to be improved With the manner in which templates are stored.
    TemplateLibFolder = Prj.PrjFolder & "..\Templates\"
    FileName = Dir(TemplateLibFolder & "*.ecb", FileAttribute.Normal)    ' Retrieve the first entry.
    Do While FileName <> ""   ' Start the loop.
      FileCopy(TemplateLibFolder & FileName, Prj.PrjFolder & FileName)
      FileName = Dir()   ' Get next entry.
    Loop

    List1.Items.Add("Import - Start")
    Dim ExFn As String
    ExFn = RootPath.Text & NeutralFiles & DBFileName
    If GroupInserts Then
      Prj.ImportNeutralDB(eScdNDBOptions.eNDB_StdImport Or eScdNDBOptions.eNDB_GroupInserts, ExFn, ExFn, Nothing)
    Else
      Prj.ImportNeutralDB(eScdNDBOptions.eNDB_StdImport, ExFn, ExFn, Nothing)
    End If
    List1.Items.Add("Import - Done : " & ExFn)

    RunDynamic(Solver)

    SetAllNothing()

Done:
    Exit Sub

ErrorHandler:
    List1.Items.Add("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    System.Diagnostics.Debug.WriteLine("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    Resume Next

  End Sub

  Private Sub FlwsheetExport_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles FlwsheetExport.Click
    FlwshtExport(False, "NeutralTest.mdb")
  End Sub

  Private Sub FlwsheetImport_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles FlwsheetImport.Click
    FlwshtImport(False, "NeutralTest.mdb")
  End Sub

  Private Sub ExportBlank_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ExportBlank.Click
    FlwshtExport(True, "NeutralUser.Blank.mdb")
  End Sub

  Private Sub ImportUser_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ImportUser.Click
    FlwshtImport(True, "NeutralUser.mdb")
  End Sub

  Private Sub RunDynamic(ByVal Solver As ScdSolver)
    On Error GoTo ErrorHandler

    Dim Index, NoItems As Integer
    Dim NoIOs, IONo As Integer
    Dim eClass As eScdEvalSeqIO_Class
    Dim eFlags As eScdEvalSeqIO_Flags
    Dim RemoteIndex As Integer
    Dim RemoteDesc As String
    'UPGRADE_NOTE: Class was upgraded to Class_Renamed. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1061"'
    Dim Class_Renamed As String
    'UPGRADE_NOTE: Tag was upgraded to Tag_Renamed. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1061"'
    Dim Tag_Renamed As String
    Dim Dynamic As ScdDynamic
    Dim I As Integer

    Dynamic = Solver.Dynamic
    If True Then
      Dynamic.Idle()
      For I = 1 To 3
        Dynamic.Step(0, 0.1)
        'Sleep (1000)
        List1.Items.Add("Step")
      Next I
      Dynamic.Stop()

      Solver.CollectEvalSequenceInfo(eScdEvalSeq_Options.eScdEvalSeq_Process, NoItems)
      For Index = 1 To NoItems
        Solver.GetEvalSequenceItem(Index, Tag_Renamed, Class_Renamed, NoIOs)
        List1.Items.Add("ESeq  : " & Tag_Renamed & " - " & Class_Renamed)
        For IONo = 1 To NoIOs
          Solver.GetEvalSequenceIOItem(Index, IONo, eClass, eFlags, RemoteIndex, RemoteDesc)
          List1.Items.Add("      : " & Hex(eClass) & " - " & Hex(eFlags) & " - " & RemoteIndex & " - " & RemoteDesc)
        Next IONo
      Next Index
    End If
    Exit Sub

ErrorHandler:
    List1.Items.Add("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    System.Diagnostics.Debug.WriteLine("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    Resume Next

  End Sub

  Private Sub BuildFromTemplates(ByVal Grf1 As ScdGraphic, ByVal Nodes As ScdNodes)
    On Error GoTo ErrorHandler

    Dim TemplateLibFolder As String
    Dim FileName As String
    ' Copy the required ECB's from the TemplateLib to the project folder
    ' NBNB FUTURES: This needs to be improved With the manner in which templates are stored.
    TemplateLibFolder = Prj.PrjFolder & "..\Templates\"
    FileName = Dir(TemplateLibFolder & "*.ecb", FileAttribute.Normal)    ' Retrieve the first entry.
    Do While FileName <> ""   ' Start the loop.
      FileCopy(TemplateLibFolder & FileName, Prj.PrjFolder & FileName)
      FileName = Dir()   ' Get next entry.
    Loop

    Dim TagFixup As New ScdTagFixup
    TagFixup.Rule = eScdTagFixupRules.eFixup_Prefix

    TagFixup.String1 = "A"
    Grf1.InsertGroup(TemplateLibFolder & "TemplateLib1.mdb", "Elect:All", 100, 100, 0, 1, 1, 1, TagFixup)

    TagFixup.String1 = "B"
    Grf1.InsertGroup(TemplateLibFolder & "TemplateLib1.mdb", "Elect:Left", 120, 100, 0, 1, 1, 1, TagFixup)

    'A Cable to join the templates
    Nodes.AddCable("E_Cable", "X1", "A_Bb", "IO", "Ts1", "B_Bb", "IO", "Ts0")
    Dim Pts1(5) As Double
    Pts1(0) = 100
    Pts1(1) = 100 - 5
    Pts1(2) = 0
    Pts1(3) = 120
    Pts1(4) = 100 - 5
    Pts1(5) = 0
    Grf1.AddLinkA("X1", Pts1)
    Exit Sub

ErrorHandler:
    List1.Items.Add("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    System.Diagnostics.Debug.WriteLine("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    Resume Next

  End Sub

  Private Sub LdAndRunMineServe_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles LdAndRunMineServe.Click

    ShiftLists()

    Const GoodTagSetPlace = 1

    Dim RetCode As Integer
    Dim I As Integer
    Dim Grfs As ScdGraphics
    Dim Trends As ScdTrends
    Dim Tags As ScdAppTags
    Dim Historian As ScdHistorian
    Dim Snapshot As ScdSnapshot
    Dim Dbg As ScdDebug
    Dim Test As ScdTest
    Dim Messages As ScdMessages

    Dim STest As ScdTest
    Dim Test1 As ScdTest
    Dim Dynamic As ScdDynamic

    On Error GoTo ErrorHandler

    If App Is Nothing Then
      App = New ScdApplication
      ScdOpn.CheckState = System.Windows.Forms.CheckState.Checked
    End If


    If Prj Is Nothing Then
      Prj = App.CreateProject("D:\Projects\M\MineServe2\CfgFiles", "D:\Projects\M\MineServe2\Water Valve testWS(Correct)-03.spf")
    End If

    Solver = Prj.Solver
    Solver.RunMode = eScdMode.eScdNet_Dynamic Or eScdMode.eScdHeat_Full

    MsgLog = Solver.Messages


    Dynamic = Solver.Dynamic

    '' still need this
    'Dim TemplateLibFolder As String
    'Dim FileName As String
    '' Copy the required ECB's from the TemplateLib to the project folder
    '' NBNB FUTURES: This needs to be improved With the manner in which templates are stored.
    'TemplateLibFolder = Prj.PrjFolder & "..\Templates\"
    'FileName = Dir(TemplateLibFolder & "*.ecb", FileAttribute.Normal)    ' Retrieve the first entry.
    'Do While FileName <> ""   ' Start the loop.
    '  FileCopy(TemplateLibFolder & FileName, Prj.PrjFolder & FileName)
    '  FileName = Dir()   ' Get next entry.
    'Loop

    'List1.Items.Add("Import - Start")
    'Dim ExFn As String
    'ExFn = RootPath.Text & NeutralFiles & DBFileName
    'If GroupInserts Then
    '  Prj.ImportNeutralDB(eScdNDBOptions.eNDB_StdImport Or eScdNDBOptions.eNDB_GroupInserts, ExFn, ExFn, Nothing)
    'Else
    '  Prj.ImportNeutralDB(eScdNDBOptions.eNDB_StdImport, ExFn, ExFn, Nothing)
    'End If
    'List1.Items.Add("Import - Done : " & ExFn)

    RunDynamic(Solver)

    SetAllNothing()

Done:
    Exit Sub

ErrorHandler:
    List1.Items.Add("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    System.Diagnostics.Debug.WriteLine("-->" & Hex(Err.Number) & "[" & (Err.Number And 255) & "] - " & Err.Source & " - " & Err.Description)
    Resume Next


  End Sub
End Class