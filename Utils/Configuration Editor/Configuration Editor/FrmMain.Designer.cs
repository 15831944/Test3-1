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
            Configuration_Editor.PVIOrderer pviOrderer1 = new Configuration_Editor.PVIOrderer();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FrmMain));
            this.ssMain = new System.Windows.Forms.StatusStrip();
            this.statusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.menuMain = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.MenuNew = new System.Windows.Forms.ToolStripMenuItem();
            this.menuOpenConfig = new System.Windows.Forms.ToolStripMenuItem();
            this.menuClose = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.menuSave = new System.Windows.Forms.ToolStripMenuItem();
            this.menuSaveAs = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
            this.menuOpenDatabase = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
            this.menuExit = new System.Windows.Forms.ToolStripMenuItem();
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
            this.toolStripMenuItem5 = new System.Windows.Forms.ToolStripSeparator();
            this.menuDelete = new System.Windows.Forms.ToolStripMenuItem();
            this.menuNewSpecies = new System.Windows.Forms.ToolStripMenuItem();
            this.pnlFilter = new System.Windows.Forms.Panel();
            this.txtFilter = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tcSpecies = new System.Windows.Forms.TabControl();
            this.tabSpecieDatabase = new System.Windows.Forms.TabPage();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
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
            this.tlpTempDependantRadios = new System.Windows.Forms.TableLayoutPanel();
            this.label37 = new System.Windows.Forms.Label();
            this.groupBox8 = new System.Windows.Forms.GroupBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label51 = new System.Windows.Forms.Label();
            this.txtPhaseChange = new System.Windows.Forms.TextBox();
            this.label48 = new System.Windows.Forms.Label();
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
            this.txtDensityCorrection = new System.Windows.Forms.GroupBox();
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
            this.comboPhase = new System.Windows.Forms.ComboBox();
            this.label25 = new System.Windows.Forms.Label();
            this.txtName = new System.Windows.Forms.TextBox();
            this.label30 = new System.Windows.Forms.Label();
            this.txtSymbol = new System.Windows.Forms.TextBox();
            this.label31 = new System.Windows.Forms.Label();
            this.label32 = new System.Windows.Forms.Label();
            this.tabProjectSpeciesTest = new System.Windows.Forms.TabPage();
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
            this.errorProvider1 = new System.Windows.Forms.ErrorProvider(this.components);
            this.tabDatabaseTest = new System.Windows.Forms.TabPage();
            this.graph1 = new Configuration_Editor.Graph();
            this.projectVectorControl1 = new Configuration_Editor.ProjectVectorControl();
            this.specieDatabaseControl1 = new Configuration_Editor.SpecieDatabaseControl();
            this.ssMain.SuspendLayout();
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
            this.tabSpecieDatabase.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.tlpEquations.SuspendLayout();
            this.pnlTempDependantRadios.SuspendLayout();
            this.groupBox8.SuspendLayout();
            this.txtDensityCorrection.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.grpNomenclature.SuspendLayout();
            this.tabProjectSpeciesTest.SuspendLayout();
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
            ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).BeginInit();
            this.tabDatabaseTest.SuspendLayout();
            this.SuspendLayout();
            // 
            // ssMain
            // 
            this.ssMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.statusLabel});
            this.ssMain.Location = new System.Drawing.Point(0, 594);
            this.ssMain.Name = "ssMain";
            this.ssMain.Size = new System.Drawing.Size(952, 22);
            this.ssMain.TabIndex = 0;
            this.ssMain.Text = "statusStrip1";
            // 
            // statusLabel
            // 
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(0, 17);
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
            this.MenuNew,
            this.menuOpenConfig,
            this.menuClose,
            this.toolStripMenuItem1,
            this.menuSave,
            this.menuSaveAs,
            this.toolStripMenuItem2,
            this.menuOpenDatabase,
            this.toolStripMenuItem3,
            this.menuExit});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "&File";
            this.fileToolStripMenuItem.DropDownOpening += new System.EventHandler(this.fileToolStripMenuItem_DropDownOpening);
            // 
            // MenuNew
            // 
            this.MenuNew.Name = "MenuNew";
            this.MenuNew.Size = new System.Drawing.Size(179, 22);
            this.MenuNew.Text = "&New";
            this.MenuNew.Click += new System.EventHandler(this.menuNew_Click);
            // 
            // menuOpenConfig
            // 
            this.menuOpenConfig.Name = "menuOpenConfig";
            this.menuOpenConfig.Size = new System.Drawing.Size(179, 22);
            this.menuOpenConfig.Text = "&Open Configuration";
            this.menuOpenConfig.Click += new System.EventHandler(this.menuOpen_Click);
            // 
            // menuClose
            // 
            this.menuClose.Enabled = false;
            this.menuClose.Name = "menuClose";
            this.menuClose.Size = new System.Drawing.Size(179, 22);
            this.menuClose.Text = "&Close";
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(176, 6);
            // 
            // menuSave
            // 
            this.menuSave.Name = "menuSave";
            this.menuSave.Size = new System.Drawing.Size(179, 22);
            this.menuSave.Text = "&Save";
            this.menuSave.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // menuSaveAs
            // 
            this.menuSaveAs.Name = "menuSaveAs";
            this.menuSaveAs.Size = new System.Drawing.Size(179, 22);
            this.menuSaveAs.Text = "Save &As";
            this.menuSaveAs.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
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
            // menuExit
            // 
            this.menuExit.Name = "menuExit";
            this.menuExit.Size = new System.Drawing.Size(179, 22);
            this.menuExit.Text = "E&xit";
            this.menuExit.Click += new System.EventHandler(this.menuExit_Click);
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
            this.tcMain.TabIndex = 0;
            this.tcMain.SelectedIndexChanged += new System.EventHandler(this.FireUpdateStatusBar);
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
            this.splitContainer1.TabStop = false;
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
            this.lstDBSpecies.TabIndex = 0;
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
            this.menuSpDBAddToProject,
            this.toolStripMenuItem5,
            this.menuDelete,
            this.menuNewSpecies});
            this.menuSpDBContext.Name = "menuSpDBContext";
            this.menuSpDBContext.Size = new System.Drawing.Size(191, 126);
            this.menuSpDBContext.Opening += new System.ComponentModel.CancelEventHandler(this.menuSpDBContext_Opening);
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
            // toolStripMenuItem5
            // 
            this.toolStripMenuItem5.Name = "toolStripMenuItem5";
            this.toolStripMenuItem5.Size = new System.Drawing.Size(187, 6);
            // 
            // menuDelete
            // 
            this.menuDelete.Name = "menuDelete";
            this.menuDelete.Size = new System.Drawing.Size(190, 22);
            this.menuDelete.Text = "Delete";
            this.menuDelete.Click += new System.EventHandler(this.menuDelete_Click);
            // 
            // menuNewSpecies
            // 
            this.menuNewSpecies.Name = "menuNewSpecies";
            this.menuNewSpecies.Size = new System.Drawing.Size(190, 22);
            this.menuNewSpecies.Text = "New Species...";
            this.menuNewSpecies.Click += new System.EventHandler(this.menuNewSpecies_Click);
            // 
            // pnlFilter
            // 
            this.pnlFilter.Controls.Add(this.txtFilter);
            this.pnlFilter.Controls.Add(this.label1);
            this.pnlFilter.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnlFilter.Location = new System.Drawing.Point(3, 508);
            this.pnlFilter.Name = "pnlFilter";
            this.pnlFilter.Size = new System.Drawing.Size(219, 27);
            this.pnlFilter.TabIndex = 1;
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
            this.tcSpecies.Controls.Add(this.tabSpecieDatabase);
            this.tcSpecies.Controls.Add(this.tabProjectSpeciesTest);
            this.tcSpecies.Controls.Add(this.tabDatabaseTest);
            this.tcSpecies.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tcSpecies.Location = new System.Drawing.Point(0, 0);
            this.tcSpecies.Name = "tcSpecies";
            this.tcSpecies.SelectedIndex = 0;
            this.tcSpecies.Size = new System.Drawing.Size(709, 538);
            this.tcSpecies.TabIndex = 1;
            this.tcSpecies.SelectedIndexChanged += new System.EventHandler(this.FireUpdateStatusBar);
            // 
            // tabSpecieDatabase
            // 
            this.tabSpecieDatabase.AutoScroll = true;
            this.tabSpecieDatabase.Controls.Add(this.groupBox6);
            this.tabSpecieDatabase.Controls.Add(this.groupBox8);
            this.tabSpecieDatabase.Controls.Add(this.txtDensityCorrection);
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
            this.groupBox6.Location = new System.Drawing.Point(3, 309);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(695, 200);
            this.groupBox6.TabIndex = 4;
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
            this.splitContainer2.Size = new System.Drawing.Size(562, 181);
            this.splitContainer2.SplitterDistance = 226;
            this.splitContainer2.TabIndex = 10;
            this.splitContainer2.TabStop = false;
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
            this.tlpEquations.Enabled = false;
            this.tlpEquations.Location = new System.Drawing.Point(0, 0);
            this.tlpEquations.Name = "tlpEquations";
            this.tlpEquations.RowCount = 5;
            this.tlpEquations.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tlpEquations.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tlpEquations.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tlpEquations.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tlpEquations.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tlpEquations.Size = new System.Drawing.Size(332, 181);
            this.tlpEquations.TabIndex = 1;
            // 
            // txtMaxTemp3
            // 
            this.txtMaxTemp3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMaxTemp3.Location = new System.Drawing.Point(285, 115);
            this.txtMaxTemp3.Name = "txtMaxTemp3";
            this.txtMaxTemp3.Size = new System.Drawing.Size(44, 20);
            this.txtMaxTemp3.TabIndex = 11;
            this.txtMaxTemp3.Leave += new System.EventHandler(this.txtFormula0_Leave);
            this.txtMaxTemp3.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.NumbersOnly);
            // 
            // txtMinTemp3
            // 
            this.txtMinTemp3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMinTemp3.Location = new System.Drawing.Point(220, 115);
            this.txtMinTemp3.Name = "txtMinTemp3";
            this.txtMinTemp3.Size = new System.Drawing.Size(44, 20);
            this.txtMinTemp3.TabIndex = 10;
            this.txtMinTemp3.Leave += new System.EventHandler(this.txtFormula0_Leave);
            this.txtMinTemp3.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.NumbersOnly);
            // 
            // txtMaxTemp2
            // 
            this.txtMaxTemp2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMaxTemp2.Location = new System.Drawing.Point(285, 89);
            this.txtMaxTemp2.Name = "txtMaxTemp2";
            this.txtMaxTemp2.Size = new System.Drawing.Size(44, 20);
            this.txtMaxTemp2.TabIndex = 8;
            this.txtMaxTemp2.Leave += new System.EventHandler(this.txtFormula0_Leave);
            this.txtMaxTemp2.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.NumbersOnly);
            // 
            // txtMinTemp2
            // 
            this.txtMinTemp2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMinTemp2.Location = new System.Drawing.Point(220, 89);
            this.txtMinTemp2.Name = "txtMinTemp2";
            this.txtMinTemp2.Size = new System.Drawing.Size(44, 20);
            this.txtMinTemp2.TabIndex = 7;
            this.txtMinTemp2.Leave += new System.EventHandler(this.txtFormula0_Leave);
            this.txtMinTemp2.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.NumbersOnly);
            // 
            // txtMaxTemp1
            // 
            this.txtMaxTemp1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMaxTemp1.Location = new System.Drawing.Point(285, 63);
            this.txtMaxTemp1.Name = "txtMaxTemp1";
            this.txtMaxTemp1.Size = new System.Drawing.Size(44, 20);
            this.txtMaxTemp1.TabIndex = 5;
            this.txtMaxTemp1.Leave += new System.EventHandler(this.txtFormula0_Leave);
            this.txtMaxTemp1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.NumbersOnly);
            // 
            // txtMinTemp1
            // 
            this.txtMinTemp1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMinTemp1.Location = new System.Drawing.Point(220, 63);
            this.txtMinTemp1.Name = "txtMinTemp1";
            this.txtMinTemp1.Size = new System.Drawing.Size(44, 20);
            this.txtMinTemp1.TabIndex = 4;
            this.txtMinTemp1.Leave += new System.EventHandler(this.txtFormula0_Leave);
            this.txtMinTemp1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.NumbersOnly);
            // 
            // txtMaxTemp0
            // 
            this.txtMaxTemp0.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMaxTemp0.Location = new System.Drawing.Point(285, 37);
            this.txtMaxTemp0.Name = "txtMaxTemp0";
            this.txtMaxTemp0.Size = new System.Drawing.Size(44, 20);
            this.txtMaxTemp0.TabIndex = 2;
            this.txtMaxTemp0.Leave += new System.EventHandler(this.txtFormula0_Leave);
            this.txtMaxTemp0.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.NumbersOnly);
            // 
            // txtMinTemp0
            // 
            this.txtMinTemp0.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMinTemp0.Location = new System.Drawing.Point(220, 37);
            this.txtMinTemp0.Name = "txtMinTemp0";
            this.txtMinTemp0.Size = new System.Drawing.Size(44, 20);
            this.txtMinTemp0.TabIndex = 1;
            this.txtMinTemp0.Leave += new System.EventHandler(this.txtFormula0_Leave);
            this.txtMinTemp0.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.NumbersOnly);
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
            this.txtFormula0.TabIndex = 0;
            this.txtFormula0.Leave += new System.EventHandler(this.txtFormula0_Leave);
            this.txtFormula0.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PreventCurlyBraces);
            // 
            // txtFormula1
            // 
            this.txtFormula1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtFormula1.Location = new System.Drawing.Point(3, 63);
            this.txtFormula1.Name = "txtFormula1";
            this.txtFormula1.Size = new System.Drawing.Size(211, 20);
            this.txtFormula1.TabIndex = 3;
            this.txtFormula1.Leave += new System.EventHandler(this.txtFormula0_Leave);
            this.txtFormula1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PreventCurlyBraces);
            // 
            // txtFormula2
            // 
            this.txtFormula2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtFormula2.Location = new System.Drawing.Point(3, 89);
            this.txtFormula2.Name = "txtFormula2";
            this.txtFormula2.Size = new System.Drawing.Size(211, 20);
            this.txtFormula2.TabIndex = 6;
            this.txtFormula2.Leave += new System.EventHandler(this.txtFormula0_Leave);
            this.txtFormula2.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PreventCurlyBraces);
            // 
            // txtFormula3
            // 
            this.txtFormula3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtFormula3.Location = new System.Drawing.Point(3, 115);
            this.txtFormula3.Name = "txtFormula3";
            this.txtFormula3.Size = new System.Drawing.Size(211, 20);
            this.txtFormula3.TabIndex = 9;
            this.txtFormula3.Leave += new System.EventHandler(this.txtFormula0_Leave);
            this.txtFormula3.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PreventCurlyBraces);
            // 
            // pnlTempDependantRadios
            // 
            this.pnlTempDependantRadios.Controls.Add(this.tlpTempDependantRadios);
            this.pnlTempDependantRadios.Controls.Add(this.label37);
            this.pnlTempDependantRadios.Dock = System.Windows.Forms.DockStyle.Left;
            this.pnlTempDependantRadios.Location = new System.Drawing.Point(3, 16);
            this.pnlTempDependantRadios.Name = "pnlTempDependantRadios";
            this.pnlTempDependantRadios.Size = new System.Drawing.Size(127, 181);
            this.pnlTempDependantRadios.TabIndex = 0;
            // 
            // tlpTempDependantRadios
            // 
            this.tlpTempDependantRadios.AutoScroll = true;
            this.tlpTempDependantRadios.ColumnCount = 1;
            this.tlpTempDependantRadios.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.tlpTempDependantRadios.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tlpTempDependantRadios.Location = new System.Drawing.Point(0, 13);
            this.tlpTempDependantRadios.Name = "tlpTempDependantRadios";
            this.tlpTempDependantRadios.RowCount = 1;
            this.tlpTempDependantRadios.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tlpTempDependantRadios.Size = new System.Drawing.Size(127, 168);
            this.tlpTempDependantRadios.TabIndex = 1;
            // 
            // label37
            // 
            this.label37.AutoSize = true;
            this.label37.Dock = System.Windows.Forms.DockStyle.Top;
            this.label37.Location = new System.Drawing.Point(0, 0);
            this.label37.Name = "label37";
            this.label37.Size = new System.Drawing.Size(92, 13);
            this.label37.TabIndex = 0;
            this.label37.Text = "Adjust Values For:";
            // 
            // groupBox8
            // 
            this.groupBox8.Controls.Add(this.textBox1);
            this.groupBox8.Controls.Add(this.label51);
            this.groupBox8.Controls.Add(this.txtPhaseChange);
            this.groupBox8.Controls.Add(this.label48);
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
            this.groupBox8.Size = new System.Drawing.Size(695, 124);
            this.groupBox8.TabIndex = 3;
            this.groupBox8.TabStop = false;
            this.groupBox8.Text = "Thermodynamic Properties";
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(125, 97);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(392, 20);
            this.textBox1.TabIndex = 19;
            // 
            // label51
            // 
            this.label51.AutoSize = true;
            this.label51.Location = new System.Drawing.Point(26, 100);
            this.label51.Name = "label51";
            this.label51.Size = new System.Drawing.Size(93, 13);
            this.label51.TabIndex = 20;
            this.label51.Text = "Density Correction";
            // 
            // txtPhaseChange
            // 
            this.txtPhaseChange.Location = new System.Drawing.Point(387, 19);
            this.txtPhaseChange.Name = "txtPhaseChange";
            this.txtPhaseChange.Size = new System.Drawing.Size(130, 20);
            this.txtPhaseChange.TabIndex = 1;
            // 
            // label48
            // 
            this.label48.AutoSize = true;
            this.label48.Location = new System.Drawing.Point(304, 22);
            this.label48.Name = "label48";
            this.label48.Size = new System.Drawing.Size(77, 13);
            this.label48.TabIndex = 18;
            this.label48.Text = "Phase Change";
            // 
            // txtSolvent
            // 
            this.txtSolvent.Location = new System.Drawing.Point(125, 19);
            this.txtSolvent.Name = "txtSolvent";
            this.txtSolvent.Size = new System.Drawing.Size(130, 20);
            this.txtSolvent.TabIndex = 0;
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
            this.txtAccentricity.TabIndex = 5;
            // 
            // txtCritTemp
            // 
            this.txtCritTemp.Location = new System.Drawing.Point(387, 45);
            this.txtCritTemp.Name = "txtCritTemp";
            this.txtCritTemp.Size = new System.Drawing.Size(130, 20);
            this.txtCritTemp.TabIndex = 3;
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
            this.txtCritVol.TabIndex = 4;
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
            this.txtCritPressure.TabIndex = 2;
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
            // txtDensityCorrection
            // 
            this.txtDensityCorrection.Controls.Add(this.txtCheckedBy);
            this.txtDensityCorrection.Controls.Add(this.label35);
            this.txtDensityCorrection.Controls.Add(this.label34);
            this.txtDensityCorrection.Controls.Add(this.txtSource);
            this.txtDensityCorrection.Dock = System.Windows.Forms.DockStyle.Top;
            this.txtDensityCorrection.Location = new System.Drawing.Point(3, 134);
            this.txtDensityCorrection.Name = "txtDensityCorrection";
            this.txtDensityCorrection.Size = new System.Drawing.Size(695, 51);
            this.txtDensityCorrection.TabIndex = 2;
            this.txtDensityCorrection.TabStop = false;
            this.txtDensityCorrection.Text = "Source";
            // 
            // txtCheckedBy
            // 
            this.txtCheckedBy.Location = new System.Drawing.Point(387, 19);
            this.txtCheckedBy.Name = "txtCheckedBy";
            this.txtCheckedBy.Size = new System.Drawing.Size(130, 20);
            this.txtCheckedBy.TabIndex = 1;
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
            this.txtSource.Size = new System.Drawing.Size(183, 20);
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
            this.groupBox4.TabIndex = 1;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "General Properties";
            // 
            // comboOccurence
            // 
            this.comboOccurence.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboOccurence.FormattingEnabled = true;
            this.comboOccurence.Items.AddRange(new object[] {
            "s",
            "l",
            "g"});
            this.comboOccurence.Location = new System.Drawing.Point(387, 18);
            this.comboOccurence.Name = "comboOccurence";
            this.comboOccurence.Size = new System.Drawing.Size(130, 21);
            this.comboOccurence.TabIndex = 1;
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
            this.txtMaxTemperature.TabIndex = 3;
            this.txtMaxTemperature.TextChanged += new System.EventHandler(this.txtMaxTemperature_TextChanged);
            // 
            // txtMinTemperature
            // 
            this.txtMinTemperature.Location = new System.Drawing.Point(125, 50);
            this.txtMinTemperature.Name = "txtMinTemperature";
            this.txtMinTemperature.Size = new System.Drawing.Size(130, 20);
            this.txtMinTemperature.TabIndex = 2;
            this.txtMinTemperature.TextChanged += new System.EventHandler(this.txtMinTemperature_TextChanged);
            // 
            // txtElementalComposition
            // 
            this.txtElementalComposition.Location = new System.Drawing.Point(125, 19);
            this.txtElementalComposition.Name = "txtElementalComposition";
            this.txtElementalComposition.Size = new System.Drawing.Size(130, 20);
            this.txtElementalComposition.TabIndex = 0;
            this.txtElementalComposition.Validated += new System.EventHandler(this.txtElementalComposition_Validated);
            this.txtElementalComposition.Validating += new System.ComponentModel.CancelEventHandler(this.txtElementalComposition_Validating);
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
            this.grpNomenclature.Controls.Add(this.comboPhase);
            this.grpNomenclature.Controls.Add(this.label25);
            this.grpNomenclature.Controls.Add(this.txtName);
            this.grpNomenclature.Controls.Add(this.label30);
            this.grpNomenclature.Controls.Add(this.txtSymbol);
            this.grpNomenclature.Controls.Add(this.label31);
            this.grpNomenclature.Controls.Add(this.label32);
            this.grpNomenclature.Dock = System.Windows.Forms.DockStyle.Top;
            this.grpNomenclature.Location = new System.Drawing.Point(3, 3);
            this.grpNomenclature.Name = "grpNomenclature";
            this.grpNomenclature.Size = new System.Drawing.Size(695, 49);
            this.grpNomenclature.TabIndex = 0;
            this.grpNomenclature.TabStop = false;
            this.grpNomenclature.Text = "Nomenclature";
            // 
            // comboPhase
            // 
            this.comboPhase.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Suggest;
            this.comboPhase.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems;
            this.comboPhase.FormattingEnabled = true;
            this.comboPhase.Location = new System.Drawing.Point(436, 19);
            this.comboPhase.Name = "comboPhase";
            this.comboPhase.Size = new System.Drawing.Size(65, 21);
            this.comboPhase.TabIndex = 2;
            this.comboPhase.Validated += new System.EventHandler(this.txtSymbol_Validated);
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
            this.txtName.TabIndex = 0;
            this.txtName.Validated += new System.EventHandler(this.txtName_Validated);
            this.txtName.Validating += new System.ComponentModel.CancelEventHandler(this.txtName_Validating);
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
            this.txtSymbol.Location = new System.Drawing.Point(314, 19);
            this.txtSymbol.Name = "txtSymbol";
            this.txtSymbol.Size = new System.Drawing.Size(100, 20);
            this.txtSymbol.TabIndex = 1;
            this.txtSymbol.Validated += new System.EventHandler(this.txtSymbol_Validated);
            this.txtSymbol.Validating += new System.ComponentModel.CancelEventHandler(this.txtSymbol_Validating);
            // 
            // label31
            // 
            this.label31.AutoSize = true;
            this.label31.Location = new System.Drawing.Point(420, 22);
            this.label31.Name = "label31";
            this.label31.Size = new System.Drawing.Size(10, 13);
            this.label31.TabIndex = 5;
            this.label31.Text = "(";
            // 
            // label32
            // 
            this.label32.AutoSize = true;
            this.label32.Location = new System.Drawing.Point(267, 22);
            this.label32.Name = "label32";
            this.label32.Size = new System.Drawing.Size(41, 13);
            this.label32.TabIndex = 6;
            this.label32.Text = "Symbol";
            // 
            // tabProjectSpeciesTest
            // 
            this.tabProjectSpeciesTest.Controls.Add(this.projectVectorControl1);
            this.tabProjectSpeciesTest.Location = new System.Drawing.Point(4, 22);
            this.tabProjectSpeciesTest.Name = "tabProjectSpeciesTest";
            this.tabProjectSpeciesTest.Padding = new System.Windows.Forms.Padding(3);
            this.tabProjectSpeciesTest.Size = new System.Drawing.Size(701, 512);
            this.tabProjectSpeciesTest.TabIndex = 2;
            this.tabProjectSpeciesTest.Text = "Project Species (Test)";
            this.tabProjectSpeciesTest.UseVisualStyleBackColor = true;
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
            this.chkH2OAqueous.TabIndex = 1;
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
            this.comboSpecieModel.TabIndex = 0;
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
            this.txtMinP.TabIndex = 6;
            // 
            // txtNP
            // 
            this.txtNP.Location = new System.Drawing.Point(199, 59);
            this.txtNP.Name = "txtNP";
            this.txtNP.Size = new System.Drawing.Size(92, 20);
            this.txtNP.TabIndex = 5;
            // 
            // txtStP
            // 
            this.txtStP.Location = new System.Drawing.Point(101, 59);
            this.txtStP.Name = "txtStP";
            this.txtStP.Size = new System.Drawing.Size(92, 20);
            this.txtStP.TabIndex = 4;
            // 
            // txtMaxT
            // 
            this.txtMaxT.Location = new System.Drawing.Point(395, 31);
            this.txtMaxT.Name = "txtMaxT";
            this.txtMaxT.Size = new System.Drawing.Size(92, 20);
            this.txtMaxT.TabIndex = 3;
            // 
            // txtMinT
            // 
            this.txtMinT.Location = new System.Drawing.Point(297, 31);
            this.txtMinT.Name = "txtMinT";
            this.txtMinT.Size = new System.Drawing.Size(92, 20);
            this.txtMinT.TabIndex = 2;
            // 
            // txtNT
            // 
            this.txtNT.Location = new System.Drawing.Point(199, 31);
            this.txtNT.Name = "txtNT";
            this.txtNT.Size = new System.Drawing.Size(92, 20);
            this.txtNT.TabIndex = 1;
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
            this.txtStT.TabIndex = 0;
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
            this.comboDynamicHeat.TabIndex = 7;
            // 
            // comboProBalHeat
            // 
            this.comboProBalHeat.Dock = System.Windows.Forms.DockStyle.Fill;
            this.comboProBalHeat.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboProBalHeat.FormattingEnabled = true;
            this.comboProBalHeat.Location = new System.Drawing.Point(125, 78);
            this.comboProBalHeat.Name = "comboProBalHeat";
            this.comboProBalHeat.Size = new System.Drawing.Size(116, 21);
            this.comboProBalHeat.TabIndex = 6;
            // 
            // comboMaxCompFlow
            // 
            this.comboMaxCompFlow.Dock = System.Windows.Forms.DockStyle.Fill;
            this.comboMaxCompFlow.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboMaxCompFlow.FormattingEnabled = true;
            this.comboMaxCompFlow.Location = new System.Drawing.Point(369, 53);
            this.comboMaxCompFlow.Name = "comboMaxCompFlow";
            this.comboMaxCompFlow.Size = new System.Drawing.Size(118, 21);
            this.comboMaxCompFlow.TabIndex = 5;
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
            this.comboProbalFlow.TabIndex = 3;
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
            this.comboMaxCompSurge.TabIndex = 2;
            // 
            // comboDynamicSurge
            // 
            this.comboDynamicSurge.Dock = System.Windows.Forms.DockStyle.Fill;
            this.comboDynamicSurge.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboDynamicSurge.FormattingEnabled = true;
            this.comboDynamicSurge.Location = new System.Drawing.Point(247, 28);
            this.comboDynamicSurge.Name = "comboDynamicSurge";
            this.comboDynamicSurge.Size = new System.Drawing.Size(116, 21);
            this.comboDynamicSurge.TabIndex = 1;
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
            this.comboProBalSurge.TabIndex = 0;
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
            this.comboDynamicFlow.TabIndex = 4;
            // 
            // comboMaxCompHeat
            // 
            this.comboMaxCompHeat.Dock = System.Windows.Forms.DockStyle.Fill;
            this.comboMaxCompHeat.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboMaxCompHeat.FormattingEnabled = true;
            this.comboMaxCompHeat.Location = new System.Drawing.Point(369, 78);
            this.comboMaxCompHeat.Name = "comboMaxCompHeat";
            this.comboMaxCompHeat.Size = new System.Drawing.Size(118, 21);
            this.comboMaxCompHeat.TabIndex = 8;
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
            this.chkDynamicAllowed.TabIndex = 1;
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
            this.chkProbalAllowed.TabIndex = 2;
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
            this.comboDefaultSolution.TabIndex = 0;
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
            this.scModels.TabStop = false;
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
            // errorProvider1
            // 
            this.errorProvider1.ContainerControl = this;
            // 
            // tabDatabaseTest
            // 
            this.tabDatabaseTest.Controls.Add(this.specieDatabaseControl1);
            this.tabDatabaseTest.Location = new System.Drawing.Point(4, 22);
            this.tabDatabaseTest.Name = "tabDatabaseTest";
            this.tabDatabaseTest.Padding = new System.Windows.Forms.Padding(3);
            this.tabDatabaseTest.Size = new System.Drawing.Size(701, 512);
            this.tabDatabaseTest.TabIndex = 3;
            this.tabDatabaseTest.Text = "Specie Database (Test)";
            this.tabDatabaseTest.UseVisualStyleBackColor = true;
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
            this.graph1.Size = new System.Drawing.Size(226, 181);
            this.graph1.TabIndex = 6;
            this.graph1.UnselectedTransparency = 64;
            this.graph1.XLabel = "Temperature (K)";
            this.graph1.YLabel = null;
            this.graph1.YLabelColour = System.Drawing.Color.Empty;
            // 
            // projectVectorControl1
            // 
            this.projectVectorControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.projectVectorControl1.ListDBSpecies = null;
            this.projectVectorControl1.Location = new System.Drawing.Point(3, 3);
            this.projectVectorControl1.Name = "projectVectorControl1";
            this.projectVectorControl1.Size = new System.Drawing.Size(695, 506);
            this.projectVectorControl1.Sorter = pviOrderer1;
            this.projectVectorControl1.SpecieDataTable = null;
            this.projectVectorControl1.StatusColour = System.Drawing.Color.Empty;
            this.projectVectorControl1.StatusMessage = null;
            this.projectVectorControl1.TabIndex = 0;
            // 
            // specieDatabaseControl1
            // 
            this.specieDatabaseControl1.DataSource = null;
            this.specieDatabaseControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.specieDatabaseControl1.Location = new System.Drawing.Point(3, 3);
            this.specieDatabaseControl1.Name = "specieDatabaseControl1";
            this.specieDatabaseControl1.Size = new System.Drawing.Size(695, 506);
            this.specieDatabaseControl1.StatusColour = System.Drawing.SystemColors.WindowText;
            this.specieDatabaseControl1.StatusMessage = "";
            this.specieDatabaseControl1.TabIndex = 0;
            // 
            // FrmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(952, 616);
            this.Controls.Add(this.tcMain);
            this.Controls.Add(this.ssMain);
            this.Controls.Add(this.menuMain);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuMain;
            this.Name = "FrmMain";
            this.Text = "SysCAD Configuration Editor";
            this.ssMain.ResumeLayout(false);
            this.ssMain.PerformLayout();
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
            this.txtDensityCorrection.ResumeLayout(false);
            this.txtDensityCorrection.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.grpNomenclature.ResumeLayout(false);
            this.grpNomenclature.PerformLayout();
            this.tabProjectSpeciesTest.ResumeLayout(false);
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
            ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).EndInit();
            this.tabDatabaseTest.ResumeLayout(false);
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
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem MenuNew;
        private System.Windows.Forms.ToolStripMenuItem menuOpenConfig;
        private System.Windows.Forms.ToolStripMenuItem menuClose;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem menuSave;
        private System.Windows.Forms.ToolStripMenuItem menuSaveAs;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem menuOpenDatabase;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem3;
        private System.Windows.Forms.ToolStripMenuItem menuExit;
        private System.Windows.Forms.TabPage tabModels;
        private System.Windows.Forms.OpenFileDialog dlgOpenDB;
        private System.Windows.Forms.ColumnHeader chSymbol;
        private System.Windows.Forms.ColumnHeader chName;
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
        private System.Windows.Forms.TabPage tabSpecieDatabase;
        private System.Windows.Forms.TextBox txtName;
        private System.Windows.Forms.TextBox txtSymbol;
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
        private System.Windows.Forms.GroupBox txtDensityCorrection;
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
        private System.Windows.Forms.Label label37;
        private System.Windows.Forms.TextBox txtSolvent;
        private System.Windows.Forms.Label label36;
        private System.Windows.Forms.Label label39;
        private System.Windows.Forms.Label label38;
        private Graph graph1;
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
        private System.Windows.Forms.TextBox txtPhaseChange;
        private System.Windows.Forms.Label label48;
        private System.Windows.Forms.ComboBox comboPhase;
        private System.Windows.Forms.ErrorProvider errorProvider1;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem5;
        private System.Windows.Forms.ToolStripMenuItem menuDelete;
        private System.Windows.Forms.ToolStripMenuItem menuNewSpecies;
        private System.Windows.Forms.TableLayoutPanel tlpTempDependantRadios;
        private System.Windows.Forms.ToolStripStatusLabel statusLabel;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label51;
        private System.Windows.Forms.TabPage tabProjectSpeciesTest;
        private ProjectVectorControl projectVectorControl1;
        private System.Windows.Forms.TabPage tabDatabaseTest;
        private SpecieDatabaseControl specieDatabaseControl1;
    }
}

