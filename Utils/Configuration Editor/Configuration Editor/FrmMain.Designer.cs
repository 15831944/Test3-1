namespace Configuration_Editor
{
    partial class FrmMain
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.ssMain = new System.Windows.Forms.StatusStrip();
            this.menuMain = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.closeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
            this.menuOpenDatabase = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.MenuSort = new System.Windows.Forms.ToolStripMenuItem();
            this.menuAdvancedSort = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutSysCADConfigurationEditorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tcMain = new System.Windows.Forms.TabControl();
            this.tabSpecies = new System.Windows.Forms.TabPage();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.grpAvailableSpecies = new System.Windows.Forms.GroupBox();
            this.lstDBSpecies = new System.Windows.Forms.ListView();
            this.chSymbol = new System.Windows.Forms.ColumnHeader();
            this.chName = new System.Windows.Forms.ColumnHeader();
            this.menuSpDBContext = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.menuSortSpDBAlph = new System.Windows.Forms.ToolStripMenuItem();
            this.menuSortSpDBPhase = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripSeparator();
            this.menuSpDBAddToProject = new System.Windows.Forms.ToolStripMenuItem();
            this.pnlFilter = new System.Windows.Forms.Panel();
            this.txtFilter = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tcSpecies = new System.Windows.Forms.TabControl();
            this.tabProjectSpecies = new System.Windows.Forms.TabPage();
            this.grpProjectList = new System.Windows.Forms.GroupBox();
            this.pnlActiveList = new System.Windows.Forms.Panel();
            this.lstProjectVector = new System.Windows.Forms.ListView();
            this.chProjSymbol = new System.Windows.Forms.ColumnHeader();
            this.chProjMinTemp = new System.Windows.Forms.ColumnHeader();
            this.chProjMaxTemp = new System.Windows.Forms.ColumnHeader();
            this.pnlListButtons = new System.Windows.Forms.Panel();
            this.btnAdd = new System.Windows.Forms.Button();
            this.btnRemove = new System.Windows.Forms.Button();
            this.btnMoveUp = new System.Windows.Forms.Button();
            this.btnMoveDown = new System.Windows.Forms.Button();
            this.pnlActive = new System.Windows.Forms.Panel();
            this.tcIDE = new System.Windows.Forms.TabControl();
            this.tabIDESpecie = new System.Windows.Forms.TabPage();
            this.grpSpecie = new System.Windows.Forms.GroupBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.numLowerTempTolerance = new System.Windows.Forms.NumericUpDown();
            this.label6 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.txtDefinedMinTemp = new System.Windows.Forms.TextBox();
            this.txtDefinedMaxTemp = new System.Windows.Forms.TextBox();
            this.txtAllowedMaxTemp = new System.Windows.Forms.TextBox();
            this.txtAllowedMinTemp = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.numGreaterTempTolerance = new System.Windows.Forms.NumericUpDown();
            this.chkIdeal = new System.Windows.Forms.CheckBox();
            this.tabIDECalculation = new System.Windows.Forms.TabPage();
            this.grpCalculation = new System.Windows.Forms.GroupBox();
            this.txtCalcSymbol = new System.Windows.Forms.TextBox();
            this.label26 = new System.Windows.Forms.Label();
            this.label24 = new System.Windows.Forms.Label();
            this.txtCalcName = new System.Windows.Forms.TextBox();
            this.label23 = new System.Windows.Forms.Label();
            this.txtCalculation = new Be.Windows.Forms.RichTextBoxEx();
            this.tabIDELabel = new System.Windows.Forms.TabPage();
            this.grpTextValue = new System.Windows.Forms.GroupBox();
            this.txtText = new System.Windows.Forms.TextBox();
            this.tabIDEBlank = new System.Windows.Forms.TabPage();
            this.pnlBlank = new System.Windows.Forms.Panel();
            this.tabIDEAttribute = new System.Windows.Forms.TabPage();
            this.grpAttribute = new System.Windows.Forms.GroupBox();
            this.comboAttributeType = new System.Windows.Forms.ComboBox();
            this.label8 = new System.Windows.Forms.Label();
            this.txtAttributeName = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.tabSpecieDatabase = new System.Windows.Forms.TabPage();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.graph1 = new Configuration_Editor.Graph();
            this.tlpEquations = new System.Windows.Forms.TableLayoutPanel();
            this.txtMaxTemp3 = new System.Windows.Forms.TextBox();
            this.txtMinTemp3 = new System.Windows.Forms.TextBox();
            this.txtMaxTemp2 = new System.Windows.Forms.TextBox();
            this.txtMinTemp2 = new System.Windows.Forms.TextBox();
            this.txtMaxTemp1 = new System.Windows.Forms.TextBox();
            this.txtMinTemp1 = new System.Windows.Forms.TextBox();
            this.txtMaxTemp0 = new System.Windows.Forms.TextBox();
            this.txtMinTemp0 = new System.Windows.Forms.TextBox();
            this.label38 = new System.Windows.Forms.Label();
            this.label39 = new System.Windows.Forms.Label();
            this.label40 = new System.Windows.Forms.Label();
            this.label45 = new System.Windows.Forms.Label();
            this.label41 = new System.Windows.Forms.Label();
            this.label42 = new System.Windows.Forms.Label();
            this.txtFormula0 = new System.Windows.Forms.TextBox();
            this.txtFormula1 = new System.Windows.Forms.TextBox();
            this.txtFormula2 = new System.Windows.Forms.TextBox();
            this.txtFormula3 = new System.Windows.Forms.TextBox();
            this.pnlTempDependantRadios = new System.Windows.Forms.Panel();
            this.label37 = new System.Windows.Forms.Label();
            this.radioDensity = new System.Windows.Forms.RadioButton();
            this.radioEntropy = new System.Windows.Forms.RadioButton();
            this.radioHeatOfFormation = new System.Windows.Forms.RadioButton();
            this.radioHeatCapacity = new System.Windows.Forms.RadioButton();
            this.radioVapourPressure = new System.Windows.Forms.RadioButton();
            this.groupBox8 = new System.Windows.Forms.GroupBox();
            this.txtSolvent = new System.Windows.Forms.TextBox();
            this.label36 = new System.Windows.Forms.Label();
            this.txtAccentricity = new System.Windows.Forms.TextBox();
            this.txtCritTemp = new System.Windows.Forms.TextBox();
            this.label47 = new System.Windows.Forms.Label();
            this.txtCritVol = new System.Windows.Forms.TextBox();
            this.label46 = new System.Windows.Forms.Label();
            this.txtCritPressure = new System.Windows.Forms.TextBox();
            this.label43 = new System.Windows.Forms.Label();
            this.label44 = new System.Windows.Forms.Label();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.txtCheckedBy = new System.Windows.Forms.TextBox();
            this.label35 = new System.Windows.Forms.Label();
            this.label34 = new System.Windows.Forms.Label();
            this.txtSource = new System.Windows.Forms.TextBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.comboOccurence = new System.Windows.Forms.ComboBox();
            this.label33 = new System.Windows.Forms.Label();
            this.txtMaxTemperature = new System.Windows.Forms.TextBox();
            this.txtMinTemperature = new System.Windows.Forms.TextBox();
            this.txtElementalComposition = new System.Windows.Forms.TextBox();
            this.label29 = new System.Windows.Forms.Label();
            this.label27 = new System.Windows.Forms.Label();
            this.label28 = new System.Windows.Forms.Label();
            this.grpNomenclature = new System.Windows.Forms.GroupBox();
            this.label25 = new System.Windows.Forms.Label();
            this.txtName = new System.Windows.Forms.TextBox();
            this.label30 = new System.Windows.Forms.Label();
            this.txtSymbol = new System.Windows.Forms.TextBox();
            this.label31 = new System.Windows.Forms.Label();
            this.txtPhase = new System.Windows.Forms.TextBox();
            this.label32 = new System.Windows.Forms.Label();
            this.tabMisc = new System.Windows.Forms.TabPage();
            this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
            this.grpDefaults = new System.Windows.Forms.GroupBox();
            this.chkH2OAqueous = new System.Windows.Forms.CheckBox();
            this.comboSpecieModel = new System.Windows.Forms.ComboBox();
            this.label22 = new System.Windows.Forms.Label();
            this.grpThermo = new System.Windows.Forms.GroupBox();
            this.tableLayoutPanel4 = new System.Windows.Forms.TableLayoutPanel();
            this.txtMinP = new System.Windows.Forms.TextBox();
            this.txtNP = new System.Windows.Forms.TextBox();
            this.txtStP = new System.Windows.Forms.TextBox();
            this.txtMaxT = new System.Windows.Forms.TextBox();
            this.txtMinT = new System.Windows.Forms.TextBox();
            this.txtNT = new System.Windows.Forms.TextBox();
            this.label21 = new System.Windows.Forms.Label();
            this.label20 = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.label18 = new System.Windows.Forms.Label();
            this.label19 = new System.Windows.Forms.Label();
            this.txtStT = new System.Windows.Forms.TextBox();
            this.txtMaxP = new System.Windows.Forms.TextBox();
            this.grpCalculationSelection = new System.Windows.Forms.GroupBox();
            this.tableLayoutPanel3 = new System.Windows.Forms.TableLayoutPanel();
            this.comboDynamicHeat = new System.Windows.Forms.ComboBox();
            this.comboProBalHeat = new System.Windows.Forms.ComboBox();
            this.comboMaxCompFlow = new System.Windows.Forms.ComboBox();
            this.comboProbalFlow = new System.Windows.Forms.ComboBox();
            this.comboMaxCompSurge = new System.Windows.Forms.ComboBox();
            this.comboDynamicSurge = new System.Windows.Forms.ComboBox();
            this.comboProBalSurge = new System.Windows.Forms.ComboBox();
            this.label14 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.comboDynamicFlow = new System.Windows.Forms.ComboBox();
            this.comboMaxCompHeat = new System.Windows.Forms.ComboBox();
            this.grpSolution = new System.Windows.Forms.GroupBox();
            this.tableLayoutPanel5 = new System.Windows.Forms.TableLayoutPanel();
            this.chkDynamicAllowed = new System.Windows.Forms.CheckBox();
            this.label9 = new System.Windows.Forms.Label();
            this.chkProbalAllowed = new System.Windows.Forms.CheckBox();
            this.comboDefaultSolution = new System.Windows.Forms.ComboBox();
            this.grpDescription = new System.Windows.Forms.GroupBox();
            this.txtDescription = new System.Windows.Forms.TextBox();
            this.tabModels = new System.Windows.Forms.TabPage();
            this.scModels = new System.Windows.Forms.SplitContainer();
            this.grpModels = new System.Windows.Forms.GroupBox();
            this.treeModels = new System.Windows.Forms.TreeView();
            this.pnlModelOptions = new System.Windows.Forms.Panel();
            this.radioSortByDLL = new System.Windows.Forms.RadioButton();
            this.radioSortByGroup = new System.Windows.Forms.RadioButton();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.treeReqSpecies = new System.Windows.Forms.TreeView();
            this.pnlReqSpecieOptions = new System.Windows.Forms.Panel();
            this.radioSpecieSortBySpecie = new System.Windows.Forms.RadioButton();
            this.radioSpecieSortByDLL = new System.Windows.Forms.RadioButton();
            this.dlgOpenDB = new System.Windows.Forms.OpenFileDialog();
            this.dlgOpenConfig = new System.Windows.Forms.OpenFileDialog();
            this.dlgSaveConfig = new System.Windows.Forms.SaveFileDialog();
            this.menuMain.SuspendLayout();
            this.tcMain.SuspendLayout();
            this.tabSpecies.SuspendLayout();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.grpAvailableSpecies.SuspendLayout();
            this.menuSpDBContext.SuspendLayout();
            this.pnlFilter.SuspendLayout();
            this.tcSpecies.SuspendLayout();
            this.tabProjectSpecies.SuspendLayout();
            this.grpProjectList.SuspendLayout();
            this.pnlActiveList.SuspendLayout();
            this.pnlListButtons.SuspendLayout();
            this.pnlActive.SuspendLayout();
            this.tcIDE.SuspendLayout();
            this.tabIDESpecie.SuspendLayout();
            this.grpSpecie.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numLowerTempTolerance)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numGreaterTempTolerance)).BeginInit();
            this.tabIDECalculation.SuspendLayout();
            this.grpCalculation.SuspendLayout();
            this.tabIDELabel.SuspendLayout();
            this.grpTextValue.SuspendLayout();
            this.tabIDEBlank.SuspendLayout();
            this.tabIDEAttribute.SuspendLayout();
            this.grpAttribute.SuspendLayout();
            this.tabSpecieDatabase.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.tlpEquations.SuspendLayout();
            this.pnlTempDependantRadios.SuspendLayout();
            this.groupBox8.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.grpNomenclature.SuspendLayout();
            this.tabMisc.SuspendLayout();
            this.tableLayoutPanel2.SuspendLayout();
            this.grpDefaults.SuspendLayout();
            this.grpThermo.SuspendLayout();
            this.tableLayoutPanel4.SuspendLayout();
            this.grpCalculationSelection.SuspendLayout();
            this.tableLayoutPanel3.SuspendLayout();
            this.grpSolution.SuspendLayout();
            this.tableLayoutPanel5.SuspendLayout();
            this.grpDescription.SuspendLayout();
            this.tabModels.SuspendLayout();
            this.scModels.Panel1.SuspendLayout();
            this.scModels.Panel2.SuspendLayout();
            this.scModels.SuspendLayout();
            this.grpModels.SuspendLayout();
            this.pnlModelOptions.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.pnlReqSpecieOptions.SuspendLayout();
            this.SuspendLayout();
            // 
            // ssMain
            // 
            this.ssMain.Location = new System.Drawing.Point(0, 594);
            this.ssMain.Name = "ssMain";
            this.ssMain.Size = new System.Drawing.Size(952, 22);
            this.ssMain.TabIndex = 0;
            this.ssMain.Text = "statusStrip1";
            // 
            // menuMain
            // 
            this.menuMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.editToolStripMenuItem,
            this.helpToolStripMenuItem});
            this.menuMain.Location = new System.Drawing.Point(0, 0);
            this.menuMain.Name = "menuMain";
            this.menuMain.Size = new System.Drawing.Size(952, 24);
            this.menuMain.TabIndex = 1;
            this.menuMain.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.openToolStripMenuItem,
            this.closeToolStripMenuItem,
            this.toolStripMenuItem1,
            this.saveToolStripMenuItem,
            this.saveAsToolStripMenuItem,
            this.toolStripMenuItem2,
            this.menuOpenDatabase,
            this.toolStripMenuItem3,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "&File";
            // 
            // newToolStripMenuItem
            // 
            this.newToolStripMenuItem.Name = "newToolStripMenuItem";
            this.newToolStripMenuItem.Size = new System.Drawing.Size(179, 22);
            this.newToolStripMenuItem.Text = "&New";
            this.newToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(179, 22);
            this.openToolStripMenuItem.Text = "&Open Configuration";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.menuOpen_Click);
            // 
            // closeToolStripMenuItem
            // 
            this.closeToolStripMenuItem.Name = "closeToolStripMenuItem";
            this.closeToolStripMenuItem.Size = new System.Drawing.Size(179, 22);
            this.closeToolStripMenuItem.Text = "&Close";
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(176, 6);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(179, 22);
            this.saveToolStripMenuItem.Text = "&Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // saveAsToolStripMenuItem
            // 
            this.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
            this.saveAsToolStripMenuItem.Size = new System.Drawing.Size(179, 22);
            this.saveAsToolStripMenuItem.Text = "Save &As";
            this.saveAsToolStripMenuItem.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(176, 6);
            // 
            // menuOpenDatabase
            // 
            this.menuOpenDatabase.Name = "menuOpenDatabase";
            this.menuOpenDatabase.Size = new System.Drawing.Size(179, 22);
            this.menuOpenDatabase.Text = "Open &Database";
            this.menuOpenDatabase.Click += new System.EventHandler(this.menuOpenDatabase_Click);
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            this.toolStripMenuItem3.Size = new System.Drawing.Size(176, 6);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(179, 22);
            this.exitToolStripMenuItem.Text = "E&xit";
            // 
            // editToolStripMenuItem
            // 
            this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.MenuSort,
            this.menuAdvancedSort});
            this.editToolStripMenuItem.Name = "editToolStripMenuItem";
            this.editToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.editToolStripMenuItem.Text = "&Edit";
            // 
            // MenuSort
            // 
            this.MenuSort.Name = "MenuSort";
            this.MenuSort.Size = new System.Drawing.Size(168, 22);
            this.MenuSort.Text = "Quick &Sort";
            this.MenuSort.Click += new System.EventHandler(this.MenuSort_Click);
            // 
            // menuAdvancedSort
            // 
            this.menuAdvancedSort.Name = "menuAdvancedSort";
            this.menuAdvancedSort.Size = new System.Drawing.Size(168, 22);
            this.menuAdvancedSort.Text = "&Advanced Sort...";
            this.menuAdvancedSort.Click += new System.EventHandler(this.menuAdvancedSort_Click);
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.aboutSysCADConfigurationEditorToolStripMenuItem});
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(40, 20);
            this.helpToolStripMenuItem.Text = "&Help";
            // 
            // aboutSysCADConfigurationEditorToolStripMenuItem
            // 
            this.aboutSysCADConfigurationEditorToolStripMenuItem.Name = "aboutSysCADConfigurationEditorToolStripMenuItem";
            this.aboutSysCADConfigurationEditorToolStripMenuItem.Size = new System.Drawing.Size(254, 22);
            this.aboutSysCADConfigurationEditorToolStripMenuItem.Text = "&About SysCAD Configuration Editor";
            this.aboutSysCADConfigurationEditorToolStripMenuItem.Click += new System.EventHandler(this.aboutSysCADConfigurationEditorToolStripMenuItem_Click);
            // 
            // tcMain
            // 
            this.tcMain.Controls.Add(this.tabSpecies);
            this.tcMain.Controls.Add(this.tabMisc);
            this.tcMain.Controls.Add(this.tabModels);
            this.tcMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tcMain.Location = new System.Drawing.Point(0, 24);
            this.tcMain.Name = "tcMain";
            this.tcMain.SelectedIndex = 0;
            this.tcMain.Size = new System.Drawing.Size(952, 570);
            this.tcMain.TabIndex = 2;
            // 
            // tabSpecies
            // 
            this.tabSpecies.Controls.Add(this.splitContainer1);
            this.tabSpecies.Location = new System.Drawing.Point(4, 22);
            this.tabSpecies.Name = "tabSpecies";
            this.tabSpecies.Padding = new System.Windows.Forms.Padding(3);
            this.tabSpecies.Size = new System.Drawing.Size(944, 544);
            this.tabSpecies.TabIndex = 0;
            this.tabSpecies.Text = "Species";
            this.tabSpecies.UseVisualStyleBackColor = true;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(3, 3);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.grpAvailableSpecies);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.tcSpecies);
            this.splitContainer1.Size = new System.Drawing.Size(938, 538);
            this.splitContainer1.SplitterDistance = 225;
            this.splitContainer1.TabIndex = 1;
            // 
            // grpAvailableSpecies
            // 
            this.grpAvailableSpecies.Controls.Add(this.lstDBSpecies);
            this.grpAvailableSpecies.Controls.Add(this.pnlFilter);
            this.grpAvailableSpecies.Dock = System.Windows.Forms.DockStyle.Fill;
            this.grpAvailableSpecies.Location = new System.Drawing.Point(0, 0);
            this.grpAvailableSpecies.Name = "grpAvailableSpecies";
            this.grpAvailableSpecies.Size = new System.Drawing.Size(225, 538);
            this.grpAvailableSpecies.TabIndex = 0;
            this.grpAvailableSpecies.TabStop = false;
            this.grpAvailableSpecies.Text = "Available Species";
            // 
            // lstDBSpecies
            // 
            this.lstDBSpecies.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chSymbol,
            this.chName});
            this.lstDBSpecies.ContextMenuStrip = this.menuSpDBContext;
            this.lstDBSpecies.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lstDBSpecies.HideSelection = false;
            this.lstDBSpecies.Location = new System.Drawing.Point(3, 16);
            this.lstDBSpecies.Name = "lstDBSpecies";
            this.lstDBSpecies.Size = new System.Drawing.Size(219, 492);
            this.lstDBSpecies.TabIndex = 1;
            this.lstDBSpecies.UseCompatibleStateImageBehavior = false;
            this.lstDBSpecies.View = System.Windows.Forms.View.Details;
            this.lstDBSpecies.ItemActivate += new System.EventHandler(this.lstDBSpecies_ItemActivate);
            this.lstDBSpecies.SelectedIndexChanged += new System.EventHandler(this.lstDBSpecies_SelectedIndexChanged);
            this.lstDBSpecies.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.lstDBSpecies_ItemDrag);
            // 
            // chSymbol
            // 
            this.chSymbol.Text = "Symbol";
            this.chSymbol.Width = 105;
            // 
            // chName
            // 
            this.chName.Text = "Name";
            // 
            // menuSpDBContext
            // 
            this.menuSpDBContext.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuSortSpDBAlph,
            this.menuSortSpDBPhase,
            this.toolStripMenuItem4,
            this.menuSpDBAddToProject});
            this.menuSpDBContext.Name = "menuSpDBContext";
            this.menuSpDBContext.Size = new System.Drawing.Size(191, 76);
            // 
            // menuSortSpDBAlph
            // 
            this.menuSortSpDBAlph.Checked = true;
            this.menuSortSpDBAlph.CheckOnClick = true;
            this.menuSortSpDBAlph.CheckState = System.Windows.Forms.CheckState.Checked;
            this.menuSortSpDBAlph.Name = "menuSortSpDBAlph";
            this.menuSortSpDBAlph.Size = new System.Drawing.Size(190, 22);
            this.menuSortSpDBAlph.Text = "Sort Alphabetically";
            this.menuSortSpDBAlph.CheckedChanged += new System.EventHandler(this.menuSortSpDBAlph_CheckedChanged);
            // 
            // menuSortSpDBPhase
            // 
            this.menuSortSpDBPhase.Checked = true;
            this.menuSortSpDBPhase.CheckOnClick = true;
            this.menuSortSpDBPhase.CheckState = System.Windows.Forms.CheckState.Checked;
            this.menuSortSpDBPhase.Name = "menuSortSpDBPhase";
            this.menuSortSpDBPhase.Size = new System.Drawing.Size(190, 22);
            this.menuSortSpDBPhase.Text = "Sort By Phase";
            this.menuSortSpDBPhase.CheckedChanged += new System.EventHandler(this.menuSortSpDBAlph_CheckedChanged);
            // 
            // toolStripMenuItem4
            // 
            this.toolStripMenuItem4.Name = "toolStripMenuItem4";
            this.toolStripMenuItem4.Size = new System.Drawing.Size(187, 6);
            // 
            // menuSpDBAddToProject
            // 
            this.menuSpDBAddToProject.Name = "menuSpDBAddToProject";
            this.menuSpDBAddToProject.Size = new System.Drawing.Size(190, 22);
            this.menuSpDBAddToProject.Text = "&Add To Project Vector";
            this.menuSpDBAddToProject.Click += new System.EventHandler(this.menuSpDBAddToProject_Click);
            // 
            // pnlFilter
            // 
            this.pnlFilter.Controls.Add(this.txtFilter);
            this.pnlFilter.Controls.Add(this.label1);
            this.pnlFilter.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnlFilter.Location = new System.Drawing.Point(3, 508);
            this.pnlFilter.Name = "pnlFilter";
            this.pnlFilter.Size = new System.Drawing.Size(219, 27);
            this.pnlFilter.TabIndex = 0;
            // 
            // txtFilter
            // 
            this.txtFilter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtFilter.Location = new System.Drawing.Point(38, 4);
            this.txtFilter.Name = "txtFilter";
            this.txtFilter.Size = new System.Drawing.Size(178, 20);
            this.txtFilter.TabIndex = 1;
            this.txtFilter.TextChanged += new System.EventHandler(this.txtFilter_TextChanged);
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(3, 7);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Filter";
            // 
            // tcSpecies
            // 
            this.tcSpecies.Controls.Add(this.tabProjectSpecies);
            this.tcSpecies.Controls.Add(this.tabSpecieDatabase);
            this.tcSpecies.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tcSpecies.Location = new System.Drawing.Point(0, 0);
            this.tcSpecies.Name = "tcSpecies";
            this.tcSpecies.SelectedIndex = 0;
            this.tcSpecies.Size = new System.Drawing.Size(709, 538);
            this.tcSpecies.TabIndex = 1;
            // 
            // tabProjectSpecies
            // 
            this.tabProjectSpecies.Controls.Add(this.grpProjectList);
            this.tabProjectSpecies.Location = new System.Drawing.Point(4, 22);
            this.tabProjectSpecies.Name = "tabProjectSpecies";
            this.tabProjectSpecies.Padding = new System.Windows.Forms.Padding(3);
            this.tabProjectSpecies.Size = new System.Drawing.Size(701, 512);
            this.tabProjectSpecies.TabIndex = 0;
            this.tabProjectSpecies.Text = "Project Species";
            this.tabProjectSpecies.UseVisualStyleBackColor = true;
            // 
            // grpProjectList
            // 
            this.grpProjectList.Controls.Add(this.pnlActiveList);
            this.grpProjectList.Controls.Add(this.pnlActive);
            this.grpProjectList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.grpProjectList.Location = new System.Drawing.Point(3, 3);
            this.grpProjectList.Name = "grpProjectList";
            this.grpProjectList.Size = new System.Drawing.Size(695, 506);
            this.grpProjectList.TabIndex = 0;
            this.grpProjectList.TabStop = false;
            this.grpProjectList.Text = "Project Species";
            // 
            // pnlActiveList
            // 
            this.pnlActiveList.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.pnlActiveList.Controls.Add(this.lstProjectVector);
            this.pnlActiveList.Controls.Add(this.pnlListButtons);
            this.pnlActiveList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlActiveList.Location = new System.Drawing.Point(3, 16);
            this.pnlActiveList.Name = "pnlActiveList";
            this.pnlActiveList.Size = new System.Drawing.Size(689, 328);
            this.pnlActiveList.TabIndex = 2;
            // 
            // lstProjectVector
            // 
            this.lstProjectVector.AllowDrop = true;
            this.lstProjectVector.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chProjSymbol,
            this.chProjMinTemp,
            this.chProjMaxTemp});
            this.lstProjectVector.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lstProjectVector.FullRowSelect = true;
            this.lstProjectVector.HideSelection = false;
            this.lstProjectVector.Location = new System.Drawing.Point(0, 0);
            this.lstProjectVector.Name = "lstProjectVector";
            this.lstProjectVector.Size = new System.Drawing.Size(685, 293);
            this.lstProjectVector.TabIndex = 1;
            this.lstProjectVector.UseCompatibleStateImageBehavior = false;
            this.lstProjectVector.View = System.Windows.Forms.View.Details;
            this.lstProjectVector.DragEnter += new System.Windows.Forms.DragEventHandler(this.lstProjectVector_DragEnter);
            this.lstProjectVector.DragDrop += new System.Windows.Forms.DragEventHandler(this.lstProjectVector_DragDrop);
            this.lstProjectVector.DragOver += new System.Windows.Forms.DragEventHandler(this.lstProjectVector_DragOver);
            this.lstProjectVector.SelectedIndexChanged += new System.EventHandler(this.lstProjectVector_SelectedIndexChanged);
            this.lstProjectVector.KeyDown += new System.Windows.Forms.KeyEventHandler(this.lstProjectVector_KeyDown);
            this.lstProjectVector.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.lstProjectVector_ItemDrag);
            // 
            // chProjSymbol
            // 
            this.chProjSymbol.Text = "Symbol";
            this.chProjSymbol.Width = 296;
            // 
            // chProjMinTemp
            // 
            this.chProjMinTemp.Text = "Minimum Temperature";
            this.chProjMinTemp.Width = 129;
            // 
            // chProjMaxTemp
            // 
            this.chProjMaxTemp.Text = "Maximum Temperature";
            this.chProjMaxTemp.Width = 123;
            // 
            // pnlListButtons
            // 
            this.pnlListButtons.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.pnlListButtons.Controls.Add(this.btnAdd);
            this.pnlListButtons.Controls.Add(this.btnRemove);
            this.pnlListButtons.Controls.Add(this.btnMoveUp);
            this.pnlListButtons.Controls.Add(this.btnMoveDown);
            this.pnlListButtons.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnlListButtons.Location = new System.Drawing.Point(0, 293);
            this.pnlListButtons.Name = "pnlListButtons";
            this.pnlListButtons.Size = new System.Drawing.Size(685, 31);
            this.pnlListButtons.TabIndex = 0;
            // 
            // btnAdd
            // 
            this.btnAdd.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnAdd.Location = new System.Drawing.Point(364, 5);
            this.btnAdd.Name = "btnAdd";
            this.btnAdd.Size = new System.Drawing.Size(75, 23);
            this.btnAdd.TabIndex = 3;
            this.btnAdd.Text = "&Add";
            this.btnAdd.UseVisualStyleBackColor = true;
            this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
            // 
            // btnRemove
            // 
            this.btnRemove.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnRemove.Enabled = false;
            this.btnRemove.Location = new System.Drawing.Point(445, 5);
            this.btnRemove.Name = "btnRemove";
            this.btnRemove.Size = new System.Drawing.Size(75, 23);
            this.btnRemove.TabIndex = 2;
            this.btnRemove.Text = "&Remove";
            this.btnRemove.UseVisualStyleBackColor = true;
            this.btnRemove.Click += new System.EventHandler(this.btnRemove_Click);
            // 
            // btnMoveUp
            // 
            this.btnMoveUp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnMoveUp.Enabled = false;
            this.btnMoveUp.Font = new System.Drawing.Font("Symbol", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.btnMoveUp.Location = new System.Drawing.Point(526, 5);
            this.btnMoveUp.Name = "btnMoveUp";
            this.btnMoveUp.Size = new System.Drawing.Size(75, 23);
            this.btnMoveUp.TabIndex = 1;
            this.btnMoveUp.Text = "­";
            this.btnMoveUp.UseVisualStyleBackColor = true;
            this.btnMoveUp.Click += new System.EventHandler(this.btnMoveUp_Click);
            // 
            // btnMoveDown
            // 
            this.btnMoveDown.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnMoveDown.Enabled = false;
            this.btnMoveDown.Font = new System.Drawing.Font("Symbol", 8.25F);
            this.btnMoveDown.Location = new System.Drawing.Point(607, 5);
            this.btnMoveDown.Name = "btnMoveDown";
            this.btnMoveDown.Size = new System.Drawing.Size(75, 23);
            this.btnMoveDown.TabIndex = 0;
            this.btnMoveDown.Text = "¯";
            this.btnMoveDown.UseVisualStyleBackColor = true;
            this.btnMoveDown.Click += new System.EventHandler(this.btnMoveDown_Click);
            // 
            // pnlActive
            // 
            this.pnlActive.Controls.Add(this.tcIDE);
            this.pnlActive.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnlActive.Location = new System.Drawing.Point(3, 344);
            this.pnlActive.Name = "pnlActive";
            this.pnlActive.Size = new System.Drawing.Size(689, 159);
            this.pnlActive.TabIndex = 2;
            // 
            // tcIDE
            // 
            this.tcIDE.Controls.Add(this.tabIDESpecie);
            this.tcIDE.Controls.Add(this.tabIDECalculation);
            this.tcIDE.Controls.Add(this.tabIDELabel);
            this.tcIDE.Controls.Add(this.tabIDEBlank);
            this.tcIDE.Controls.Add(this.tabIDEAttribute);
            this.tcIDE.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tcIDE.Location = new System.Drawing.Point(0, 0);
            this.tcIDE.Name = "tcIDE";
            this.tcIDE.SelectedIndex = 0;
            this.tcIDE.Size = new System.Drawing.Size(689, 159);
            this.tcIDE.TabIndex = 1;
            // 
            // tabIDESpecie
            // 
            this.tabIDESpecie.Controls.Add(this.grpSpecie);
            this.tabIDESpecie.Location = new System.Drawing.Point(4, 22);
            this.tabIDESpecie.Name = "tabIDESpecie";
            this.tabIDESpecie.Padding = new System.Windows.Forms.Padding(3);
            this.tabIDESpecie.Size = new System.Drawing.Size(681, 133);
            this.tabIDESpecie.TabIndex = 0;
            this.tabIDESpecie.Text = "Specie";
            this.tabIDESpecie.UseVisualStyleBackColor = true;
            // 
            // grpSpecie
            // 
            this.grpSpecie.Controls.Add(this.groupBox1);
            this.grpSpecie.Controls.Add(this.chkIdeal);
            this.grpSpecie.Dock = System.Windows.Forms.DockStyle.Fill;
            this.grpSpecie.Location = new System.Drawing.Point(3, 3);
            this.grpSpecie.Name = "grpSpecie";
            this.grpSpecie.Size = new System.Drawing.Size(675, 127);
            this.grpSpecie.TabIndex = 1;
            this.grpSpecie.TabStop = false;
            this.grpSpecie.Text = "grpSpecie";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.tableLayoutPanel1);
            this.groupBox1.Location = new System.Drawing.Point(104, 19);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(334, 100);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Temperature Range (°C)";
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 4;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.Controls.Add(this.numLowerTempTolerance, 1, 1);
            this.tableLayoutPanel1.Controls.Add(this.label6, 0, 2);
            this.tableLayoutPanel1.Controls.Add(this.label4, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.txtDefinedMinTemp, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this.txtDefinedMaxTemp, 3, 0);
            this.tableLayoutPanel1.Controls.Add(this.txtAllowedMaxTemp, 3, 2);
            this.tableLayoutPanel1.Controls.Add(this.txtAllowedMinTemp, 1, 2);
            this.tableLayoutPanel1.Controls.Add(this.label2, 2, 0);
            this.tableLayoutPanel1.Controls.Add(this.label3, 2, 2);
            this.tableLayoutPanel1.Controls.Add(this.label5, 0, 1);
            this.tableLayoutPanel1.Controls.Add(this.numGreaterTempTolerance, 3, 1);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(3, 16);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 3;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(328, 81);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // numLowerTempTolerance
            // 
            this.numLowerTempTolerance.DecimalPlaces = 1;
            this.numLowerTempTolerance.Location = new System.Drawing.Point(64, 29);
            this.numLowerTempTolerance.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.numLowerTempTolerance.Name = "numLowerTempTolerance";
            this.numLowerTempTolerance.Size = new System.Drawing.Size(116, 20);
            this.numLowerTempTolerance.TabIndex = 12;
            this.numLowerTempTolerance.ValueChanged += new System.EventHandler(this.numLowerTempTolerance_ValueChanged);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label6.Location = new System.Drawing.Point(3, 52);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(55, 29);
            this.label6.TabIndex = 10;
            this.label6.Text = "Allowed";
            this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label4.Location = new System.Drawing.Point(3, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(55, 26);
            this.label4.TabIndex = 8;
            this.label4.Text = "Defined";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // txtDefinedMinTemp
            // 
            this.txtDefinedMinTemp.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtDefinedMinTemp.Location = new System.Drawing.Point(64, 3);
            this.txtDefinedMinTemp.Name = "txtDefinedMinTemp";
            this.txtDefinedMinTemp.ReadOnly = true;
            this.txtDefinedMinTemp.Size = new System.Drawing.Size(116, 20);
            this.txtDefinedMinTemp.TabIndex = 0;
            // 
            // txtDefinedMaxTemp
            // 
            this.txtDefinedMaxTemp.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtDefinedMaxTemp.Location = new System.Drawing.Point(208, 3);
            this.txtDefinedMaxTemp.Name = "txtDefinedMaxTemp";
            this.txtDefinedMaxTemp.ReadOnly = true;
            this.txtDefinedMaxTemp.Size = new System.Drawing.Size(117, 20);
            this.txtDefinedMaxTemp.TabIndex = 1;
            // 
            // txtAllowedMaxTemp
            // 
            this.txtAllowedMaxTemp.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtAllowedMaxTemp.Location = new System.Drawing.Point(208, 55);
            this.txtAllowedMaxTemp.Name = "txtAllowedMaxTemp";
            this.txtAllowedMaxTemp.ReadOnly = true;
            this.txtAllowedMaxTemp.Size = new System.Drawing.Size(117, 20);
            this.txtAllowedMaxTemp.TabIndex = 5;
            // 
            // txtAllowedMinTemp
            // 
            this.txtAllowedMinTemp.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtAllowedMinTemp.Location = new System.Drawing.Point(64, 55);
            this.txtAllowedMinTemp.Name = "txtAllowedMinTemp";
            this.txtAllowedMinTemp.ReadOnly = true;
            this.txtAllowedMinTemp.Size = new System.Drawing.Size(116, 20);
            this.txtAllowedMinTemp.TabIndex = 3;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label2.Location = new System.Drawing.Point(186, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(16, 26);
            this.label2.TabIndex = 6;
            this.label2.Text = "to";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label3.Location = new System.Drawing.Point(186, 52);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(16, 29);
            this.label3.TabIndex = 7;
            this.label3.Text = "to";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label5.Location = new System.Drawing.Point(3, 26);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(55, 26);
            this.label5.TabIndex = 9;
            this.label5.Text = "Tolerance";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // numGreaterTempTolerance
            // 
            this.numGreaterTempTolerance.DecimalPlaces = 1;
            this.numGreaterTempTolerance.Location = new System.Drawing.Point(208, 29);
            this.numGreaterTempTolerance.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.numGreaterTempTolerance.Name = "numGreaterTempTolerance";
            this.numGreaterTempTolerance.Size = new System.Drawing.Size(117, 20);
            this.numGreaterTempTolerance.TabIndex = 11;
            this.numGreaterTempTolerance.ValueChanged += new System.EventHandler(this.numGreaterTempTolerance_ValueChanged);
            // 
            // chkIdeal
            // 
            this.chkIdeal.Location = new System.Drawing.Point(6, 19);
            this.chkIdeal.Name = "chkIdeal";
            this.chkIdeal.Size = new System.Drawing.Size(92, 36);
            this.chkIdeal.TabIndex = 2;
            this.chkIdeal.Text = "Use Ideal Formulation";
            this.chkIdeal.UseVisualStyleBackColor = true;
            this.chkIdeal.CheckedChanged += new System.EventHandler(this.chkIdeal_CheckedChanged);
            // 
            // tabIDECalculation
            // 
            this.tabIDECalculation.Controls.Add(this.grpCalculation);
            this.tabIDECalculation.Location = new System.Drawing.Point(4, 22);
            this.tabIDECalculation.Name = "tabIDECalculation";
            this.tabIDECalculation.Padding = new System.Windows.Forms.Padding(3);
            this.tabIDECalculation.Size = new System.Drawing.Size(681, 133);
            this.tabIDECalculation.TabIndex = 1;
            this.tabIDECalculation.Text = "Calculation";
            this.tabIDECalculation.UseVisualStyleBackColor = true;
            // 
            // grpCalculation
            // 
            this.grpCalculation.Controls.Add(this.txtCalcSymbol);
            this.grpCalculation.Controls.Add(this.label26);
            this.grpCalculation.Controls.Add(this.label24);
            this.grpCalculation.Controls.Add(this.txtCalcName);
            this.grpCalculation.Controls.Add(this.label23);
            this.grpCalculation.Controls.Add(this.txtCalculation);
            this.grpCalculation.Dock = System.Windows.Forms.DockStyle.Fill;
            this.grpCalculation.Location = new System.Drawing.Point(3, 3);
            this.grpCalculation.Name = "grpCalculation";
            this.grpCalculation.Size = new System.Drawing.Size(675, 127);
            this.grpCalculation.TabIndex = 1;
            this.grpCalculation.TabStop = false;
            this.grpCalculation.Text = "Calculation";
            // 
            // txtCalcSymbol
            // 
            this.txtCalcSymbol.Location = new System.Drawing.Point(250, 15);
            this.txtCalcSymbol.Name = "txtCalcSymbol";
            this.txtCalcSymbol.Size = new System.Drawing.Size(100, 20);
            this.txtCalcSymbol.TabIndex = 5;
            this.txtCalcSymbol.TextChanged += new System.EventHandler(this.txtCalcSymbol_TextChanged);
            // 
            // label26
            // 
            this.label26.AutoSize = true;
            this.label26.Location = new System.Drawing.Point(203, 18);
            this.label26.Name = "label26";
            this.label26.Size = new System.Drawing.Size(41, 13);
            this.label26.TabIndex = 4;
            this.label26.Text = "Symbol";
            // 
            // label24
            // 
            this.label24.AutoSize = true;
            this.label24.Location = new System.Drawing.Point(6, 44);
            this.label24.Name = "label24";
            this.label24.Size = new System.Drawing.Size(62, 13);
            this.label24.TabIndex = 3;
            this.label24.Text = "Calculation:";
            // 
            // txtCalcName
            // 
            this.txtCalcName.Location = new System.Drawing.Point(77, 15);
            this.txtCalcName.Name = "txtCalcName";
            this.txtCalcName.Size = new System.Drawing.Size(100, 20);
            this.txtCalcName.TabIndex = 2;
            this.txtCalcName.TextChanged += new System.EventHandler(this.txtCalcName_TextChanged);
            // 
            // label23
            // 
            this.label23.AutoSize = true;
            this.label23.Location = new System.Drawing.Point(33, 18);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(35, 13);
            this.label23.TabIndex = 1;
            this.label23.Text = "Name";
            // 
            // txtCalculation
            // 
            this.txtCalculation.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtCalculation.Location = new System.Drawing.Point(77, 41);
            this.txtCalculation.Name = "txtCalculation";
            this.txtCalculation.Size = new System.Drawing.Size(595, 83);
            this.txtCalculation.TabIndex = 0;
            this.txtCalculation.Text = "";
            this.txtCalculation.TextChanged += new System.EventHandler(this.txtCalculation_TextChanged);
            // 
            // tabIDELabel
            // 
            this.tabIDELabel.Controls.Add(this.grpTextValue);
            this.tabIDELabel.Location = new System.Drawing.Point(4, 22);
            this.tabIDELabel.Name = "tabIDELabel";
            this.tabIDELabel.Padding = new System.Windows.Forms.Padding(3);
            this.tabIDELabel.Size = new System.Drawing.Size(681, 133);
            this.tabIDELabel.TabIndex = 2;
            this.tabIDELabel.Text = "Text";
            this.tabIDELabel.UseVisualStyleBackColor = true;
            // 
            // grpTextValue
            // 
            this.grpTextValue.Controls.Add(this.txtText);
            this.grpTextValue.Dock = System.Windows.Forms.DockStyle.Fill;
            this.grpTextValue.Location = new System.Drawing.Point(3, 3);
            this.grpTextValue.Name = "grpTextValue";
            this.grpTextValue.Size = new System.Drawing.Size(675, 127);
            this.grpTextValue.TabIndex = 0;
            this.grpTextValue.TabStop = false;
            this.grpTextValue.Text = "Value";
            // 
            // txtText
            // 
            this.txtText.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtText.Location = new System.Drawing.Point(3, 16);
            this.txtText.Name = "txtText";
            this.txtText.Size = new System.Drawing.Size(669, 20);
            this.txtText.TabIndex = 0;
            this.txtText.TextChanged += new System.EventHandler(this.txtText_TextChanged);
            // 
            // tabIDEBlank
            // 
            this.tabIDEBlank.Controls.Add(this.pnlBlank);
            this.tabIDEBlank.Location = new System.Drawing.Point(4, 22);
            this.tabIDEBlank.Name = "tabIDEBlank";
            this.tabIDEBlank.Padding = new System.Windows.Forms.Padding(3);
            this.tabIDEBlank.Size = new System.Drawing.Size(681, 133);
            this.tabIDEBlank.TabIndex = 3;
            this.tabIDEBlank.Text = "Blank";
            this.tabIDEBlank.UseVisualStyleBackColor = true;
            // 
            // pnlBlank
            // 
            this.pnlBlank.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlBlank.Location = new System.Drawing.Point(3, 3);
            this.pnlBlank.Name = "pnlBlank";
            this.pnlBlank.Size = new System.Drawing.Size(675, 127);
            this.pnlBlank.TabIndex = 0;
            // 
            // tabIDEAttribute
            // 
            this.tabIDEAttribute.Controls.Add(this.grpAttribute);
            this.tabIDEAttribute.Location = new System.Drawing.Point(4, 22);
            this.tabIDEAttribute.Name = "tabIDEAttribute";
            this.tabIDEAttribute.Padding = new System.Windows.Forms.Padding(3);
            this.tabIDEAttribute.Size = new System.Drawing.Size(681, 133);
            this.tabIDEAttribute.TabIndex = 4;
            this.tabIDEAttribute.Text = "Attribute";
            this.tabIDEAttribute.UseVisualStyleBackColor = true;
            // 
            // grpAttribute
            // 
            this.grpAttribute.Controls.Add(this.comboAttributeType);
            this.grpAttribute.Controls.Add(this.label8);
            this.grpAttribute.Controls.Add(this.txtAttributeName);
            this.grpAttribute.Controls.Add(this.label7);
            this.grpAttribute.Dock = System.Windows.Forms.DockStyle.Fill;
            this.grpAttribute.Location = new System.Drawing.Point(3, 3);
            this.grpAttribute.Name = "grpAttribute";
            this.grpAttribute.Size = new System.Drawing.Size(675, 127);
            this.grpAttribute.TabIndex = 0;
            this.grpAttribute.TabStop = false;
            this.grpAttribute.Text = "Attribute Details";
            // 
            // comboAttributeType
            // 
            this.comboAttributeType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboAttributeType.FormattingEnabled = true;
            this.comboAttributeType.Items.AddRange(new object[] {
            "Extensive",
            "Intensive"});
            this.comboAttributeType.Location = new System.Drawing.Point(47, 45);
            this.comboAttributeType.Name = "comboAttributeType";
            this.comboAttributeType.Size = new System.Drawing.Size(121, 21);
            this.comboAttributeType.TabIndex = 3;
            this.comboAttributeType.SelectedIndexChanged += new System.EventHandler(this.comboAttributeType_SelectedIndexChanged);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(6, 48);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(31, 13);
            this.label8.TabIndex = 2;
            this.label8.Text = "Type";
            // 
            // txtAttributeName
            // 
            this.txtAttributeName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtAttributeName.Location = new System.Drawing.Point(47, 19);
            this.txtAttributeName.Name = "txtAttributeName";
            this.txtAttributeName.Size = new System.Drawing.Size(622, 20);
            this.txtAttributeName.TabIndex = 1;
            this.txtAttributeName.TextChanged += new System.EventHandler(this.txtAttributeName_TextChanged);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(6, 22);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(35, 13);
            this.label7.TabIndex = 0;
            this.label7.Text = "Name";
            // 
            // tabSpecieDatabase
            // 
            this.tabSpecieDatabase.AutoScroll = true;
            this.tabSpecieDatabase.Controls.Add(this.groupBox6);
            this.tabSpecieDatabase.Controls.Add(this.groupBox8);
            this.tabSpecieDatabase.Controls.Add(this.groupBox5);
            this.tabSpecieDatabase.Controls.Add(this.groupBox4);
            this.tabSpecieDatabase.Controls.Add(this.grpNomenclature);
            this.tabSpecieDatabase.Location = new System.Drawing.Point(4, 22);
            this.tabSpecieDatabase.Name = "tabSpecieDatabase";
            this.tabSpecieDatabase.Padding = new System.Windows.Forms.Padding(3);
            this.tabSpecieDatabase.Size = new System.Drawing.Size(701, 512);
            this.tabSpecieDatabase.TabIndex = 1;
            this.tabSpecieDatabase.Text = "Specie Database";
            this.tabSpecieDatabase.UseVisualStyleBackColor = true;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.splitContainer2);
            this.groupBox6.Controls.Add(this.pnlTempDependantRadios);
            this.groupBox6.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox6.Location = new System.Drawing.Point(3, 286);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(695, 223);
            this.groupBox6.TabIndex = 19;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Temperature Dependant Properties";
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(130, 16);
            this.splitContainer2.Name = "splitContainer2";
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.graph1);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.tlpEquations);
            this.splitContainer2.Size = new System.Drawing.Size(562, 204);
            this.splitContainer2.SplitterDistance = 226;
            this.splitContainer2.TabIndex = 10;
            // 
            // graph1
            // 
            this.graph1.BackColor = System.Drawing.Color.LightGray;
            this.graph1.Border = 5;
            this.graph1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.graph1.CheckSize = 5;
            this.graph1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.graph1.DrawYLabel = true;
            this.graph1.Location = new System.Drawing.Point(0, 0);
            this.graph1.MaxChecks = 6;
            this.graph1.MaxXValue = 0;
            this.graph1.MinXValue = 0;
            this.graph1.Name = "graph1";
            this.graph1.Size = new System.Drawing.Size(226, 204);
            this.graph1.TabIndex = 6;
            this.graph1.UnselectedTransparency = 64;
            this.graph1.XLabel = "Temperature (K)";
            this.graph1.YLabel = null;
            this.graph1.YLabelColour = System.Drawing.Color.Empty;
            // 
            // tlpEquations
            // 
            this.tlpEquations.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.tlpEquations.ColumnCount = 4;
            this.tlpEquations.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tlpEquations.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 50F));
            this.tlpEquations.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 15F));
            this.tlpEquations.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 50F));
            this.tlpEquations.Controls.Add(this.txtMaxTemp3, 3, 4);
            this.tlpEquations.Controls.Add(this.txtMinTemp3, 1, 4);
            this.tlpEquations.Controls.Add(this.txtMaxTemp2, 3, 3);
            this.tlpEquations.Controls.Add(this.txtMinTemp2, 1, 3);
            this.tlpEquations.Controls.Add(this.txtMaxTemp1, 3, 2);
            this.tlpEquations.Controls.Add(this.txtMinTemp1, 1, 2);
            this.tlpEquations.Controls.Add(this.txtMaxTemp0, 3, 1);
            this.tlpEquations.Controls.Add(this.txtMinTemp0, 1, 1);
            this.tlpEquations.Controls.Add(this.label38, 0, 0);
            this.tlpEquations.Controls.Add(this.label39, 1, 0);
            this.tlpEquations.Controls.Add(this.label40, 2, 1);
            this.tlpEquations.Controls.Add(this.label45, 2, 3);
            this.tlpEquations.Controls.Add(this.label41, 2, 2);
            this.tlpEquations.Controls.Add(this.label42, 2, 4);
            this.tlpEquations.Controls.Add(this.txtFormula0, 0, 1);
            this.tlpEquations.Controls.Add(this.txtFormula1, 0, 2);
            this.tlpEquations.Controls.Add(this.txtFormula2, 0, 3);
            this.tlpEquations.Controls.Add(this.txtFormula3, 0, 4);
            this.tlpEquations.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tlpEquations.Location = new System.Drawing.Point(0, 0);
            this.tlpEquations.Name = "tlpEquations";
            this.tlpEquations.RowCount = 5;
            this.tlpEquations.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tlpEquations.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tlpEquations.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tlpEquations.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tlpEquations.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tlpEquations.Size = new System.Drawing.Size(332, 204);
            this.tlpEquations.TabIndex = 9;
            // 
            // txtMaxTemp3
            // 
            this.txtMaxTemp3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMaxTemp3.Location = new System.Drawing.Point(285, 115);
            this.txtMaxTemp3.Name = "txtMaxTemp3";
            this.txtMaxTemp3.Size = new System.Drawing.Size(44, 20);
            this.txtMaxTemp3.TabIndex = 24;
            this.txtMaxTemp3.Leave += new System.EventHandler(this.txtFormula0_Leave);
            // 
            // txtMinTemp3
            // 
            this.txtMinTemp3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMinTemp3.Location = new System.Drawing.Point(220, 115);
            this.txtMinTemp3.Name = "txtMinTemp3";
            this.txtMinTemp3.Size = new System.Drawing.Size(44, 20);
            this.txtMinTemp3.TabIndex = 23;
            this.txtMinTemp3.Leave += new System.EventHandler(this.txtFormula0_Leave);
            // 
            // txtMaxTemp2
            // 
            this.txtMaxTemp2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMaxTemp2.Location = new System.Drawing.Point(285, 89);
            this.txtMaxTemp2.Name = "txtMaxTemp2";
            this.txtMaxTemp2.Size = new System.Drawing.Size(44, 20);
            this.txtMaxTemp2.TabIndex = 22;
            this.txtMaxTemp2.Leave += new System.EventHandler(this.txtFormula0_Leave);
            // 
            // txtMinTemp2
            // 
            this.txtMinTemp2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMinTemp2.Location = new System.Drawing.Point(220, 89);
            this.txtMinTemp2.Name = "txtMinTemp2";
            this.txtMinTemp2.Size = new System.Drawing.Size(44, 20);
            this.txtMinTemp2.TabIndex = 21;
            this.txtMinTemp2.Leave += new System.EventHandler(this.txtFormula0_Leave);
            // 
            // txtMaxTemp1
            // 
            this.txtMaxTemp1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMaxTemp1.Location = new System.Drawing.Point(285, 63);
            this.txtMaxTemp1.Name = "txtMaxTemp1";
            this.txtMaxTemp1.Size = new System.Drawing.Size(44, 20);
            this.txtMaxTemp1.TabIndex = 20;
            this.txtMaxTemp1.Leave += new System.EventHandler(this.txtFormula0_Leave);
            // 
            // txtMinTemp1
            // 
            this.txtMinTemp1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMinTemp1.Location = new System.Drawing.Point(220, 63);
            this.txtMinTemp1.Name = "txtMinTemp1";
            this.txtMinTemp1.Size = new System.Drawing.Size(44, 20);
            this.txtMinTemp1.TabIndex = 19;
            this.txtMinTemp1.Leave += new System.EventHandler(this.txtFormula0_Leave);
            // 
            // txtMaxTemp0
            // 
            this.txtMaxTemp0.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMaxTemp0.Location = new System.Drawing.Point(285, 37);
            this.txtMaxTemp0.Name = "txtMaxTemp0";
            this.txtMaxTemp0.Size = new System.Drawing.Size(44, 20);
            this.txtMaxTemp0.TabIndex = 18;
            this.txtMaxTemp0.Leave += new System.EventHandler(this.txtFormula0_Leave);
            // 
            // txtMinTemp0
            // 
            this.txtMinTemp0.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMinTemp0.Location = new System.Drawing.Point(220, 37);
            this.txtMinTemp0.Name = "txtMinTemp0";
            this.txtMinTemp0.Size = new System.Drawing.Size(44, 20);
            this.txtMinTemp0.TabIndex = 17;
            this.txtMinTemp0.Leave += new System.EventHandler(this.txtFormula0_Leave);
            // 
            // label38
            // 
            this.label38.AutoSize = true;
            this.label38.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label38.Location = new System.Drawing.Point(3, 0);
            this.label38.Name = "label38";
            this.label38.Padding = new System.Windows.Forms.Padding(4);
            this.label38.Size = new System.Drawing.Size(211, 34);
            this.label38.TabIndex = 7;
            this.label38.Text = "Equation";
            this.label38.TextAlign = System.Drawing.ContentAlignment.BottomLeft;
            // 
            // label39
            // 
            this.label39.AutoSize = true;
            this.tlpEquations.SetColumnSpan(this.label39, 3);
            this.label39.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label39.Location = new System.Drawing.Point(220, 0);
            this.label39.Name = "label39";
            this.label39.Padding = new System.Windows.Forms.Padding(4);
            this.label39.Size = new System.Drawing.Size(109, 34);
            this.label39.TabIndex = 8;
            this.label39.Text = "Temperature Range";
            this.label39.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            // 
            // label40
            // 
            this.label40.AutoSize = true;
            this.label40.Location = new System.Drawing.Point(270, 34);
            this.label40.Name = "label40";
            this.label40.Size = new System.Drawing.Size(9, 13);
            this.label40.TabIndex = 9;
            this.label40.Text = "-";
            // 
            // label45
            // 
            this.label45.AutoSize = true;
            this.label45.Location = new System.Drawing.Point(270, 86);
            this.label45.Name = "label45";
            this.label45.Size = new System.Drawing.Size(9, 13);
            this.label45.TabIndex = 12;
            this.label45.Text = "-";
            // 
            // label41
            // 
            this.label41.AutoSize = true;
            this.label41.Location = new System.Drawing.Point(270, 60);
            this.label41.Name = "label41";
            this.label41.Size = new System.Drawing.Size(9, 13);
            this.label41.TabIndex = 10;
            this.label41.Text = "-";
            // 
            // label42
            // 
            this.label42.AutoSize = true;
            this.label42.Location = new System.Drawing.Point(270, 112);
            this.label42.Name = "label42";
            this.label42.Size = new System.Drawing.Size(9, 13);
            this.label42.TabIndex = 11;
            this.label42.Text = "-";
            // 
            // txtFormula0
            // 
            this.txtFormula0.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtFormula0.Location = new System.Drawing.Point(3, 37);
            this.txtFormula0.Name = "txtFormula0";
            this.txtFormula0.Size = new System.Drawing.Size(211, 20);
            this.txtFormula0.TabIndex = 13;
            this.txtFormula0.Leave += new System.EventHandler(this.txtFormula0_Leave);
            // 
            // txtFormula1
            // 
            this.txtFormula1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtFormula1.Location = new System.Drawing.Point(3, 63);
            this.txtFormula1.Name = "txtFormula1";
            this.txtFormula1.Size = new System.Drawing.Size(211, 20);
            this.txtFormula1.TabIndex = 14;
            this.txtFormula1.Leave += new System.EventHandler(this.txtFormula0_Leave);
            // 
            // txtFormula2
            // 
            this.txtFormula2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtFormula2.Location = new System.Drawing.Point(3, 89);
            this.txtFormula2.Name = "txtFormula2";
            this.txtFormula2.Size = new System.Drawing.Size(211, 20);
            this.txtFormula2.TabIndex = 16;
            this.txtFormula2.Leave += new System.EventHandler(this.txtFormula0_Leave);
            // 
            // txtFormula3
            // 
            this.txtFormula3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtFormula3.Location = new System.Drawing.Point(3, 115);
            this.txtFormula3.Name = "txtFormula3";
            this.txtFormula3.Size = new System.Drawing.Size(211, 20);
            this.txtFormula3.TabIndex = 15;
            this.txtFormula3.Leave += new System.EventHandler(this.txtFormula0_Leave);
            // 
            // pnlTempDependantRadios
            // 
            this.pnlTempDependantRadios.Controls.Add(this.label37);
            this.pnlTempDependantRadios.Controls.Add(this.radioDensity);
            this.pnlTempDependantRadios.Controls.Add(this.radioEntropy);
            this.pnlTempDependantRadios.Controls.Add(this.radioHeatOfFormation);
            this.pnlTempDependantRadios.Controls.Add(this.radioHeatCapacity);
            this.pnlTempDependantRadios.Controls.Add(this.radioVapourPressure);
            this.pnlTempDependantRadios.Dock = System.Windows.Forms.DockStyle.Left;
            this.pnlTempDependantRadios.Location = new System.Drawing.Point(3, 16);
            this.pnlTempDependantRadios.Name = "pnlTempDependantRadios";
            this.pnlTempDependantRadios.Size = new System.Drawing.Size(127, 204);
            this.pnlTempDependantRadios.TabIndex = 11;
            // 
            // label37
            // 
            this.label37.AutoSize = true;
            this.label37.Location = new System.Drawing.Point(3, 0);
            this.label37.Name = "label37";
            this.label37.Size = new System.Drawing.Size(92, 13);
            this.label37.TabIndex = 0;
            this.label37.Text = "Adjust Values For:";
            // 
            // radioDensity
            // 
            this.radioDensity.AutoSize = true;
            this.radioDensity.Checked = true;
            this.radioDensity.ForeColor = System.Drawing.Color.Navy;
            this.radioDensity.Location = new System.Drawing.Point(3, 16);
            this.radioDensity.Name = "radioDensity";
            this.radioDensity.Size = new System.Drawing.Size(60, 17);
            this.radioDensity.TabIndex = 1;
            this.radioDensity.TabStop = true;
            this.radioDensity.Tag = "Density";
            this.radioDensity.Text = "Density";
            this.radioDensity.UseVisualStyleBackColor = true;
            this.radioDensity.CheckedChanged += new System.EventHandler(this.radioEntropy_CheckedChanged);
            // 
            // radioEntropy
            // 
            this.radioEntropy.AutoSize = true;
            this.radioEntropy.ForeColor = System.Drawing.Color.MediumSlateBlue;
            this.radioEntropy.Location = new System.Drawing.Point(3, 108);
            this.radioEntropy.Name = "radioEntropy";
            this.radioEntropy.Size = new System.Drawing.Size(76, 17);
            this.radioEntropy.TabIndex = 5;
            this.radioEntropy.Tag = "S25";
            this.radioEntropy.Text = "Entropy (?)";
            this.radioEntropy.UseVisualStyleBackColor = true;
            this.radioEntropy.CheckedChanged += new System.EventHandler(this.radioEntropy_CheckedChanged);
            // 
            // radioHeatOfFormation
            // 
            this.radioHeatOfFormation.AutoSize = true;
            this.radioHeatOfFormation.ForeColor = System.Drawing.Color.DarkRed;
            this.radioHeatOfFormation.Location = new System.Drawing.Point(3, 39);
            this.radioHeatOfFormation.Name = "radioHeatOfFormation";
            this.radioHeatOfFormation.Size = new System.Drawing.Size(109, 17);
            this.radioHeatOfFormation.TabIndex = 2;
            this.radioHeatOfFormation.Tag = "Hf25";
            this.radioHeatOfFormation.Text = "Heat of Formation";
            this.radioHeatOfFormation.UseVisualStyleBackColor = true;
            this.radioHeatOfFormation.CheckedChanged += new System.EventHandler(this.radioEntropy_CheckedChanged);
            // 
            // radioHeatCapacity
            // 
            this.radioHeatCapacity.AutoSize = true;
            this.radioHeatCapacity.ForeColor = System.Drawing.Color.DarkOrange;
            this.radioHeatCapacity.Location = new System.Drawing.Point(3, 62);
            this.radioHeatCapacity.Name = "radioHeatCapacity";
            this.radioHeatCapacity.Size = new System.Drawing.Size(92, 17);
            this.radioHeatCapacity.TabIndex = 4;
            this.radioHeatCapacity.Tag = "Cp";
            this.radioHeatCapacity.Text = "Heat Capacity";
            this.radioHeatCapacity.UseVisualStyleBackColor = true;
            this.radioHeatCapacity.CheckedChanged += new System.EventHandler(this.radioEntropy_CheckedChanged);
            // 
            // radioVapourPressure
            // 
            this.radioVapourPressure.AutoSize = true;
            this.radioVapourPressure.ForeColor = System.Drawing.Color.DarkGreen;
            this.radioVapourPressure.Location = new System.Drawing.Point(3, 85);
            this.radioVapourPressure.Name = "radioVapourPressure";
            this.radioVapourPressure.Size = new System.Drawing.Size(103, 17);
            this.radioVapourPressure.TabIndex = 3;
            this.radioVapourPressure.Tag = "Vp";
            this.radioVapourPressure.Text = "Vapour Pressure";
            this.radioVapourPressure.UseVisualStyleBackColor = true;
            this.radioVapourPressure.CheckedChanged += new System.EventHandler(this.radioEntropy_CheckedChanged);
            // 
            // groupBox8
            // 
            this.groupBox8.Controls.Add(this.txtSolvent);
            this.groupBox8.Controls.Add(this.label36);
            this.groupBox8.Controls.Add(this.txtAccentricity);
            this.groupBox8.Controls.Add(this.txtCritTemp);
            this.groupBox8.Controls.Add(this.label47);
            this.groupBox8.Controls.Add(this.txtCritVol);
            this.groupBox8.Controls.Add(this.label46);
            this.groupBox8.Controls.Add(this.txtCritPressure);
            this.groupBox8.Controls.Add(this.label43);
            this.groupBox8.Controls.Add(this.label44);
            this.groupBox8.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox8.Location = new System.Drawing.Point(3, 185);
            this.groupBox8.Name = "groupBox8";
            this.groupBox8.Size = new System.Drawing.Size(695, 101);
            this.groupBox8.TabIndex = 18;
            this.groupBox8.TabStop = false;
            this.groupBox8.Text = "Thermodynamic Properties";
            // 
            // txtSolvent
            // 
            this.txtSolvent.Location = new System.Drawing.Point(125, 19);
            this.txtSolvent.Name = "txtSolvent";
            this.txtSolvent.Size = new System.Drawing.Size(130, 20);
            this.txtSolvent.TabIndex = 17;
            // 
            // label36
            // 
            this.label36.AutoSize = true;
            this.label36.Location = new System.Drawing.Point(76, 22);
            this.label36.Name = "label36";
            this.label36.Size = new System.Drawing.Size(43, 13);
            this.label36.TabIndex = 16;
            this.label36.Text = "Solvent";
            // 
            // txtAccentricity
            // 
            this.txtAccentricity.Location = new System.Drawing.Point(387, 71);
            this.txtAccentricity.Name = "txtAccentricity";
            this.txtAccentricity.Size = new System.Drawing.Size(130, 20);
            this.txtAccentricity.TabIndex = 15;
            // 
            // txtCritTemp
            // 
            this.txtCritTemp.Location = new System.Drawing.Point(387, 45);
            this.txtCritTemp.Name = "txtCritTemp";
            this.txtCritTemp.Size = new System.Drawing.Size(130, 20);
            this.txtCritTemp.TabIndex = 14;
            // 
            // label47
            // 
            this.label47.AutoSize = true;
            this.label47.Location = new System.Drawing.Point(319, 74);
            this.label47.Name = "label47";
            this.label47.Size = new System.Drawing.Size(62, 13);
            this.label47.TabIndex = 13;
            this.label47.Text = "Accentricity";
            // 
            // txtCritVol
            // 
            this.txtCritVol.Location = new System.Drawing.Point(125, 71);
            this.txtCritVol.Name = "txtCritVol";
            this.txtCritVol.Size = new System.Drawing.Size(130, 20);
            this.txtCritVol.TabIndex = 12;
            // 
            // label46
            // 
            this.label46.AutoSize = true;
            this.label46.Location = new System.Drawing.Point(2, 74);
            this.label46.Name = "label46";
            this.label46.Size = new System.Drawing.Size(117, 13);
            this.label46.TabIndex = 11;
            this.label46.Text = "Specific Critical Volume";
            // 
            // txtCritPressure
            // 
            this.txtCritPressure.Location = new System.Drawing.Point(125, 45);
            this.txtCritPressure.Name = "txtCritPressure";
            this.txtCritPressure.Size = new System.Drawing.Size(130, 20);
            this.txtCritPressure.TabIndex = 10;
            // 
            // label43
            // 
            this.label43.AutoSize = true;
            this.label43.Location = new System.Drawing.Point(280, 48);
            this.label43.Name = "label43";
            this.label43.Size = new System.Drawing.Size(101, 13);
            this.label43.TabIndex = 8;
            this.label43.Text = "Critical Temperature";
            // 
            // label44
            // 
            this.label44.AutoSize = true;
            this.label44.Location = new System.Drawing.Point(37, 48);
            this.label44.Name = "label44";
            this.label44.Size = new System.Drawing.Size(82, 13);
            this.label44.TabIndex = 7;
            this.label44.Text = "Critical Pressure";
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.txtCheckedBy);
            this.groupBox5.Controls.Add(this.label35);
            this.groupBox5.Controls.Add(this.label34);
            this.groupBox5.Controls.Add(this.txtSource);
            this.groupBox5.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox5.Location = new System.Drawing.Point(3, 134);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(695, 51);
            this.groupBox5.TabIndex = 15;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Source";
            // 
            // txtCheckedBy
            // 
            this.txtCheckedBy.Location = new System.Drawing.Point(387, 19);
            this.txtCheckedBy.Name = "txtCheckedBy";
            this.txtCheckedBy.Size = new System.Drawing.Size(130, 20);
            this.txtCheckedBy.TabIndex = 3;
            // 
            // label35
            // 
            this.label35.AutoSize = true;
            this.label35.Location = new System.Drawing.Point(62, 22);
            this.label35.Name = "label35";
            this.label35.Size = new System.Drawing.Size(57, 13);
            this.label35.TabIndex = 2;
            this.label35.Text = "Reference";
            // 
            // label34
            // 
            this.label34.AutoSize = true;
            this.label34.Location = new System.Drawing.Point(316, 22);
            this.label34.Name = "label34";
            this.label34.Size = new System.Drawing.Size(65, 13);
            this.label34.TabIndex = 1;
            this.label34.Text = "Checked By";
            // 
            // txtSource
            // 
            this.txtSource.Location = new System.Drawing.Point(125, 19);
            this.txtSource.Name = "txtSource";
            this.txtSource.Size = new System.Drawing.Size(130, 20);
            this.txtSource.TabIndex = 0;
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.comboOccurence);
            this.groupBox4.Controls.Add(this.label33);
            this.groupBox4.Controls.Add(this.txtMaxTemperature);
            this.groupBox4.Controls.Add(this.txtMinTemperature);
            this.groupBox4.Controls.Add(this.txtElementalComposition);
            this.groupBox4.Controls.Add(this.label29);
            this.groupBox4.Controls.Add(this.label27);
            this.groupBox4.Controls.Add(this.label28);
            this.groupBox4.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox4.Location = new System.Drawing.Point(3, 52);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(695, 82);
            this.groupBox4.TabIndex = 14;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "General Properties";
            // 
            // comboOccurence
            // 
            this.comboOccurence.FormattingEnabled = true;
            this.comboOccurence.Location = new System.Drawing.Point(387, 18);
            this.comboOccurence.Name = "comboOccurence";
            this.comboOccurence.Size = new System.Drawing.Size(130, 21);
            this.comboOccurence.TabIndex = 10;
            // 
            // label33
            // 
            this.label33.AutoSize = true;
            this.label33.Location = new System.Drawing.Point(6, 22);
            this.label33.Name = "label33";
            this.label33.Size = new System.Drawing.Size(113, 13);
            this.label33.TabIndex = 1;
            this.label33.Text = "Elemental Composition";
            // 
            // txtMaxTemperature
            // 
            this.txtMaxTemperature.Location = new System.Drawing.Point(387, 50);
            this.txtMaxTemperature.Name = "txtMaxTemperature";
            this.txtMaxTemperature.Size = new System.Drawing.Size(130, 20);
            this.txtMaxTemperature.TabIndex = 7;
            this.txtMaxTemperature.TextChanged += new System.EventHandler(this.txtMaxTemperature_TextChanged);
            // 
            // txtMinTemperature
            // 
            this.txtMinTemperature.Location = new System.Drawing.Point(125, 50);
            this.txtMinTemperature.Name = "txtMinTemperature";
            this.txtMinTemperature.Size = new System.Drawing.Size(130, 20);
            this.txtMinTemperature.TabIndex = 8;
            this.txtMinTemperature.TextChanged += new System.EventHandler(this.txtMinTemperature_TextChanged);
            // 
            // txtElementalComposition
            // 
            this.txtElementalComposition.Location = new System.Drawing.Point(125, 19);
            this.txtElementalComposition.Name = "txtElementalComposition";
            this.txtElementalComposition.Size = new System.Drawing.Size(130, 20);
            this.txtElementalComposition.TabIndex = 9;
            // 
            // label29
            // 
            this.label29.AutoSize = true;
            this.label29.Location = new System.Drawing.Point(321, 21);
            this.label29.Name = "label29";
            this.label29.Size = new System.Drawing.Size(60, 13);
            this.label29.TabIndex = 3;
            this.label29.Text = "Occurence";
            // 
            // label27
            // 
            this.label27.AutoSize = true;
            this.label27.Location = new System.Drawing.Point(267, 52);
            this.label27.Name = "label27";
            this.label27.Size = new System.Drawing.Size(114, 13);
            this.label27.TabIndex = 1;
            this.label27.Text = "Maximum Temperature";
            // 
            // label28
            // 
            this.label28.AutoSize = true;
            this.label28.Location = new System.Drawing.Point(5, 53);
            this.label28.Name = "label28";
            this.label28.Size = new System.Drawing.Size(114, 13);
            this.label28.TabIndex = 2;
            this.label28.Text = "Minimum Tempterature";
            // 
            // grpNomenclature
            // 
            this.grpNomenclature.Controls.Add(this.label25);
            this.grpNomenclature.Controls.Add(this.txtName);
            this.grpNomenclature.Controls.Add(this.label30);
            this.grpNomenclature.Controls.Add(this.txtSymbol);
            this.grpNomenclature.Controls.Add(this.label31);
            this.grpNomenclature.Controls.Add(this.txtPhase);
            this.grpNomenclature.Controls.Add(this.label32);
            this.grpNomenclature.Dock = System.Windows.Forms.DockStyle.Top;
            this.grpNomenclature.Location = new System.Drawing.Point(3, 3);
            this.grpNomenclature.Name = "grpNomenclature";
            this.grpNomenclature.Size = new System.Drawing.Size(695, 49);
            this.grpNomenclature.TabIndex = 13;
            this.grpNomenclature.TabStop = false;
            this.grpNomenclature.Text = "Nomenclature";
            // 
            // label25
            // 
            this.label25.AutoSize = true;
            this.label25.Location = new System.Drawing.Point(6, 22);
            this.label25.Name = "label25";
            this.label25.Size = new System.Drawing.Size(35, 13);
            this.label25.TabIndex = 0;
            this.label25.Text = "Name";
            // 
            // txtName
            // 
            this.txtName.Location = new System.Drawing.Point(47, 19);
            this.txtName.Name = "txtName";
            this.txtName.Size = new System.Drawing.Size(100, 20);
            this.txtName.TabIndex = 12;
            // 
            // label30
            // 
            this.label30.AutoSize = true;
            this.label30.Location = new System.Drawing.Point(507, 22);
            this.label30.Name = "label30";
            this.label30.Size = new System.Drawing.Size(10, 13);
            this.label30.TabIndex = 4;
            this.label30.Text = ")";
            // 
            // txtSymbol
            // 
            this.txtSymbol.Location = new System.Drawing.Point(340, 19);
            this.txtSymbol.Name = "txtSymbol";
            this.txtSymbol.Size = new System.Drawing.Size(100, 20);
            this.txtSymbol.TabIndex = 11;
            // 
            // label31
            // 
            this.label31.AutoSize = true;
            this.label31.Location = new System.Drawing.Point(446, 22);
            this.label31.Name = "label31";
            this.label31.Size = new System.Drawing.Size(10, 13);
            this.label31.TabIndex = 5;
            this.label31.Text = "(";
            // 
            // txtPhase
            // 
            this.txtPhase.Location = new System.Drawing.Point(462, 19);
            this.txtPhase.Name = "txtPhase";
            this.txtPhase.Size = new System.Drawing.Size(39, 20);
            this.txtPhase.TabIndex = 10;
            // 
            // label32
            // 
            this.label32.AutoSize = true;
            this.label32.Location = new System.Drawing.Point(293, 22);
            this.label32.Name = "label32";
            this.label32.Size = new System.Drawing.Size(41, 13);
            this.label32.TabIndex = 6;
            this.label32.Text = "Symbol";
            // 
            // tabMisc
            // 
            this.tabMisc.Controls.Add(this.tableLayoutPanel2);
            this.tabMisc.Location = new System.Drawing.Point(4, 22);
            this.tabMisc.Name = "tabMisc";
            this.tabMisc.Padding = new System.Windows.Forms.Padding(3);
            this.tabMisc.Size = new System.Drawing.Size(944, 544);
            this.tabMisc.TabIndex = 1;
            this.tabMisc.Text = "Misc";
            this.tabMisc.UseVisualStyleBackColor = true;
            // 
            // tableLayoutPanel2
            // 
            this.tableLayoutPanel2.ColumnCount = 1;
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel2.Controls.Add(this.grpDefaults, 0, 4);
            this.tableLayoutPanel2.Controls.Add(this.grpThermo, 0, 3);
            this.tableLayoutPanel2.Controls.Add(this.grpCalculationSelection, 0, 2);
            this.tableLayoutPanel2.Controls.Add(this.grpSolution, 0, 1);
            this.tableLayoutPanel2.Controls.Add(this.grpDescription, 0, 0);
            this.tableLayoutPanel2.Location = new System.Drawing.Point(12, 10);
            this.tableLayoutPanel2.Name = "tableLayoutPanel2";
            this.tableLayoutPanel2.RowCount = 5;
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 54F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 61F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 126F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 110F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 12F));
            this.tableLayoutPanel2.Size = new System.Drawing.Size(770, 404);
            this.tableLayoutPanel2.TabIndex = 0;
            // 
            // grpDefaults
            // 
            this.grpDefaults.Controls.Add(this.chkH2OAqueous);
            this.grpDefaults.Controls.Add(this.comboSpecieModel);
            this.grpDefaults.Controls.Add(this.label22);
            this.grpDefaults.Location = new System.Drawing.Point(3, 354);
            this.grpDefaults.Name = "grpDefaults";
            this.grpDefaults.Size = new System.Drawing.Size(496, 47);
            this.grpDefaults.TabIndex = 4;
            this.grpDefaults.TabStop = false;
            this.grpDefaults.Text = "Defaults";
            // 
            // chkH2OAqueous
            // 
            this.chkH2OAqueous.AutoSize = true;
            this.chkH2OAqueous.Location = new System.Drawing.Point(321, 19);
            this.chkH2OAqueous.Name = "chkH2OAqueous";
            this.chkH2OAqueous.Size = new System.Drawing.Size(169, 17);
            this.chkH2OAqueous.TabIndex = 2;
            this.chkH2OAqueous.Text = "Recognise H2O(l) as Aqueous";
            this.chkH2OAqueous.UseVisualStyleBackColor = true;
            // 
            // comboSpecieModel
            // 
            this.comboSpecieModel.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboSpecieModel.FormattingEnabled = true;
            this.comboSpecieModel.Location = new System.Drawing.Point(104, 17);
            this.comboSpecieModel.Name = "comboSpecieModel";
            this.comboSpecieModel.Size = new System.Drawing.Size(121, 21);
            this.comboSpecieModel.TabIndex = 1;
            // 
            // label22
            // 
            this.label22.AutoSize = true;
            this.label22.Location = new System.Drawing.Point(29, 20);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(69, 13);
            this.label22.TabIndex = 0;
            this.label22.Text = "SpecieModel";
            // 
            // grpThermo
            // 
            this.grpThermo.Controls.Add(this.tableLayoutPanel4);
            this.grpThermo.Location = new System.Drawing.Point(3, 244);
            this.grpThermo.Name = "grpThermo";
            this.grpThermo.Size = new System.Drawing.Size(496, 104);
            this.grpThermo.TabIndex = 3;
            this.grpThermo.TabStop = false;
            this.grpThermo.Text = "Thermodynamic Properties";
            // 
            // tableLayoutPanel4
            // 
            this.tableLayoutPanel4.ColumnCount = 5;
            this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.tableLayoutPanel4.Controls.Add(this.txtMinP, 3, 2);
            this.tableLayoutPanel4.Controls.Add(this.txtNP, 2, 2);
            this.tableLayoutPanel4.Controls.Add(this.txtStP, 1, 2);
            this.tableLayoutPanel4.Controls.Add(this.txtMaxT, 4, 1);
            this.tableLayoutPanel4.Controls.Add(this.txtMinT, 3, 1);
            this.tableLayoutPanel4.Controls.Add(this.txtNT, 2, 1);
            this.tableLayoutPanel4.Controls.Add(this.label21, 3, 0);
            this.tableLayoutPanel4.Controls.Add(this.label20, 2, 0);
            this.tableLayoutPanel4.Controls.Add(this.label16, 0, 1);
            this.tableLayoutPanel4.Controls.Add(this.label17, 0, 2);
            this.tableLayoutPanel4.Controls.Add(this.label18, 1, 0);
            this.tableLayoutPanel4.Controls.Add(this.label19, 4, 0);
            this.tableLayoutPanel4.Controls.Add(this.txtStT, 1, 1);
            this.tableLayoutPanel4.Controls.Add(this.txtMaxP, 4, 2);
            this.tableLayoutPanel4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel4.Location = new System.Drawing.Point(3, 16);
            this.tableLayoutPanel4.Name = "tableLayoutPanel4";
            this.tableLayoutPanel4.RowCount = 3;
            this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
            this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
            this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
            this.tableLayoutPanel4.Size = new System.Drawing.Size(490, 85);
            this.tableLayoutPanel4.TabIndex = 1;
            // 
            // txtMinP
            // 
            this.txtMinP.Location = new System.Drawing.Point(297, 59);
            this.txtMinP.Name = "txtMinP";
            this.txtMinP.Size = new System.Drawing.Size(92, 20);
            this.txtMinP.TabIndex = 13;
            // 
            // txtNP
            // 
            this.txtNP.Location = new System.Drawing.Point(199, 59);
            this.txtNP.Name = "txtNP";
            this.txtNP.Size = new System.Drawing.Size(92, 20);
            this.txtNP.TabIndex = 12;
            // 
            // txtStP
            // 
            this.txtStP.Location = new System.Drawing.Point(101, 59);
            this.txtStP.Name = "txtStP";
            this.txtStP.Size = new System.Drawing.Size(92, 20);
            this.txtStP.TabIndex = 11;
            // 
            // txtMaxT
            // 
            this.txtMaxT.Location = new System.Drawing.Point(395, 31);
            this.txtMaxT.Name = "txtMaxT";
            this.txtMaxT.Size = new System.Drawing.Size(92, 20);
            this.txtMaxT.TabIndex = 10;
            // 
            // txtMinT
            // 
            this.txtMinT.Location = new System.Drawing.Point(297, 31);
            this.txtMinT.Name = "txtMinT";
            this.txtMinT.Size = new System.Drawing.Size(92, 20);
            this.txtMinT.TabIndex = 9;
            // 
            // txtNT
            // 
            this.txtNT.Location = new System.Drawing.Point(199, 31);
            this.txtNT.Name = "txtNT";
            this.txtNT.Size = new System.Drawing.Size(92, 20);
            this.txtNT.TabIndex = 8;
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label21.Location = new System.Drawing.Point(297, 0);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(92, 28);
            this.label21.TabIndex = 5;
            this.label21.Text = "Minimum";
            this.label21.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label20.Location = new System.Drawing.Point(199, 0);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(92, 28);
            this.label20.TabIndex = 4;
            this.label20.Text = "Normal";
            this.label20.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label16.Location = new System.Drawing.Point(3, 28);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(92, 28);
            this.label16.TabIndex = 0;
            this.label16.Text = "Temp (C)";
            this.label16.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label17.Location = new System.Drawing.Point(3, 56);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(92, 29);
            this.label17.TabIndex = 1;
            this.label17.Text = "Pressure (kPa)";
            this.label17.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label18.Location = new System.Drawing.Point(101, 0);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(92, 28);
            this.label18.TabIndex = 2;
            this.label18.Text = "Standard";
            this.label18.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label19.Location = new System.Drawing.Point(395, 0);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(92, 28);
            this.label19.TabIndex = 3;
            this.label19.Text = "Maximum";
            this.label19.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            // 
            // txtStT
            // 
            this.txtStT.Location = new System.Drawing.Point(101, 31);
            this.txtStT.Name = "txtStT";
            this.txtStT.Size = new System.Drawing.Size(92, 20);
            this.txtStT.TabIndex = 6;
            // 
            // txtMaxP
            // 
            this.txtMaxP.Location = new System.Drawing.Point(395, 59);
            this.txtMaxP.Name = "txtMaxP";
            this.txtMaxP.Size = new System.Drawing.Size(92, 20);
            this.txtMaxP.TabIndex = 7;
            // 
            // grpCalculationSelection
            // 
            this.grpCalculationSelection.Controls.Add(this.tableLayoutPanel3);
            this.grpCalculationSelection.Location = new System.Drawing.Point(3, 118);
            this.grpCalculationSelection.Name = "grpCalculationSelection";
            this.grpCalculationSelection.Size = new System.Drawing.Size(496, 120);
            this.grpCalculationSelection.TabIndex = 2;
            this.grpCalculationSelection.TabStop = false;
            this.grpCalculationSelection.Text = "Calculation Selection";
            // 
            // tableLayoutPanel3
            // 
            this.tableLayoutPanel3.ColumnCount = 4;
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel3.Controls.Add(this.comboDynamicHeat, 2, 3);
            this.tableLayoutPanel3.Controls.Add(this.comboProBalHeat, 1, 3);
            this.tableLayoutPanel3.Controls.Add(this.comboMaxCompFlow, 3, 2);
            this.tableLayoutPanel3.Controls.Add(this.comboProbalFlow, 1, 2);
            this.tableLayoutPanel3.Controls.Add(this.comboMaxCompSurge, 3, 1);
            this.tableLayoutPanel3.Controls.Add(this.comboDynamicSurge, 2, 1);
            this.tableLayoutPanel3.Controls.Add(this.comboProBalSurge, 1, 1);
            this.tableLayoutPanel3.Controls.Add(this.label14, 0, 1);
            this.tableLayoutPanel3.Controls.Add(this.label13, 3, 0);
            this.tableLayoutPanel3.Controls.Add(this.label12, 2, 0);
            this.tableLayoutPanel3.Controls.Add(this.label10, 1, 0);
            this.tableLayoutPanel3.Controls.Add(this.label15, 0, 2);
            this.tableLayoutPanel3.Controls.Add(this.label11, 0, 3);
            this.tableLayoutPanel3.Controls.Add(this.comboDynamicFlow, 2, 2);
            this.tableLayoutPanel3.Controls.Add(this.comboMaxCompHeat, 3, 3);
            this.tableLayoutPanel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel3.Location = new System.Drawing.Point(3, 16);
            this.tableLayoutPanel3.Name = "tableLayoutPanel3";
            this.tableLayoutPanel3.RowCount = 4;
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel3.Size = new System.Drawing.Size(490, 101);
            this.tableLayoutPanel3.TabIndex = 0;
            // 
            // comboDynamicHeat
            // 
            this.comboDynamicHeat.Dock = System.Windows.Forms.DockStyle.Fill;
            this.comboDynamicHeat.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboDynamicHeat.FormattingEnabled = true;
            this.comboDynamicHeat.Location = new System.Drawing.Point(247, 78);
            this.comboDynamicHeat.Name = "comboDynamicHeat";
            this.comboDynamicHeat.Size = new System.Drawing.Size(116, 21);
            this.comboDynamicHeat.TabIndex = 14;
            // 
            // comboProBalHeat
            // 
            this.comboProBalHeat.Dock = System.Windows.Forms.DockStyle.Fill;
            this.comboProBalHeat.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboProBalHeat.FormattingEnabled = true;
            this.comboProBalHeat.Location = new System.Drawing.Point(125, 78);
            this.comboProBalHeat.Name = "comboProBalHeat";
            this.comboProBalHeat.Size = new System.Drawing.Size(116, 21);
            this.comboProBalHeat.TabIndex = 13;
            // 
            // comboMaxCompFlow
            // 
            this.comboMaxCompFlow.Dock = System.Windows.Forms.DockStyle.Fill;
            this.comboMaxCompFlow.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboMaxCompFlow.FormattingEnabled = true;
            this.comboMaxCompFlow.Location = new System.Drawing.Point(369, 53);
            this.comboMaxCompFlow.Name = "comboMaxCompFlow";
            this.comboMaxCompFlow.Size = new System.Drawing.Size(118, 21);
            this.comboMaxCompFlow.TabIndex = 12;
            // 
            // comboProbalFlow
            // 
            this.comboProbalFlow.Dock = System.Windows.Forms.DockStyle.Fill;
            this.comboProbalFlow.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboProbalFlow.Enabled = false;
            this.comboProbalFlow.FormattingEnabled = true;
            this.comboProbalFlow.Location = new System.Drawing.Point(125, 53);
            this.comboProbalFlow.Name = "comboProbalFlow";
            this.comboProbalFlow.Size = new System.Drawing.Size(116, 21);
            this.comboProbalFlow.TabIndex = 11;
            // 
            // comboMaxCompSurge
            // 
            this.comboMaxCompSurge.Dock = System.Windows.Forms.DockStyle.Fill;
            this.comboMaxCompSurge.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboMaxCompSurge.Enabled = false;
            this.comboMaxCompSurge.FormattingEnabled = true;
            this.comboMaxCompSurge.Location = new System.Drawing.Point(369, 28);
            this.comboMaxCompSurge.Name = "comboMaxCompSurge";
            this.comboMaxCompSurge.Size = new System.Drawing.Size(118, 21);
            this.comboMaxCompSurge.TabIndex = 10;
            // 
            // comboDynamicSurge
            // 
            this.comboDynamicSurge.Dock = System.Windows.Forms.DockStyle.Fill;
            this.comboDynamicSurge.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboDynamicSurge.FormattingEnabled = true;
            this.comboDynamicSurge.Location = new System.Drawing.Point(247, 28);
            this.comboDynamicSurge.Name = "comboDynamicSurge";
            this.comboDynamicSurge.Size = new System.Drawing.Size(116, 21);
            this.comboDynamicSurge.TabIndex = 9;
            // 
            // comboProBalSurge
            // 
            this.comboProBalSurge.Dock = System.Windows.Forms.DockStyle.Fill;
            this.comboProBalSurge.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboProBalSurge.Enabled = false;
            this.comboProBalSurge.FormattingEnabled = true;
            this.comboProBalSurge.Location = new System.Drawing.Point(125, 28);
            this.comboProBalSurge.Name = "comboProBalSurge";
            this.comboProBalSurge.Size = new System.Drawing.Size(116, 21);
            this.comboProBalSurge.TabIndex = 8;
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label14.Location = new System.Drawing.Point(3, 25);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(116, 25);
            this.label14.TabIndex = 4;
            this.label14.Text = "Surge Solution";
            this.label14.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label13.Location = new System.Drawing.Point(369, 0);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(118, 25);
            this.label13.TabIndex = 3;
            this.label13.Text = "Maximum Complexity";
            this.label13.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label12.Location = new System.Drawing.Point(247, 0);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(116, 25);
            this.label12.TabIndex = 2;
            this.label12.Text = "Dynamic";
            this.label12.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label10.Location = new System.Drawing.Point(125, 0);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(116, 25);
            this.label10.TabIndex = 0;
            this.label10.Text = "ProBal";
            this.label10.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label15.Location = new System.Drawing.Point(3, 50);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(116, 25);
            this.label15.TabIndex = 5;
            this.label15.Text = "Flow Calculations";
            this.label15.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label11.Location = new System.Drawing.Point(3, 75);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(116, 26);
            this.label11.TabIndex = 1;
            this.label11.Text = "Heat Calculations";
            this.label11.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // comboDynamicFlow
            // 
            this.comboDynamicFlow.Dock = System.Windows.Forms.DockStyle.Fill;
            this.comboDynamicFlow.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboDynamicFlow.FormattingEnabled = true;
            this.comboDynamicFlow.Location = new System.Drawing.Point(247, 53);
            this.comboDynamicFlow.Name = "comboDynamicFlow";
            this.comboDynamicFlow.Size = new System.Drawing.Size(116, 21);
            this.comboDynamicFlow.TabIndex = 6;
            // 
            // comboMaxCompHeat
            // 
            this.comboMaxCompHeat.Dock = System.Windows.Forms.DockStyle.Fill;
            this.comboMaxCompHeat.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboMaxCompHeat.FormattingEnabled = true;
            this.comboMaxCompHeat.Location = new System.Drawing.Point(369, 78);
            this.comboMaxCompHeat.Name = "comboMaxCompHeat";
            this.comboMaxCompHeat.Size = new System.Drawing.Size(118, 21);
            this.comboMaxCompHeat.TabIndex = 7;
            // 
            // grpSolution
            // 
            this.grpSolution.Controls.Add(this.tableLayoutPanel5);
            this.grpSolution.Location = new System.Drawing.Point(3, 57);
            this.grpSolution.Name = "grpSolution";
            this.grpSolution.Size = new System.Drawing.Size(496, 55);
            this.grpSolution.TabIndex = 1;
            this.grpSolution.TabStop = false;
            this.grpSolution.Text = "Solution";
            // 
            // tableLayoutPanel5
            // 
            this.tableLayoutPanel5.ColumnCount = 4;
            this.tableLayoutPanel5.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel5.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel5.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel5.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel5.Controls.Add(this.chkDynamicAllowed, 2, 0);
            this.tableLayoutPanel5.Controls.Add(this.label9, 0, 0);
            this.tableLayoutPanel5.Controls.Add(this.chkProbalAllowed, 3, 0);
            this.tableLayoutPanel5.Controls.Add(this.comboDefaultSolution, 1, 0);
            this.tableLayoutPanel5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel5.Location = new System.Drawing.Point(3, 16);
            this.tableLayoutPanel5.Name = "tableLayoutPanel5";
            this.tableLayoutPanel5.Padding = new System.Windows.Forms.Padding(0, 5, 0, 5);
            this.tableLayoutPanel5.RowCount = 1;
            this.tableLayoutPanel5.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel5.Size = new System.Drawing.Size(490, 36);
            this.tableLayoutPanel5.TabIndex = 0;
            // 
            // chkDynamicAllowed
            // 
            this.chkDynamicAllowed.AutoSize = true;
            this.chkDynamicAllowed.Checked = true;
            this.chkDynamicAllowed.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkDynamicAllowed.Dock = System.Windows.Forms.DockStyle.Fill;
            this.chkDynamicAllowed.Location = new System.Drawing.Point(247, 8);
            this.chkDynamicAllowed.Name = "chkDynamicAllowed";
            this.chkDynamicAllowed.Size = new System.Drawing.Size(116, 20);
            this.chkDynamicAllowed.TabIndex = 2;
            this.chkDynamicAllowed.Text = "Dynamic Allowed";
            this.chkDynamicAllowed.UseVisualStyleBackColor = true;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label9.Location = new System.Drawing.Point(3, 5);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(116, 26);
            this.label9.TabIndex = 0;
            this.label9.Text = "Default Solution";
            this.label9.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // chkProbalAllowed
            // 
            this.chkProbalAllowed.AutoSize = true;
            this.chkProbalAllowed.Checked = true;
            this.chkProbalAllowed.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkProbalAllowed.Dock = System.Windows.Forms.DockStyle.Fill;
            this.chkProbalAllowed.Location = new System.Drawing.Point(369, 8);
            this.chkProbalAllowed.Name = "chkProbalAllowed";
            this.chkProbalAllowed.Size = new System.Drawing.Size(118, 20);
            this.chkProbalAllowed.TabIndex = 1;
            this.chkProbalAllowed.Text = "Probal Allowed";
            this.chkProbalAllowed.UseVisualStyleBackColor = true;
            // 
            // comboDefaultSolution
            // 
            this.comboDefaultSolution.Dock = System.Windows.Forms.DockStyle.Fill;
            this.comboDefaultSolution.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboDefaultSolution.FormattingEnabled = true;
            this.comboDefaultSolution.Location = new System.Drawing.Point(125, 8);
            this.comboDefaultSolution.Name = "comboDefaultSolution";
            this.comboDefaultSolution.Size = new System.Drawing.Size(116, 21);
            this.comboDefaultSolution.TabIndex = 3;
            // 
            // grpDescription
            // 
            this.grpDescription.Controls.Add(this.txtDescription);
            this.grpDescription.Location = new System.Drawing.Point(3, 3);
            this.grpDescription.Name = "grpDescription";
            this.grpDescription.Size = new System.Drawing.Size(496, 48);
            this.grpDescription.TabIndex = 0;
            this.grpDescription.TabStop = false;
            this.grpDescription.Text = "Description";
            // 
            // txtDescription
            // 
            this.txtDescription.Location = new System.Drawing.Point(9, 19);
            this.txtDescription.Name = "txtDescription";
            this.txtDescription.Size = new System.Drawing.Size(481, 20);
            this.txtDescription.TabIndex = 0;
            // 
            // tabModels
            // 
            this.tabModels.Controls.Add(this.scModels);
            this.tabModels.Location = new System.Drawing.Point(4, 22);
            this.tabModels.Name = "tabModels";
            this.tabModels.Padding = new System.Windows.Forms.Padding(3);
            this.tabModels.Size = new System.Drawing.Size(944, 544);
            this.tabModels.TabIndex = 2;
            this.tabModels.Text = "Models";
            this.tabModels.UseVisualStyleBackColor = true;
            // 
            // scModels
            // 
            this.scModels.Dock = System.Windows.Forms.DockStyle.Fill;
            this.scModels.Location = new System.Drawing.Point(3, 3);
            this.scModels.Name = "scModels";
            // 
            // scModels.Panel1
            // 
            this.scModels.Panel1.Controls.Add(this.grpModels);
            // 
            // scModels.Panel2
            // 
            this.scModels.Panel2.Controls.Add(this.groupBox3);
            this.scModels.Size = new System.Drawing.Size(938, 538);
            this.scModels.SplitterDistance = 608;
            this.scModels.TabIndex = 4;
            // 
            // grpModels
            // 
            this.grpModels.Controls.Add(this.treeModels);
            this.grpModels.Controls.Add(this.pnlModelOptions);
            this.grpModels.Dock = System.Windows.Forms.DockStyle.Fill;
            this.grpModels.Location = new System.Drawing.Point(0, 0);
            this.grpModels.Name = "grpModels";
            this.grpModels.Size = new System.Drawing.Size(608, 538);
            this.grpModels.TabIndex = 2;
            this.grpModels.TabStop = false;
            this.grpModels.Text = "Models";
            // 
            // treeModels
            // 
            this.treeModels.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeModels.Location = new System.Drawing.Point(3, 42);
            this.treeModels.Name = "treeModels";
            this.treeModels.Size = new System.Drawing.Size(602, 493);
            this.treeModels.TabIndex = 0;
            // 
            // pnlModelOptions
            // 
            this.pnlModelOptions.Controls.Add(this.radioSortByDLL);
            this.pnlModelOptions.Controls.Add(this.radioSortByGroup);
            this.pnlModelOptions.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnlModelOptions.Location = new System.Drawing.Point(3, 16);
            this.pnlModelOptions.Name = "pnlModelOptions";
            this.pnlModelOptions.Size = new System.Drawing.Size(602, 26);
            this.pnlModelOptions.TabIndex = 3;
            // 
            // radioSortByDLL
            // 
            this.radioSortByDLL.AutoSize = true;
            this.radioSortByDLL.Location = new System.Drawing.Point(100, 3);
            this.radioSortByDLL.Name = "radioSortByDLL";
            this.radioSortByDLL.Size = new System.Drawing.Size(82, 17);
            this.radioSortByDLL.TabIndex = 2;
            this.radioSortByDLL.Text = "Sort By DLL";
            this.radioSortByDLL.UseVisualStyleBackColor = true;
            // 
            // radioSortByGroup
            // 
            this.radioSortByGroup.AutoSize = true;
            this.radioSortByGroup.Checked = true;
            this.radioSortByGroup.Location = new System.Drawing.Point(3, 3);
            this.radioSortByGroup.Name = "radioSortByGroup";
            this.radioSortByGroup.Size = new System.Drawing.Size(91, 17);
            this.radioSortByGroup.TabIndex = 1;
            this.radioSortByGroup.TabStop = true;
            this.radioSortByGroup.Text = "Sort By Group";
            this.radioSortByGroup.UseVisualStyleBackColor = true;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.treeReqSpecies);
            this.groupBox3.Controls.Add(this.pnlReqSpecieOptions);
            this.groupBox3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox3.Location = new System.Drawing.Point(0, 0);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(326, 538);
            this.groupBox3.TabIndex = 3;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "RequiredSpecies";
            // 
            // treeReqSpecies
            // 
            this.treeReqSpecies.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeReqSpecies.Location = new System.Drawing.Point(3, 42);
            this.treeReqSpecies.Name = "treeReqSpecies";
            this.treeReqSpecies.Size = new System.Drawing.Size(320, 493);
            this.treeReqSpecies.TabIndex = 1;
            // 
            // pnlReqSpecieOptions
            // 
            this.pnlReqSpecieOptions.Controls.Add(this.radioSpecieSortBySpecie);
            this.pnlReqSpecieOptions.Controls.Add(this.radioSpecieSortByDLL);
            this.pnlReqSpecieOptions.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnlReqSpecieOptions.Location = new System.Drawing.Point(3, 16);
            this.pnlReqSpecieOptions.Name = "pnlReqSpecieOptions";
            this.pnlReqSpecieOptions.Size = new System.Drawing.Size(320, 26);
            this.pnlReqSpecieOptions.TabIndex = 3;
            // 
            // radioSpecieSortBySpecie
            // 
            this.radioSpecieSortBySpecie.AutoSize = true;
            this.radioSpecieSortBySpecie.Enabled = false;
            this.radioSpecieSortBySpecie.Location = new System.Drawing.Point(91, 3);
            this.radioSpecieSortBySpecie.Name = "radioSpecieSortBySpecie";
            this.radioSpecieSortBySpecie.Size = new System.Drawing.Size(95, 17);
            this.radioSpecieSortBySpecie.TabIndex = 3;
            this.radioSpecieSortBySpecie.Text = "Sort By Specie";
            this.radioSpecieSortBySpecie.UseVisualStyleBackColor = true;
            // 
            // radioSpecieSortByDLL
            // 
            this.radioSpecieSortByDLL.AutoSize = true;
            this.radioSpecieSortByDLL.Checked = true;
            this.radioSpecieSortByDLL.Location = new System.Drawing.Point(3, 3);
            this.radioSpecieSortByDLL.Name = "radioSpecieSortByDLL";
            this.radioSpecieSortByDLL.Size = new System.Drawing.Size(82, 17);
            this.radioSpecieSortByDLL.TabIndex = 2;
            this.radioSpecieSortByDLL.TabStop = true;
            this.radioSpecieSortByDLL.Text = "Sort By DLL";
            this.radioSpecieSortByDLL.UseVisualStyleBackColor = true;
            // 
            // dlgOpenDB
            // 
            this.dlgOpenDB.Filter = "Access Databases|*.mdb|All files|*.*";
            this.dlgOpenDB.Title = "Open Database";
            this.dlgOpenDB.FileOk += new System.ComponentModel.CancelEventHandler(this.dlgOpenDB_FileOk);
            // 
            // dlgOpenConfig
            // 
            this.dlgOpenConfig.Filter = "Configuration files|*.cfg|All files|*.*";
            this.dlgOpenConfig.Title = "Open Configuration File";
            this.dlgOpenConfig.FileOk += new System.ComponentModel.CancelEventHandler(this.dlgOpenDB_FileOk);
            // 
            // dlgSaveConfig
            // 
            this.dlgSaveConfig.Filter = "Configuration files|*.cfg|All files|*.*";
            this.dlgSaveConfig.Title = "Save As";
            this.dlgSaveConfig.FileOk += new System.ComponentModel.CancelEventHandler(this.dlgOpenDB_FileOk);
            // 
            // FrmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(952, 616);
            this.Controls.Add(this.tcMain);
            this.Controls.Add(this.ssMain);
            this.Controls.Add(this.menuMain);
            this.MainMenuStrip = this.menuMain;
            this.Name = "FrmMain";
            this.Text = "SysCAD Configuration Editor";
            this.menuMain.ResumeLayout(false);
            this.menuMain.PerformLayout();
            this.tcMain.ResumeLayout(false);
            this.tabSpecies.ResumeLayout(false);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.grpAvailableSpecies.ResumeLayout(false);
            this.menuSpDBContext.ResumeLayout(false);
            this.pnlFilter.ResumeLayout(false);
            this.pnlFilter.PerformLayout();
            this.tcSpecies.ResumeLayout(false);
            this.tabProjectSpecies.ResumeLayout(false);
            this.grpProjectList.ResumeLayout(false);
            this.pnlActiveList.ResumeLayout(false);
            this.pnlListButtons.ResumeLayout(false);
            this.pnlActive.ResumeLayout(false);
            this.tcIDE.ResumeLayout(false);
            this.tabIDESpecie.ResumeLayout(false);
            this.grpSpecie.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numLowerTempTolerance)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numGreaterTempTolerance)).EndInit();
            this.tabIDECalculation.ResumeLayout(false);
            this.grpCalculation.ResumeLayout(false);
            this.grpCalculation.PerformLayout();
            this.tabIDELabel.ResumeLayout(false);
            this.grpTextValue.ResumeLayout(false);
            this.grpTextValue.PerformLayout();
            this.tabIDEBlank.ResumeLayout(false);
            this.tabIDEAttribute.ResumeLayout(false);
            this.grpAttribute.ResumeLayout(false);
            this.grpAttribute.PerformLayout();
            this.tabSpecieDatabase.ResumeLayout(false);
            this.groupBox6.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            this.splitContainer2.ResumeLayout(false);
            this.tlpEquations.ResumeLayout(false);
            this.tlpEquations.PerformLayout();
            this.pnlTempDependantRadios.ResumeLayout(false);
            this.pnlTempDependantRadios.PerformLayout();
            this.groupBox8.ResumeLayout(false);
            this.groupBox8.PerformLayout();
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.grpNomenclature.ResumeLayout(false);
            this.grpNomenclature.PerformLayout();
            this.tabMisc.ResumeLayout(false);
            this.tableLayoutPanel2.ResumeLayout(false);
            this.grpDefaults.ResumeLayout(false);
            this.grpDefaults.PerformLayout();
            this.grpThermo.ResumeLayout(false);
            this.tableLayoutPanel4.ResumeLayout(false);
            this.tableLayoutPanel4.PerformLayout();
            this.grpCalculationSelection.ResumeLayout(false);
            this.tableLayoutPanel3.ResumeLayout(false);
            this.tableLayoutPanel3.PerformLayout();
            this.grpSolution.ResumeLayout(false);
            this.tableLayoutPanel5.ResumeLayout(false);
            this.tableLayoutPanel5.PerformLayout();
            this.grpDescription.ResumeLayout(false);
            this.grpDescription.PerformLayout();
            this.tabModels.ResumeLayout(false);
            this.scModels.Panel1.ResumeLayout(false);
            this.scModels.Panel2.ResumeLayout(false);
            this.scModels.ResumeLayout(false);
            this.grpModels.ResumeLayout(false);
            this.pnlModelOptions.ResumeLayout(false);
            this.pnlModelOptions.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.pnlReqSpecieOptions.ResumeLayout(false);
            this.pnlReqSpecieOptions.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip ssMain;
        private System.Windows.Forms.MenuStrip menuMain;
        private System.Windows.Forms.TabControl tcMain;
        private System.Windows.Forms.TabPage tabSpecies;
        private System.Windows.Forms.TabPage tabMisc;
        private System.Windows.Forms.GroupBox grpAvailableSpecies;
        private System.Windows.Forms.ListView lstDBSpecies;
        private System.Windows.Forms.Panel pnlFilter;
        private System.Windows.Forms.TextBox txtFilter;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox grpProjectList;
        private System.Windows.Forms.Panel pnlListButtons;
        private System.Windows.Forms.TabControl tcIDE;
        private System.Windows.Forms.TabPage tabIDESpecie;
        private System.Windows.Forms.TabPage tabIDECalculation;
        private System.Windows.Forms.ListView lstProjectVector;
        private System.Windows.Forms.Button btnMoveUp;
        private System.Windows.Forms.Button btnMoveDown;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.TextBox txtDefinedMinTemp;
        private System.Windows.Forms.TextBox txtDefinedMaxTemp;
        private System.Windows.Forms.TextBox txtAllowedMaxTemp;
        private System.Windows.Forms.TextBox txtAllowedMinTemp;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown numLowerTempTolerance;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.NumericUpDown numGreaterTempTolerance;
        private System.Windows.Forms.CheckBox chkIdeal;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem closeToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveAsToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem menuOpenDatabase;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem3;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.GroupBox grpSpecie;
        private Be.Windows.Forms.RichTextBoxEx txtCalculation;
        private System.Windows.Forms.GroupBox grpCalculation;
        private System.Windows.Forms.TabPage tabIDELabel;
        private System.Windows.Forms.GroupBox grpTextValue;
        private System.Windows.Forms.TextBox txtText;
        private System.Windows.Forms.TabPage tabModels;
        private System.Windows.Forms.Button btnAdd;
        private System.Windows.Forms.Button btnRemove;
        private System.Windows.Forms.OpenFileDialog dlgOpenDB;
        private System.Windows.Forms.ColumnHeader chSymbol;
        private System.Windows.Forms.ColumnHeader chName;
        private System.Windows.Forms.ColumnHeader chProjSymbol;
        private System.Windows.Forms.ColumnHeader chProjMinTemp;
        private System.Windows.Forms.ColumnHeader chProjMaxTemp;
        private System.Windows.Forms.TabPage tabIDEBlank;
        private System.Windows.Forms.Panel pnlBlank;
        private System.Windows.Forms.Panel pnlActive;
        private System.Windows.Forms.TabPage tabIDEAttribute;
        private System.Windows.Forms.GroupBox grpAttribute;
        private System.Windows.Forms.ComboBox comboAttributeType;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox txtAttributeName;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem MenuSort;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
        private System.Windows.Forms.GroupBox grpDefaults;
        private System.Windows.Forms.GroupBox grpThermo;
        private System.Windows.Forms.GroupBox grpCalculationSelection;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel3;
        private System.Windows.Forms.GroupBox grpSolution;
        private System.Windows.Forms.GroupBox grpDescription;
        private System.Windows.Forms.TextBox txtDescription;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel4;
        private System.Windows.Forms.ComboBox comboDynamicHeat;
        private System.Windows.Forms.ComboBox comboProBalHeat;
        private System.Windows.Forms.ComboBox comboMaxCompFlow;
        private System.Windows.Forms.ComboBox comboProbalFlow;
        private System.Windows.Forms.ComboBox comboMaxCompSurge;
        private System.Windows.Forms.ComboBox comboDynamicSurge;
        private System.Windows.Forms.ComboBox comboProBalSurge;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.ComboBox comboDynamicFlow;
        private System.Windows.Forms.ComboBox comboMaxCompHeat;
        private System.Windows.Forms.TextBox txtMinP;
        private System.Windows.Forms.TextBox txtNP;
        private System.Windows.Forms.TextBox txtStP;
        private System.Windows.Forms.TextBox txtMaxT;
        private System.Windows.Forms.TextBox txtMinT;
        private System.Windows.Forms.TextBox txtNT;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.TextBox txtStT;
        private System.Windows.Forms.TextBox txtMaxP;
        private System.Windows.Forms.CheckBox chkH2OAqueous;
        private System.Windows.Forms.ComboBox comboSpecieModel;
        private System.Windows.Forms.Label label22;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel5;
        private System.Windows.Forms.CheckBox chkDynamicAllowed;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.CheckBox chkProbalAllowed;
        private System.Windows.Forms.ComboBox comboDefaultSolution;
        private System.Windows.Forms.ToolStripMenuItem menuAdvancedSort;
        private System.Windows.Forms.Label label24;
        private System.Windows.Forms.TextBox txtCalcName;
        private System.Windows.Forms.Label label23;
        private System.Windows.Forms.Panel pnlActiveList;
        private System.Windows.Forms.TreeView treeModels;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.RadioButton radioSpecieSortByDLL;
        private System.Windows.Forms.TreeView treeReqSpecies;
        private System.Windows.Forms.GroupBox grpModels;
        private System.Windows.Forms.RadioButton radioSortByDLL;
        private System.Windows.Forms.RadioButton radioSortByGroup;
        private System.Windows.Forms.SplitContainer scModels;
        private System.Windows.Forms.Panel pnlModelOptions;
        private System.Windows.Forms.Panel pnlReqSpecieOptions;
        private System.Windows.Forms.RadioButton radioSpecieSortBySpecie;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TabControl tcSpecies;
        private System.Windows.Forms.TabPage tabProjectSpecies;
        private System.Windows.Forms.TabPage tabSpecieDatabase;
        private System.Windows.Forms.TextBox txtCalcSymbol;
        private System.Windows.Forms.Label label26;
        private System.Windows.Forms.TextBox txtName;
        private System.Windows.Forms.TextBox txtSymbol;
        private System.Windows.Forms.TextBox txtPhase;
        private System.Windows.Forms.TextBox txtElementalComposition;
        private System.Windows.Forms.TextBox txtMinTemperature;
        private System.Windows.Forms.TextBox txtMaxTemperature;
        private System.Windows.Forms.Label label32;
        private System.Windows.Forms.Label label31;
        private System.Windows.Forms.Label label30;
        private System.Windows.Forms.Label label29;
        private System.Windows.Forms.Label label28;
        private System.Windows.Forms.Label label27;
        private System.Windows.Forms.Label label25;
        private System.Windows.Forms.GroupBox grpNomenclature;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.ComboBox comboOccurence;
        private System.Windows.Forms.Label label33;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.TextBox txtCheckedBy;
        private System.Windows.Forms.Label label35;
        private System.Windows.Forms.Label label34;
        private System.Windows.Forms.TextBox txtSource;
        private System.Windows.Forms.GroupBox groupBox8;
        private System.Windows.Forms.Label label47;
        private System.Windows.Forms.TextBox txtCritVol;
        private System.Windows.Forms.Label label46;
        private System.Windows.Forms.TextBox txtCritPressure;
        private System.Windows.Forms.Label label43;
        private System.Windows.Forms.Label label44;
        private System.Windows.Forms.TextBox txtAccentricity;
        private System.Windows.Forms.TextBox txtCritTemp;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.RadioButton radioDensity;
        private System.Windows.Forms.Label label37;
        private System.Windows.Forms.TextBox txtSolvent;
        private System.Windows.Forms.Label label36;
        private System.Windows.Forms.Label label39;
        private System.Windows.Forms.Label label38;
        private Graph graph1;
        private System.Windows.Forms.RadioButton radioEntropy;
        private System.Windows.Forms.RadioButton radioHeatCapacity;
        private System.Windows.Forms.RadioButton radioVapourPressure;
        private System.Windows.Forms.RadioButton radioHeatOfFormation;
        private System.Windows.Forms.TableLayoutPanel tlpEquations;
        private System.Windows.Forms.TextBox txtMaxTemp3;
        private System.Windows.Forms.TextBox txtMinTemp3;
        private System.Windows.Forms.TextBox txtMaxTemp2;
        private System.Windows.Forms.TextBox txtMinTemp2;
        private System.Windows.Forms.TextBox txtMaxTemp1;
        private System.Windows.Forms.TextBox txtMinTemp1;
        private System.Windows.Forms.TextBox txtMaxTemp0;
        private System.Windows.Forms.TextBox txtMinTemp0;
        private System.Windows.Forms.Label label40;
        private System.Windows.Forms.Label label45;
        private System.Windows.Forms.Label label41;
        private System.Windows.Forms.Label label42;
        private System.Windows.Forms.TextBox txtFormula0;
        private System.Windows.Forms.TextBox txtFormula1;
        private System.Windows.Forms.TextBox txtFormula2;
        private System.Windows.Forms.TextBox txtFormula3;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.Panel pnlTempDependantRadios;
        private System.Windows.Forms.OpenFileDialog dlgOpenConfig;
        private System.Windows.Forms.SaveFileDialog dlgSaveConfig;
        private System.Windows.Forms.ContextMenuStrip menuSpDBContext;
        private System.Windows.Forms.ToolStripMenuItem menuSortSpDBAlph;
        private System.Windows.Forms.ToolStripMenuItem menuSortSpDBPhase;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem4;
        private System.Windows.Forms.ToolStripMenuItem menuSpDBAddToProject;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aboutSysCADConfigurationEditorToolStripMenuItem;
    }
}

