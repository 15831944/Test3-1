namespace Reaction_Editor
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
            System.Windows.Forms.TreeNode treeNode1 = new System.Windows.Forms.TreeNode("Specie Database");
            System.Windows.Forms.TreeNode treeNode2 = new System.Windows.Forms.TreeNode("Reaction Blocks");
            System.Windows.Forms.ListViewGroup listViewGroup1 = new System.Windows.Forms.ListViewGroup("Solids", System.Windows.Forms.HorizontalAlignment.Left);
            System.Windows.Forms.ListViewGroup listViewGroup2 = new System.Windows.Forms.ListViewGroup("Liquids", System.Windows.Forms.HorizontalAlignment.Left);
            System.Windows.Forms.ListViewGroup listViewGroup3 = new System.Windows.Forms.ListViewGroup("Vapours", System.Windows.Forms.HorizontalAlignment.Left);
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FrmMain));
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.menuFile = new System.Windows.Forms.ToolStripMenuItem();
            this.menuNew = new System.Windows.Forms.ToolStripMenuItem();
            this.menuOpen = new System.Windows.Forms.ToolStripMenuItem();
            this.menuOpenDir = new System.Windows.Forms.ToolStripMenuItem();
            this.menuClose = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.menuRevert = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
            this.menuSave = new System.Windows.Forms.ToolStripMenuItem();
            this.menuSaveAs = new System.Windows.Forms.ToolStripMenuItem();
            this.menuSaveAll = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
            this.menuOpenDB = new System.Windows.Forms.ToolStripMenuItem();
            this.menuRecentFileSeperator = new System.Windows.Forms.ToolStripSeparator();
            this.menuExit = new System.Windows.Forms.ToolStripMenuItem();
            this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuCut = new System.Windows.Forms.ToolStripMenuItem();
            this.menuCopy = new System.Windows.Forms.ToolStripMenuItem();
            this.menuPaste = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem5 = new System.Windows.Forms.ToolStripSeparator();
            this.menuAutoCompleteSets = new System.Windows.Forms.ToolStripMenuItem();
            this.windowToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.arrangeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuCascade = new System.Windows.Forms.ToolStripMenuItem();
            this.undockToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripSeparator();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutSysCADReactionEditorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.m_StatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.grpFiles = new System.Windows.Forms.GroupBox();
            this.treeFiles = new System.Windows.Forms.TreeView();
            this.grpSpecies = new System.Windows.Forms.GroupBox();
            this.lstSpecies = new System.Windows.Forms.ListView();
            this.chSymbol = new System.Windows.Forms.ColumnHeader();
            this.chName = new System.Windows.Forms.ColumnHeader();
            this.menuSpecieList = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.menuSortAlphabetically = new System.Windows.Forms.ToolStripMenuItem();
            this.menuSortByPhase = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripSeparator();
            this.menuSetSource = new System.Windows.Forms.ToolStripMenuItem();
            this.menuSetSink = new System.Windows.Forms.ToolStripMenuItem();
            this.menuRemoveSource = new System.Windows.Forms.ToolStripMenuItem();
            this.menuRemoveSink = new System.Windows.Forms.ToolStripMenuItem();
            this.pnlFilter = new System.Windows.Forms.Panel();
            this.txtFilter = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.splitterLeft = new System.Windows.Forms.Splitter();
            this.dlgOpenRxn = new System.Windows.Forms.OpenFileDialog();
            this.dlgSaveRxn = new System.Windows.Forms.SaveFileDialog();
            this.dlgOpenDB = new System.Windows.Forms.OpenFileDialog();
            this.menuDatabaseFile = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.unloadAllSpeciesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.dlgOpenFolder = new System.Windows.Forms.FolderBrowserDialog();
            this.splitterRight = new System.Windows.Forms.Splitter();
            this.btnCollapseRight = new System.Windows.Forms.Button();
            this.btnCollapseLeft = new System.Windows.Forms.Button();
            this.pnlLog = new System.Windows.Forms.Panel();
            this.lstLog = new System.Windows.Forms.ListView();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.listView1 = new System.Windows.Forms.ListView();
            this.pnlLogHeader = new System.Windows.Forms.Panel();
            this.btnLogCollapse = new System.Windows.Forms.Button();
            this.lblLogHeader = new System.Windows.Forms.Label();
            this.splitterLog = new System.Windows.Forms.Splitter();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.btnNew = new System.Windows.Forms.ToolStripButton();
            this.btnOpen = new System.Windows.Forms.ToolStripButton();
            this.btnSave = new System.Windows.Forms.ToolStripButton();
            this.btnSaveAll = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.btnCut = new System.Windows.Forms.ToolStripButton();
            this.btnCopy = new System.Windows.Forms.ToolStripButton();
            this.btnPaste = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.btnRevert = new System.Windows.Forms.ToolStripButton();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.menuStrip1.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.grpFiles.SuspendLayout();
            this.grpSpecies.SuspendLayout();
            this.menuSpecieList.SuspendLayout();
            this.pnlFilter.SuspendLayout();
            this.menuDatabaseFile.SuspendLayout();
            this.pnlLog.SuspendLayout();
            this.pnlLogHeader.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuFile,
            this.editToolStripMenuItem,
            this.windowToolStripMenuItem,
            this.helpToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(734, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // menuFile
            // 
            this.menuFile.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuNew,
            this.menuOpen,
            this.menuOpenDir,
            this.menuClose,
            this.toolStripSeparator3,
            this.menuRevert,
            this.toolStripMenuItem1,
            this.menuSave,
            this.menuSaveAs,
            this.menuSaveAll,
            this.toolStripMenuItem2,
            this.menuOpenDB,
            this.menuRecentFileSeperator,
            this.menuExit});
            this.menuFile.Name = "menuFile";
            this.menuFile.Size = new System.Drawing.Size(35, 20);
            this.menuFile.Text = "&File";
            this.menuFile.DropDownOpening += new System.EventHandler(this.menuFile_DropDownOpening);
            this.menuFile.Click += new System.EventHandler(this.fileToolStripMenuItem_Click);
            // 
            // menuNew
            // 
            this.menuNew.Name = "menuNew";
            this.menuNew.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N)));
            this.menuNew.Size = new System.Drawing.Size(166, 22);
            this.menuNew.Text = "&New";
            this.menuNew.Click += new System.EventHandler(this.menuNew_Click);
            // 
            // menuOpen
            // 
            this.menuOpen.Name = "menuOpen";
            this.menuOpen.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
            this.menuOpen.Size = new System.Drawing.Size(166, 22);
            this.menuOpen.Text = "&Open";
            this.menuOpen.Click += new System.EventHandler(this.menuOpen_Click);
            // 
            // menuOpenDir
            // 
            this.menuOpenDir.Name = "menuOpenDir";
            this.menuOpenDir.Size = new System.Drawing.Size(166, 22);
            this.menuOpenDir.Text = "Open &Folder";
            this.menuOpenDir.Click += new System.EventHandler(this.menuOpenDir_Click);
            // 
            // menuClose
            // 
            this.menuClose.Name = "menuClose";
            this.menuClose.Size = new System.Drawing.Size(166, 22);
            this.menuClose.Text = "&Close";
            this.menuClose.Click += new System.EventHandler(this.menuClose_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(163, 6);
            // 
            // menuRevert
            // 
            this.menuRevert.Name = "menuRevert";
            this.menuRevert.Size = new System.Drawing.Size(166, 22);
            this.menuRevert.Text = "&Revert To Saved";
            this.menuRevert.Click += new System.EventHandler(this.menuRevert_Click);
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(163, 6);
            // 
            // menuSave
            // 
            this.menuSave.Name = "menuSave";
            this.menuSave.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.menuSave.Size = new System.Drawing.Size(166, 22);
            this.menuSave.Text = "&Save";
            this.menuSave.Click += new System.EventHandler(this.menuSave_Click);
            // 
            // menuSaveAs
            // 
            this.menuSaveAs.Name = "menuSaveAs";
            this.menuSaveAs.Size = new System.Drawing.Size(166, 22);
            this.menuSaveAs.Text = "Save &As";
            this.menuSaveAs.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
            // 
            // menuSaveAll
            // 
            this.menuSaveAll.Name = "menuSaveAll";
            this.menuSaveAll.Size = new System.Drawing.Size(166, 22);
            this.menuSaveAll.Text = "Save A&ll";
            this.menuSaveAll.Click += new System.EventHandler(this.menuSaveAll_Click);
            // 
            // toolStripMenuItem2
            // 
            this.toolStripMenuItem2.Name = "toolStripMenuItem2";
            this.toolStripMenuItem2.Size = new System.Drawing.Size(163, 6);
            // 
            // menuOpenDB
            // 
            this.menuOpenDB.Name = "menuOpenDB";
            this.menuOpenDB.Size = new System.Drawing.Size(166, 22);
            this.menuOpenDB.Text = "Open Da&tabase";
            this.menuOpenDB.Click += new System.EventHandler(this.menuOpenDB_Click);
            // 
            // menuRecentFileSeperator
            // 
            this.menuRecentFileSeperator.Name = "menuRecentFileSeperator";
            this.menuRecentFileSeperator.Size = new System.Drawing.Size(163, 6);
            // 
            // menuExit
            // 
            this.menuExit.Name = "menuExit";
            this.menuExit.Size = new System.Drawing.Size(166, 22);
            this.menuExit.Text = "E&xit";
            this.menuExit.Click += new System.EventHandler(this.menuExit_Click);
            // 
            // editToolStripMenuItem
            // 
            this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuCut,
            this.menuCopy,
            this.menuPaste,
            this.toolStripMenuItem5,
            this.menuAutoCompleteSets});
            this.editToolStripMenuItem.Name = "editToolStripMenuItem";
            this.editToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.editToolStripMenuItem.Text = "&Edit";
            this.editToolStripMenuItem.DropDownOpening += new System.EventHandler(this.editToolStripMenuItem_DropDownOpening);
            // 
            // menuCut
            // 
            this.menuCut.Name = "menuCut";
            this.menuCut.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.X)));
            this.menuCut.Size = new System.Drawing.Size(192, 22);
            this.menuCut.Text = "Cu&t";
            this.menuCut.Click += new System.EventHandler(this.menuCut_Click);
            // 
            // menuCopy
            // 
            this.menuCopy.Name = "menuCopy";
            this.menuCopy.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.C)));
            this.menuCopy.Size = new System.Drawing.Size(192, 22);
            this.menuCopy.Text = "&Copy";
            this.menuCopy.Click += new System.EventHandler(this.menuCopy_Click);
            // 
            // menuPaste
            // 
            this.menuPaste.Name = "menuPaste";
            this.menuPaste.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.V)));
            this.menuPaste.Size = new System.Drawing.Size(192, 22);
            this.menuPaste.Text = "&Paste";
            this.menuPaste.Click += new System.EventHandler(this.menuPaste_Click);
            // 
            // toolStripMenuItem5
            // 
            this.toolStripMenuItem5.Name = "toolStripMenuItem5";
            this.toolStripMenuItem5.Size = new System.Drawing.Size(189, 6);
            // 
            // menuAutoCompleteSets
            // 
            this.menuAutoCompleteSets.Name = "menuAutoCompleteSets";
            this.menuAutoCompleteSets.Size = new System.Drawing.Size(192, 22);
            this.menuAutoCompleteSets.Text = "&Auto Complete Sets...";
            this.menuAutoCompleteSets.Click += new System.EventHandler(this.menuAutoCompleteSets_Click);
            // 
            // windowToolStripMenuItem
            // 
            this.windowToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.arrangeToolStripMenuItem,
            this.undockToolStripMenuItem,
            this.toolStripMenuItem3});
            this.windowToolStripMenuItem.Name = "windowToolStripMenuItem";
            this.windowToolStripMenuItem.Size = new System.Drawing.Size(57, 20);
            this.windowToolStripMenuItem.Text = "&Window";
            this.windowToolStripMenuItem.DropDownOpening += new System.EventHandler(this.windowToolStripMenuItem_Open);
            // 
            // arrangeToolStripMenuItem
            // 
            this.arrangeToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuCascade});
            this.arrangeToolStripMenuItem.Name = "arrangeToolStripMenuItem";
            this.arrangeToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.arrangeToolStripMenuItem.Text = "&Arrange";
            // 
            // menuCascade
            // 
            this.menuCascade.Name = "menuCascade";
            this.menuCascade.Size = new System.Drawing.Size(126, 22);
            this.menuCascade.Text = "&Cascade";
            this.menuCascade.Click += new System.EventHandler(this.menuCascade_Click);
            // 
            // undockToolStripMenuItem
            // 
            this.undockToolStripMenuItem.Enabled = false;
            this.undockToolStripMenuItem.Name = "undockToolStripMenuItem";
            this.undockToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.undockToolStripMenuItem.Text = "&Undock";
            this.undockToolStripMenuItem.Click += new System.EventHandler(this.undockToolStripMenuItem_Click);
            // 
            // toolStripMenuItem3
            // 
            this.toolStripMenuItem3.Name = "toolStripMenuItem3";
            this.toolStripMenuItem3.Size = new System.Drawing.Size(149, 6);
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.aboutSysCADReactionEditorToolStripMenuItem});
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(40, 20);
            this.helpToolStripMenuItem.Text = "&Help";
            // 
            // aboutSysCADReactionEditorToolStripMenuItem
            // 
            this.aboutSysCADReactionEditorToolStripMenuItem.Name = "aboutSysCADReactionEditorToolStripMenuItem";
            this.aboutSysCADReactionEditorToolStripMenuItem.Size = new System.Drawing.Size(231, 22);
            this.aboutSysCADReactionEditorToolStripMenuItem.Text = "&About SysCAD Reaction Editor";
            this.aboutSysCADReactionEditorToolStripMenuItem.Click += new System.EventHandler(this.aboutSysCADReactionEditorToolStripMenuItem_Click);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.m_StatusLabel});
            this.statusStrip1.Location = new System.Drawing.Point(0, 361);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(734, 22);
            this.statusStrip1.TabIndex = 1;
            this.statusStrip1.Text = "statusStrip1";
            this.statusStrip1.Visible = false;
            // 
            // m_StatusLabel
            // 
            this.m_StatusLabel.Name = "m_StatusLabel";
            this.m_StatusLabel.Size = new System.Drawing.Size(0, 17);
            // 
            // grpFiles
            // 
            this.grpFiles.Controls.Add(this.treeFiles);
            this.grpFiles.Dock = System.Windows.Forms.DockStyle.Left;
            this.grpFiles.Location = new System.Drawing.Point(0, 49);
            this.grpFiles.Name = "grpFiles";
            this.grpFiles.Size = new System.Drawing.Size(146, 334);
            this.grpFiles.TabIndex = 2;
            this.grpFiles.TabStop = false;
            this.grpFiles.Text = "Files";
            this.grpFiles.Resize += new System.EventHandler(this.grpFiles_Resize);
            // 
            // treeFiles
            // 
            this.treeFiles.AllowDrop = true;
            this.treeFiles.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeFiles.Location = new System.Drawing.Point(3, 16);
            this.treeFiles.Name = "treeFiles";
            treeNode1.Name = "SpecieDB";
            treeNode1.Text = "Specie Database";
            treeNode2.Name = "RBs";
            treeNode2.Text = "Reaction Blocks";
            this.treeFiles.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
            treeNode1,
            treeNode2});
            this.treeFiles.Size = new System.Drawing.Size(140, 315);
            this.treeFiles.TabIndex = 0;
            this.treeFiles.DragDrop += new System.Windows.Forms.DragEventHandler(this.treeFiles_DragDrop);
            this.treeFiles.DragOver += new System.Windows.Forms.DragEventHandler(this.treeFiles_DragOver);
            this.treeFiles.DoubleClick += new System.EventHandler(this.treeFiles_DoubleClick);
            this.treeFiles.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.treeFiles_ItemDrag);
            // 
            // grpSpecies
            // 
            this.grpSpecies.Controls.Add(this.lstSpecies);
            this.grpSpecies.Controls.Add(this.pnlFilter);
            this.grpSpecies.Dock = System.Windows.Forms.DockStyle.Right;
            this.grpSpecies.Location = new System.Drawing.Point(562, 49);
            this.grpSpecies.Name = "grpSpecies";
            this.grpSpecies.Size = new System.Drawing.Size(172, 334);
            this.grpSpecies.TabIndex = 3;
            this.grpSpecies.TabStop = false;
            this.grpSpecies.Text = "Species";
            this.grpSpecies.Resize += new System.EventHandler(this.grpSpecies_Resize);
            // 
            // lstSpecies
            // 
            this.lstSpecies.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chSymbol,
            this.chName});
            this.lstSpecies.ContextMenuStrip = this.menuSpecieList;
            this.lstSpecies.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lstSpecies.FullRowSelect = true;
            listViewGroup1.Header = "Solids";
            listViewGroup1.Name = "Solid";
            listViewGroup2.Header = "Liquids";
            listViewGroup2.Name = "Liquid";
            listViewGroup3.Header = "Vapours";
            listViewGroup3.Name = "Gas";
            this.lstSpecies.Groups.AddRange(new System.Windows.Forms.ListViewGroup[] {
            listViewGroup1,
            listViewGroup2,
            listViewGroup3});
            this.lstSpecies.Location = new System.Drawing.Point(3, 16);
            this.lstSpecies.Name = "lstSpecies";
            this.lstSpecies.ShowItemToolTips = true;
            this.lstSpecies.Size = new System.Drawing.Size(166, 281);
            this.lstSpecies.TabIndex = 0;
            this.lstSpecies.UseCompatibleStateImageBehavior = false;
            this.lstSpecies.View = System.Windows.Forms.View.Details;
            this.lstSpecies.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.lstSpecies_ItemDrag);
            // 
            // chSymbol
            // 
            this.chSymbol.Text = "Symbol";
            this.chSymbol.Width = 115;
            // 
            // chName
            // 
            this.chName.Text = "Name";
            this.chName.Width = 46;
            // 
            // menuSpecieList
            // 
            this.menuSpecieList.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuSortAlphabetically,
            this.menuSortByPhase,
            this.toolStripMenuItem4,
            this.menuSetSource,
            this.menuSetSink,
            this.menuRemoveSource,
            this.menuRemoveSink});
            this.menuSpecieList.Name = "menuSpecieList";
            this.menuSpecieList.Size = new System.Drawing.Size(175, 142);
            this.menuSpecieList.Opening += new System.ComponentModel.CancelEventHandler(this.menuSpecieList_Opening);
            // 
            // menuSortAlphabetically
            // 
            this.menuSortAlphabetically.CheckOnClick = true;
            this.menuSortAlphabetically.Name = "menuSortAlphabetically";
            this.menuSortAlphabetically.Size = new System.Drawing.Size(174, 22);
            this.menuSortAlphabetically.Text = "Sort &Alphabetically";
            this.menuSortAlphabetically.CheckedChanged += new System.EventHandler(this.menuSortAlphabetically_CheckedChanged);
            // 
            // menuSortByPhase
            // 
            this.menuSortByPhase.Checked = true;
            this.menuSortByPhase.CheckOnClick = true;
            this.menuSortByPhase.CheckState = System.Windows.Forms.CheckState.Checked;
            this.menuSortByPhase.Name = "menuSortByPhase";
            this.menuSortByPhase.Size = new System.Drawing.Size(174, 22);
            this.menuSortByPhase.Text = "Sort By &Phase";
            this.menuSortByPhase.CheckedChanged += new System.EventHandler(this.menuSortByPhase_CheckedChanged);
            // 
            // toolStripMenuItem4
            // 
            this.toolStripMenuItem4.Name = "toolStripMenuItem4";
            this.toolStripMenuItem4.Size = new System.Drawing.Size(171, 6);
            // 
            // menuSetSource
            // 
            this.menuSetSource.Name = "menuSetSource";
            this.menuSetSource.Size = new System.Drawing.Size(174, 22);
            this.menuSetSource.Text = "Set as S&ource";
            this.menuSetSource.Click += new System.EventHandler(this.menuSetSource_Click);
            // 
            // menuSetSink
            // 
            this.menuSetSink.Name = "menuSetSink";
            this.menuSetSink.Size = new System.Drawing.Size(174, 22);
            this.menuSetSink.Text = "Set as S&ink";
            this.menuSetSink.Click += new System.EventHandler(this.menuSetSink_Click);
            // 
            // menuRemoveSource
            // 
            this.menuRemoveSource.Name = "menuRemoveSource";
            this.menuRemoveSource.Size = new System.Drawing.Size(174, 22);
            this.menuRemoveSource.Text = "Remove as So&urce";
            this.menuRemoveSource.Click += new System.EventHandler(this.menuRemoveSource_Click);
            // 
            // menuRemoveSink
            // 
            this.menuRemoveSink.Name = "menuRemoveSink";
            this.menuRemoveSink.Size = new System.Drawing.Size(174, 22);
            this.menuRemoveSink.Text = "Remove as Si&nk";
            this.menuRemoveSink.Click += new System.EventHandler(this.menuRemoveSink_Click);
            // 
            // pnlFilter
            // 
            this.pnlFilter.Controls.Add(this.txtFilter);
            this.pnlFilter.Controls.Add(this.label1);
            this.pnlFilter.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnlFilter.Location = new System.Drawing.Point(3, 297);
            this.pnlFilter.Name = "pnlFilter";
            this.pnlFilter.Size = new System.Drawing.Size(166, 34);
            this.pnlFilter.TabIndex = 1;
            // 
            // txtFilter
            // 
            this.txtFilter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtFilter.Location = new System.Drawing.Point(38, 6);
            this.txtFilter.Name = "txtFilter";
            this.txtFilter.Size = new System.Drawing.Size(119, 20);
            this.txtFilter.TabIndex = 1;
            this.txtFilter.TextChanged += new System.EventHandler(this.txtFilter_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(3, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Filter";
            // 
            // splitterLeft
            // 
            this.splitterLeft.Location = new System.Drawing.Point(146, 49);
            this.splitterLeft.Name = "splitterLeft";
            this.splitterLeft.Size = new System.Drawing.Size(5, 334);
            this.splitterLeft.TabIndex = 5;
            this.splitterLeft.TabStop = false;
            // 
            // dlgOpenRxn
            // 
            this.dlgOpenRxn.Filter = "Reaction files|*.rct|All files|*.*";
            this.dlgOpenRxn.Multiselect = true;
            this.dlgOpenRxn.Title = "Open Reaction";
            // 
            // dlgSaveRxn
            // 
            this.dlgSaveRxn.Filter = "Reaction Files|*.rct|All files|*.*";
            this.dlgSaveRxn.Title = "Save Reaction";
            // 
            // dlgOpenDB
            // 
            this.dlgOpenDB.Filter = "ini files|*.ini|All files|*.*";
            this.dlgOpenDB.Title = "Open Database";
            // 
            // menuDatabaseFile
            // 
            this.menuDatabaseFile.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.unloadAllSpeciesToolStripMenuItem});
            this.menuDatabaseFile.Name = "menuDatabaseFile";
            this.menuDatabaseFile.Size = new System.Drawing.Size(221, 26);
            // 
            // unloadAllSpeciesToolStripMenuItem
            // 
            this.unloadAllSpeciesToolStripMenuItem.Name = "unloadAllSpeciesToolStripMenuItem";
            this.unloadAllSpeciesToolStripMenuItem.Size = new System.Drawing.Size(220, 22);
            this.unloadAllSpeciesToolStripMenuItem.Text = "Unload &All Specie Databases";
            this.unloadAllSpeciesToolStripMenuItem.Click += new System.EventHandler(this.unloadAllSpeciesToolStripMenuItem_Click);
            // 
            // dlgOpenFolder
            // 
            this.dlgOpenFolder.RootFolder = System.Environment.SpecialFolder.MyComputer;
            this.dlgOpenFolder.ShowNewFolderButton = false;
            // 
            // splitterRight
            // 
            this.splitterRight.Dock = System.Windows.Forms.DockStyle.Right;
            this.splitterRight.Location = new System.Drawing.Point(557, 49);
            this.splitterRight.Name = "splitterRight";
            this.splitterRight.Size = new System.Drawing.Size(5, 334);
            this.splitterRight.TabIndex = 8;
            this.splitterRight.TabStop = false;
            this.splitterRight.DoubleClick += new System.EventHandler(this.splitterRight_DoubleClick);
            this.splitterRight.Move += new System.EventHandler(this.splitterRight_Move);
            // 
            // btnCollapseRight
            // 
            this.btnCollapseRight.Location = new System.Drawing.Point(558, 159);
            this.btnCollapseRight.Name = "btnCollapseRight";
            this.btnCollapseRight.Size = new System.Drawing.Size(6, 100);
            this.btnCollapseRight.TabIndex = 9;
            this.btnCollapseRight.UseVisualStyleBackColor = true;
            this.btnCollapseRight.Click += new System.EventHandler(this.btnCollapseRight_Click);
            // 
            // btnCollapseLeft
            // 
            this.btnCollapseLeft.Location = new System.Drawing.Point(146, 159);
            this.btnCollapseLeft.Name = "btnCollapseLeft";
            this.btnCollapseLeft.Size = new System.Drawing.Size(6, 100);
            this.btnCollapseLeft.TabIndex = 10;
            this.btnCollapseLeft.UseVisualStyleBackColor = true;
            this.btnCollapseLeft.Click += new System.EventHandler(this.btnCollapseLeft_Click);
            // 
            // pnlLog
            // 
            this.pnlLog.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.pnlLog.Controls.Add(this.lstLog);
            this.pnlLog.Controls.Add(this.listView1);
            this.pnlLog.Controls.Add(this.pnlLogHeader);
            this.pnlLog.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnlLog.Location = new System.Drawing.Point(151, 258);
            this.pnlLog.MinimumSize = new System.Drawing.Size(4, 18);
            this.pnlLog.Name = "pnlLog";
            this.pnlLog.Size = new System.Drawing.Size(406, 125);
            this.pnlLog.TabIndex = 11;
            this.pnlLog.Enter += new System.EventHandler(this.pnlLog_Enter);
            this.pnlLog.Leave += new System.EventHandler(this.pnlLog_Leave);
            // 
            // lstLog
            // 
            this.lstLog.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2});
            this.lstLog.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lstLog.Location = new System.Drawing.Point(0, 18);
            this.lstLog.Name = "lstLog";
            this.lstLog.ShowItemToolTips = true;
            this.lstLog.Size = new System.Drawing.Size(402, 103);
            this.lstLog.TabIndex = 2;
            this.lstLog.UseCompatibleStateImageBehavior = false;
            this.lstLog.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Source";
            this.columnHeader1.Width = 172;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Message";
            this.columnHeader2.Width = 700;
            // 
            // listView1
            // 
            this.listView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listView1.Location = new System.Drawing.Point(0, 18);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(402, 103);
            this.listView1.TabIndex = 1;
            this.listView1.UseCompatibleStateImageBehavior = false;
            // 
            // pnlLogHeader
            // 
            this.pnlLogHeader.BackColor = System.Drawing.SystemColors.InactiveCaption;
            this.pnlLogHeader.Controls.Add(this.btnLogCollapse);
            this.pnlLogHeader.Controls.Add(this.lblLogHeader);
            this.pnlLogHeader.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnlLogHeader.Location = new System.Drawing.Point(0, 0);
            this.pnlLogHeader.Name = "pnlLogHeader";
            this.pnlLogHeader.Size = new System.Drawing.Size(402, 18);
            this.pnlLogHeader.TabIndex = 0;
            this.pnlLogHeader.DoubleClick += new System.EventHandler(this.btnLogCollapse_Click);
            this.pnlLogHeader.Click += new System.EventHandler(this.pnlLogHeader_Click);
            // 
            // btnLogCollapse
            // 
            this.btnLogCollapse.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnLogCollapse.BackColor = System.Drawing.SystemColors.InactiveCaption;
            this.btnLogCollapse.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("btnLogCollapse.BackgroundImage")));
            this.btnLogCollapse.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.btnLogCollapse.FlatAppearance.BorderColor = System.Drawing.SystemColors.InactiveCaption;
            this.btnLogCollapse.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnLogCollapse.Location = new System.Drawing.Point(384, 0);
            this.btnLogCollapse.Name = "btnLogCollapse";
            this.btnLogCollapse.Size = new System.Drawing.Size(16, 16);
            this.btnLogCollapse.TabIndex = 1;
            this.btnLogCollapse.UseVisualStyleBackColor = false;
            this.btnLogCollapse.Click += new System.EventHandler(this.btnLogCollapse_Click);
            // 
            // lblLogHeader
            // 
            this.lblLogHeader.AutoSize = true;
            this.lblLogHeader.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblLogHeader.ForeColor = System.Drawing.SystemColors.InactiveCaptionText;
            this.lblLogHeader.Location = new System.Drawing.Point(7, 3);
            this.lblLogHeader.Name = "lblLogHeader";
            this.lblLogHeader.Size = new System.Drawing.Size(28, 13);
            this.lblLogHeader.TabIndex = 0;
            this.lblLogHeader.Text = "Log";
            this.lblLogHeader.DoubleClick += new System.EventHandler(this.btnLogCollapse_Click);
            this.lblLogHeader.Click += new System.EventHandler(this.pnlLogHeader_Click);
            // 
            // splitterLog
            // 
            this.splitterLog.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.splitterLog.Location = new System.Drawing.Point(151, 255);
            this.splitterLog.Name = "splitterLog";
            this.splitterLog.Size = new System.Drawing.Size(406, 3);
            this.splitterLog.TabIndex = 12;
            this.splitterLog.TabStop = false;
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnNew,
            this.btnOpen,
            this.btnSave,
            this.btnSaveAll,
            this.toolStripSeparator1,
            this.btnCut,
            this.btnCopy,
            this.btnPaste,
            this.toolStripSeparator2,
            this.btnRevert});
            this.toolStrip1.Location = new System.Drawing.Point(0, 24);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(734, 25);
            this.toolStrip1.TabIndex = 14;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // btnNew
            // 
            this.btnNew.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnNew.Image = ((System.Drawing.Image)(resources.GetObject("btnNew.Image")));
            this.btnNew.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnNew.Name = "btnNew";
            this.btnNew.Size = new System.Drawing.Size(23, 22);
            this.btnNew.Text = "New";
            this.btnNew.Click += new System.EventHandler(this.menuNew_Click);
            // 
            // btnOpen
            // 
            this.btnOpen.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnOpen.Image = ((System.Drawing.Image)(resources.GetObject("btnOpen.Image")));
            this.btnOpen.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnOpen.Name = "btnOpen";
            this.btnOpen.Size = new System.Drawing.Size(23, 22);
            this.btnOpen.Text = "Open Folder";
            this.btnOpen.Click += new System.EventHandler(this.menuOpenDir_Click);
            // 
            // btnSave
            // 
            this.btnSave.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnSave.Image = ((System.Drawing.Image)(resources.GetObject("btnSave.Image")));
            this.btnSave.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(23, 22);
            this.btnSave.Text = "Save";
            this.btnSave.Click += new System.EventHandler(this.menuSave_Click);
            // 
            // btnSaveAll
            // 
            this.btnSaveAll.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnSaveAll.Image = ((System.Drawing.Image)(resources.GetObject("btnSaveAll.Image")));
            this.btnSaveAll.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnSaveAll.Name = "btnSaveAll";
            this.btnSaveAll.Size = new System.Drawing.Size(23, 22);
            this.btnSaveAll.Text = "Save All";
            this.btnSaveAll.Click += new System.EventHandler(this.menuSaveAll_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // btnCut
            // 
            this.btnCut.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnCut.Image = ((System.Drawing.Image)(resources.GetObject("btnCut.Image")));
            this.btnCut.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnCut.Name = "btnCut";
            this.btnCut.Size = new System.Drawing.Size(23, 22);
            this.btnCut.Text = "Cut";
            this.btnCut.Click += new System.EventHandler(this.menuCut_Click);
            // 
            // btnCopy
            // 
            this.btnCopy.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnCopy.Image = ((System.Drawing.Image)(resources.GetObject("btnCopy.Image")));
            this.btnCopy.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnCopy.Name = "btnCopy";
            this.btnCopy.Size = new System.Drawing.Size(23, 22);
            this.btnCopy.Text = "Copy";
            this.btnCopy.Click += new System.EventHandler(this.menuCopy_Click);
            // 
            // btnPaste
            // 
            this.btnPaste.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnPaste.Image = ((System.Drawing.Image)(resources.GetObject("btnPaste.Image")));
            this.btnPaste.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnPaste.Name = "btnPaste";
            this.btnPaste.Size = new System.Drawing.Size(23, 22);
            this.btnPaste.Text = "Paste";
            this.btnPaste.Click += new System.EventHandler(this.menuPaste_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // btnRevert
            // 
            this.btnRevert.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnRevert.Enabled = false;
            this.btnRevert.Image = ((System.Drawing.Image)(resources.GetObject("btnRevert.Image")));
            this.btnRevert.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnRevert.Name = "btnRevert";
            this.btnRevert.Size = new System.Drawing.Size(23, 22);
            this.btnRevert.Text = "Revert";
            this.btnRevert.Click += new System.EventHandler(this.btnRevert_Click);
            // 
            // FrmMain
            // 
            this.AllowDrop = true;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(734, 383);
            this.Controls.Add(this.splitterLog);
            this.Controls.Add(this.pnlLog);
            this.Controls.Add(this.btnCollapseLeft);
            this.Controls.Add(this.btnCollapseRight);
            this.Controls.Add(this.splitterRight);
            this.Controls.Add(this.splitterLeft);
            this.Controls.Add(this.grpSpecies);
            this.Controls.Add(this.grpFiles);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.IsMdiContainer = true;
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "FrmMain";
            this.Text = "SysCAD Reaction Editor";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.MdiChildActivate += new System.EventHandler(this.FrmMain_MdiChildActivate);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.grpFiles.ResumeLayout(false);
            this.grpSpecies.ResumeLayout(false);
            this.menuSpecieList.ResumeLayout(false);
            this.pnlFilter.ResumeLayout(false);
            this.pnlFilter.PerformLayout();
            this.menuDatabaseFile.ResumeLayout(false);
            this.pnlLog.ResumeLayout(false);
            this.pnlLogHeader.ResumeLayout(false);
            this.pnlLogHeader.PerformLayout();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.GroupBox grpFiles;
        private System.Windows.Forms.TreeView treeFiles;
        private System.Windows.Forms.GroupBox grpSpecies;
        private System.Windows.Forms.ListView lstSpecies;
        private System.Windows.Forms.Splitter splitterLeft;
        private System.Windows.Forms.ToolStripMenuItem menuFile;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem windowToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem menuNew;
        private System.Windows.Forms.ToolStripMenuItem menuOpen;
        private System.Windows.Forms.ToolStripMenuItem menuClose;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem menuSave;
        private System.Windows.Forms.ToolStripMenuItem menuSaveAs;
        private System.Windows.Forms.ToolStripMenuItem menuSaveAll;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem menuExit;
        private System.Windows.Forms.ColumnHeader chName;
        private System.Windows.Forms.ColumnHeader chSymbol;
        private System.Windows.Forms.ToolStripMenuItem menuOpenDir;
        private System.Windows.Forms.ToolStripStatusLabel m_StatusLabel;
        private System.Windows.Forms.OpenFileDialog dlgOpenRxn;
        private System.Windows.Forms.SaveFileDialog dlgSaveRxn;
        private System.Windows.Forms.ToolStripMenuItem menuOpenDB;
        private System.Windows.Forms.ToolStripSeparator menuRecentFileSeperator;
        private System.Windows.Forms.OpenFileDialog dlgOpenDB;
        private System.Windows.Forms.ToolStripMenuItem arrangeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem undockToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aboutSysCADReactionEditorToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem menuCascade;
        public System.Windows.Forms.ToolStripMenuItem menuCut;
        public System.Windows.Forms.ToolStripMenuItem menuCopy;
        public System.Windows.Forms.ToolStripMenuItem menuPaste;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem3;
        private System.Windows.Forms.ContextMenuStrip menuDatabaseFile;
        private System.Windows.Forms.ToolStripMenuItem unloadAllSpeciesToolStripMenuItem;
        private System.Windows.Forms.FolderBrowserDialog dlgOpenFolder;
        private System.Windows.Forms.Panel pnlFilter;
        private System.Windows.Forms.TextBox txtFilter;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ContextMenuStrip menuSpecieList;
        private System.Windows.Forms.ToolStripMenuItem menuSortByPhase;
        private System.Windows.Forms.Splitter splitterRight;
        private System.Windows.Forms.Button btnCollapseRight;
        private System.Windows.Forms.Button btnCollapseLeft;
        private System.Windows.Forms.Panel pnlLog;
        private System.Windows.Forms.Panel pnlLogHeader;
        private System.Windows.Forms.Button btnLogCollapse;
        private System.Windows.Forms.Label lblLogHeader;
        private System.Windows.Forms.Splitter splitterLog;
        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.ListView lstLog;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem4;
        private System.Windows.Forms.ToolStripMenuItem menuSetSource;
        private System.Windows.Forms.ToolStripMenuItem menuSetSink;
        private System.Windows.Forms.ToolStripMenuItem menuRemoveSource;
        private System.Windows.Forms.ToolStripMenuItem menuRemoveSink;
        private System.Windows.Forms.ToolStripMenuItem menuSortAlphabetically;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton btnNew;
        private System.Windows.Forms.ToolStripButton btnOpen;
        private System.Windows.Forms.ToolStripButton btnSave;
        private System.Windows.Forms.ToolStripButton btnSaveAll;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton btnCut;
        private System.Windows.Forms.ToolStripButton btnCopy;
        private System.Windows.Forms.ToolStripButton btnPaste;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton btnRevert;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem5;
        private System.Windows.Forms.ToolStripMenuItem menuAutoCompleteSets;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripMenuItem menuRevert;


    }
}

