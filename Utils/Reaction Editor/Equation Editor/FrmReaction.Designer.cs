namespace Reaction_Editor
{
    partial class FrmReaction
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
          System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FrmReaction));
          this.tcMain = new System.Windows.Forms.TabControl();
          this.tabReactions = new System.Windows.Forms.TabPage();
          this.pnlList = new System.Windows.Forms.Panel();
          this.lstReactions = new System.Windows.Forms.ListView();
          this.clNumber = new System.Windows.Forms.ColumnHeader();
          this.clFormula = new System.Windows.Forms.ColumnHeader();
          this.clExtent = new System.Windows.Forms.ColumnHeader();
          this.clHOR = new System.Windows.Forms.ColumnHeader();
          this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
          this.menuClone = new System.Windows.Forms.ToolStripMenuItem();
          this.menuAddReaction = new System.Windows.Forms.ToolStripMenuItem();
          this.menuAddOther = new System.Windows.Forms.ToolStripMenuItem();
          this.menuAddSource = new System.Windows.Forms.ToolStripMenuItem();
          this.menuAddSink = new System.Windows.Forms.ToolStripMenuItem();
          this.menuAddHX = new System.Windows.Forms.ToolStripMenuItem();
          this.menuRemove = new System.Windows.Forms.ToolStripMenuItem();
          this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
          this.menuRevert = new System.Windows.Forms.ToolStripMenuItem();
          this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
          this.menuShowSequence = new System.Windows.Forms.ToolStripMenuItem();
          this.toolStripMenuItem2 = new System.Windows.Forms.ToolStripSeparator();
          this.menuCut = new System.Windows.Forms.ToolStripMenuItem();
          this.menuCopy = new System.Windows.Forms.ToolStripMenuItem();
          this.menuPaste = new System.Windows.Forms.ToolStripMenuItem();
          this.pnlButtons = new System.Windows.Forms.Panel();
          this.btnCopy = new System.Windows.Forms.Button();
          this.btnMoveDown = new System.Windows.Forms.Button();
          this.btnMoveUp = new System.Windows.Forms.Button();
          this.btnRemove = new System.Windows.Forms.Button();
          this.btnAdd = new System.Windows.Forms.Button();
          this.tcIDE = new System.Windows.Forms.TabControl();
          this.tabReaction = new System.Windows.Forms.TabPage();
          this.pnlReaction = new System.Windows.Forms.Panel();
          this.grpExtent = new System.Windows.Forms.GroupBox();
          this.comboBoxFinalFraction = new System.Windows.Forms.ComboBox();
          this.lblExtent4 = new System.Windows.Forms.Label();
          this.chkExtentOption = new System.Windows.Forms.CheckBox();
          this.lblExtent2 = new System.Windows.Forms.Label();
          this.lblExtent3 = new System.Windows.Forms.Label();
          this.numExtentVal3 = new Reaction_Editor.NumericTextbox();
          this.numExtentVal2 = new Reaction_Editor.NumericTextbox();
          this.lblExtentUnits = new System.Windows.Forms.Label();
          this.comboExtentSpecie = new System.Windows.Forms.ComboBox();
          this.lblExtentValue = new System.Windows.Forms.Label();
          this.numExtentValue = new Reaction_Editor.NumericTextbox();
          this.comboExtentType = new System.Windows.Forms.ComboBox();
          this.comboExtentSpecie2 = new System.Windows.Forms.ComboBox();
          this.btnBalance = new System.Windows.Forms.Button();
          this.txtReactants = new Auto_Complete.BoxAutoComplete();
          this.txtProducts = new Auto_Complete.BoxAutoComplete();
          this.grpHOR = new System.Windows.Forms.GroupBox();
          this.chkHOROverride = new System.Windows.Forms.CheckBox();
          this.lblHORPUnits = new System.Windows.Forms.Label();
          this.numHORP = new Reaction_Editor.NumericTextbox();
          this.lblHORP = new System.Windows.Forms.Label();
          this.lblHORT = new System.Windows.Forms.Label();
          this.numHORT = new Reaction_Editor.NumericTextbox();
          this.comboHORConditions = new System.Windows.Forms.ComboBox();
          this.label2 = new System.Windows.Forms.Label();
          this.comboHORUnits = new System.Windows.Forms.ComboBox();
          this.comboHORSpecie = new System.Windows.Forms.ComboBox();
          this.numHORValue = new Reaction_Editor.NumericTextbox();
          this.lblHORValue = new System.Windows.Forms.Label();
          this.txtFormula = new System.Windows.Forms.TextBox();
          this.comboDirection = new System.Windows.Forms.ComboBox();
          this.grpOther = new System.Windows.Forms.GroupBox();
          this.numSequence = new System.Windows.Forms.NumericUpDown();
          this.label3 = new System.Windows.Forms.Label();
          this.txtComment = new Reaction_Editor.TextBoxEx();
          this.chkEnabled = new System.Windows.Forms.CheckBox();
          this.label1 = new System.Windows.Forms.Label();
          this.tabSources = new System.Windows.Forms.TabPage();
          this.grpSources = new System.Windows.Forms.GroupBox();
          this.txtSources = new Auto_Complete.BoxAutoComplete();
          this.pnlSourceOptions = new System.Windows.Forms.Panel();
          this.label4 = new System.Windows.Forms.Label();
          this.txtSourceComments = new Reaction_Editor.TextBoxEx();
          this.chkSourcesEnabled = new System.Windows.Forms.CheckBox();
          this.tabSinks = new System.Windows.Forms.TabPage();
          this.grpSinks = new System.Windows.Forms.GroupBox();
          this.txtSinks = new Auto_Complete.BoxAutoComplete();
          this.pnlSinkOptions = new System.Windows.Forms.Panel();
          this.label5 = new System.Windows.Forms.Label();
          this.txtSinkComments = new Reaction_Editor.TextBoxEx();
          this.chkSinksEnabled = new System.Windows.Forms.CheckBox();
          this.tabHX = new System.Windows.Forms.TabPage();
          this.grpHX = new System.Windows.Forms.GroupBox();
          this.label6 = new System.Windows.Forms.Label();
          this.txtHXComment = new Reaction_Editor.TextBoxEx();
          this.chkHXEnabled = new System.Windows.Forms.CheckBox();
          this.lblHXPercent = new System.Windows.Forms.Label();
          this.numHXApproach = new System.Windows.Forms.NumericUpDown();
          this.lblHXApproach = new System.Windows.Forms.Label();
          this.lblHXUnits = new System.Windows.Forms.Label();
          this.numHX = new Reaction_Editor.NumericTextbox();
          this.lblHXValue = new System.Windows.Forms.Label();
          this.comboHXType = new System.Windows.Forms.ComboBox();
          this.tabBlank = new System.Windows.Forms.TabPage();
          this.pnlBlank = new System.Windows.Forms.Panel();
          this.tabOptions = new System.Windows.Forms.TabPage();
          this.grpDescription = new System.Windows.Forms.GroupBox();
          this.txtDescription = new Reaction_Editor.TextBoxEx();
          this.groupBox1 = new System.Windows.Forms.GroupBox();
          this.chkFirstReactant = new System.Windows.Forms.CheckBox();
          this.chkSequence = new System.Windows.Forms.CheckBox();
          this.statusStrip1 = new System.Windows.Forms.StatusStrip();
          this.statusLabel = new System.Windows.Forms.ToolStripStatusLabel();
          this.tcMain.SuspendLayout();
          this.tabReactions.SuspendLayout();
          this.pnlList.SuspendLayout();
          this.contextMenuStrip1.SuspendLayout();
          this.pnlButtons.SuspendLayout();
          this.tcIDE.SuspendLayout();
          this.tabReaction.SuspendLayout();
          this.pnlReaction.SuspendLayout();
          this.grpExtent.SuspendLayout();
          this.grpHOR.SuspendLayout();
          this.grpOther.SuspendLayout();
          ((System.ComponentModel.ISupportInitialize)(this.numSequence)).BeginInit();
          this.tabSources.SuspendLayout();
          this.grpSources.SuspendLayout();
          this.pnlSourceOptions.SuspendLayout();
          this.tabSinks.SuspendLayout();
          this.grpSinks.SuspendLayout();
          this.pnlSinkOptions.SuspendLayout();
          this.tabHX.SuspendLayout();
          this.grpHX.SuspendLayout();
          ((System.ComponentModel.ISupportInitialize)(this.numHXApproach)).BeginInit();
          this.tabBlank.SuspendLayout();
          this.tabOptions.SuspendLayout();
          this.grpDescription.SuspendLayout();
          this.groupBox1.SuspendLayout();
          this.statusStrip1.SuspendLayout();
          this.SuspendLayout();
          // 
          // tcMain
          // 
          this.tcMain.Controls.Add(this.tabReactions);
          this.tcMain.Controls.Add(this.tabOptions);
          this.tcMain.Dock = System.Windows.Forms.DockStyle.Fill;
          this.tcMain.Location = new System.Drawing.Point(4, 4);
          this.tcMain.Name = "tcMain";
          this.tcMain.SelectedIndex = 0;
          this.tcMain.Size = new System.Drawing.Size(852, 459);
          this.tcMain.TabIndex = 7;
          // 
          // tabReactions
          // 
          this.tabReactions.Controls.Add(this.pnlList);
          this.tabReactions.Controls.Add(this.tcIDE);
          this.tabReactions.Location = new System.Drawing.Point(4, 22);
          this.tabReactions.Name = "tabReactions";
          this.tabReactions.Padding = new System.Windows.Forms.Padding(3);
          this.tabReactions.Size = new System.Drawing.Size(844, 433);
          this.tabReactions.TabIndex = 0;
          this.tabReactions.Text = "Reactions";
          this.tabReactions.UseVisualStyleBackColor = true;
          // 
          // pnlList
          // 
          this.pnlList.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
          this.pnlList.Controls.Add(this.lstReactions);
          this.pnlList.Controls.Add(this.pnlButtons);
          this.pnlList.Dock = System.Windows.Forms.DockStyle.Fill;
          this.pnlList.Location = new System.Drawing.Point(3, 3);
          this.pnlList.Name = "pnlList";
          this.pnlList.Size = new System.Drawing.Size(838, 230);
          this.pnlList.TabIndex = 0;
          // 
          // lstReactions
          // 
          this.lstReactions.AllowDrop = true;
          this.lstReactions.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.clNumber,
            this.clFormula,
            this.clExtent,
            this.clHOR});
          this.lstReactions.ContextMenuStrip = this.contextMenuStrip1;
          this.lstReactions.Dock = System.Windows.Forms.DockStyle.Fill;
          this.lstReactions.FullRowSelect = true;
          this.lstReactions.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
          this.lstReactions.HideSelection = false;
          this.lstReactions.Location = new System.Drawing.Point(0, 0);
          this.lstReactions.Margin = new System.Windows.Forms.Padding(3, 3, 3, 5);
          this.lstReactions.MultiSelect = false;
          this.lstReactions.Name = "lstReactions";
          this.lstReactions.ShowGroups = false;
          this.lstReactions.Size = new System.Drawing.Size(834, 197);
          this.lstReactions.TabIndex = 0;
          this.lstReactions.UseCompatibleStateImageBehavior = false;
          this.lstReactions.View = System.Windows.Forms.View.Details;
          this.lstReactions.SelectedIndexChanged += new System.EventHandler(this.lstReactions_SelectedIndexChanged);
          this.lstReactions.DragDrop += new System.Windows.Forms.DragEventHandler(this.lstReactions_DragDrop);
          this.lstReactions.DragLeave += new System.EventHandler(this.lstReactions_DragLeave);
          this.lstReactions.KeyDown += new System.Windows.Forms.KeyEventHandler(this.lstReactions_KeyDown);
          this.lstReactions.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.lstReactions_ItemDrag);
          this.lstReactions.DragOver += new System.Windows.Forms.DragEventHandler(this.lstReactions_DragOver);
          // 
          // clNumber
          // 
          this.clNumber.Text = "#";
          this.clNumber.Width = 38;
          // 
          // clFormula
          // 
          this.clFormula.Text = "Formula";
          this.clFormula.Width = 405;
          // 
          // clExtent
          // 
          this.clExtent.Text = "Extent";
          this.clExtent.Width = 118;
          // 
          // clHOR
          // 
          this.clHOR.Text = "Heat Of Reaction";
          this.clHOR.Width = 137;
          // 
          // contextMenuStrip1
          // 
          this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuClone,
            this.menuAddReaction,
            this.menuAddOther,
            this.menuRemove,
            this.toolStripSeparator1,
            this.menuRevert,
            this.toolStripMenuItem1,
            this.menuShowSequence,
            this.toolStripMenuItem2,
            this.menuCut,
            this.menuCopy,
            this.menuPaste});
          this.contextMenuStrip1.Name = "contextMenuStrip1";
          this.contextMenuStrip1.Size = new System.Drawing.Size(199, 220);
          this.contextMenuStrip1.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip1_Opening);
          // 
          // menuClone
          // 
          this.menuClone.Name = "menuClone";
          this.menuClone.Size = new System.Drawing.Size(198, 22);
          this.menuClone.Text = "C&lone";
          this.menuClone.Click += new System.EventHandler(this.btnClone_Click);
          // 
          // menuAddReaction
          // 
          this.menuAddReaction.Name = "menuAddReaction";
          this.menuAddReaction.Size = new System.Drawing.Size(198, 22);
          this.menuAddReaction.Text = "&Add Reaction";
          this.menuAddReaction.Click += new System.EventHandler(this.DoAddReaction);
          // 
          // menuAddOther
          // 
          this.menuAddOther.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuAddSource,
            this.menuAddSink,
            this.menuAddHX});
          this.menuAddOther.Name = "menuAddOther";
          this.menuAddOther.Size = new System.Drawing.Size(198, 22);
          this.menuAddOther.Text = "Add &Other...";
          this.menuAddOther.DropDownOpening += new System.EventHandler(this.menuAddOther_DropDownOpening);
          // 
          // menuAddSource
          // 
          this.menuAddSource.Name = "menuAddSource";
          this.menuAddSource.Size = new System.Drawing.Size(158, 22);
          this.menuAddSource.Text = "S&ource";
          this.menuAddSource.Click += new System.EventHandler(this.DoAddSource);
          // 
          // menuAddSink
          // 
          this.menuAddSink.Name = "menuAddSink";
          this.menuAddSink.Size = new System.Drawing.Size(158, 22);
          this.menuAddSink.Text = "S&ink";
          this.menuAddSink.Click += new System.EventHandler(this.DoAddSink);
          // 
          // menuAddHX
          // 
          this.menuAddHX.Name = "menuAddHX";
          this.menuAddHX.Size = new System.Drawing.Size(158, 22);
          this.menuAddHX.Text = "&Heat Exchange";
          this.menuAddHX.Click += new System.EventHandler(this.DoAddHX);
          // 
          // menuRemove
          // 
          this.menuRemove.Name = "menuRemove";
          this.menuRemove.Size = new System.Drawing.Size(198, 22);
          this.menuRemove.Text = "&Remove";
          this.menuRemove.Click += new System.EventHandler(this.btnRemove_Click);
          // 
          // toolStripSeparator1
          // 
          this.toolStripSeparator1.Name = "toolStripSeparator1";
          this.toolStripSeparator1.Size = new System.Drawing.Size(195, 6);
          // 
          // menuRevert
          // 
          this.menuRevert.Name = "menuRevert";
          this.menuRevert.Size = new System.Drawing.Size(198, 22);
          this.menuRevert.Text = "Re&vert";
          this.menuRevert.Click += new System.EventHandler(this.menuRevert_Click);
          // 
          // toolStripMenuItem1
          // 
          this.toolStripMenuItem1.Name = "toolStripMenuItem1";
          this.toolStripMenuItem1.Size = new System.Drawing.Size(195, 6);
          // 
          // menuShowSequence
          // 
          this.menuShowSequence.CheckOnClick = true;
          this.menuShowSequence.Name = "menuShowSequence";
          this.menuShowSequence.Size = new System.Drawing.Size(198, 22);
          this.menuShowSequence.Text = "&Show Sequence Groups";
          this.menuShowSequence.CheckedChanged += new System.EventHandler(this.menuShowSequence_CheckedChanged);
          // 
          // toolStripMenuItem2
          // 
          this.toolStripMenuItem2.Name = "toolStripMenuItem2";
          this.toolStripMenuItem2.Size = new System.Drawing.Size(195, 6);
          // 
          // menuCut
          // 
          this.menuCut.Name = "menuCut";
          this.menuCut.Size = new System.Drawing.Size(198, 22);
          this.menuCut.Text = "Cut";
          this.menuCut.Click += new System.EventHandler(this.menuCut_Click);
          // 
          // menuCopy
          // 
          this.menuCopy.Name = "menuCopy";
          this.menuCopy.Size = new System.Drawing.Size(198, 22);
          this.menuCopy.Text = "Copy";
          this.menuCopy.Click += new System.EventHandler(this.menuCopy_Click);
          // 
          // menuPaste
          // 
          this.menuPaste.Name = "menuPaste";
          this.menuPaste.Size = new System.Drawing.Size(198, 22);
          this.menuPaste.Text = "Paste";
          this.menuPaste.Click += new System.EventHandler(this.menuPaste_Click);
          // 
          // pnlButtons
          // 
          this.pnlButtons.Controls.Add(this.btnCopy);
          this.pnlButtons.Controls.Add(this.btnMoveDown);
          this.pnlButtons.Controls.Add(this.btnMoveUp);
          this.pnlButtons.Controls.Add(this.btnRemove);
          this.pnlButtons.Controls.Add(this.btnAdd);
          this.pnlButtons.Dock = System.Windows.Forms.DockStyle.Bottom;
          this.pnlButtons.Location = new System.Drawing.Point(0, 197);
          this.pnlButtons.Name = "pnlButtons";
          this.pnlButtons.Size = new System.Drawing.Size(834, 29);
          this.pnlButtons.TabIndex = 1;
          // 
          // btnCopy
          // 
          this.btnCopy.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
          this.btnCopy.ForeColor = System.Drawing.SystemColors.ControlText;
          this.btnCopy.Location = new System.Drawing.Point(431, 3);
          this.btnCopy.Name = "btnCopy";
          this.btnCopy.Size = new System.Drawing.Size(75, 23);
          this.btnCopy.TabIndex = 1;
          this.btnCopy.Text = "C&lone";
          this.btnCopy.UseVisualStyleBackColor = true;
          this.btnCopy.Click += new System.EventHandler(this.btnClone_Click);
          // 
          // btnMoveDown
          // 
          this.btnMoveDown.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
          this.btnMoveDown.Font = new System.Drawing.Font("Symbol", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
          this.btnMoveDown.Location = new System.Drawing.Point(755, 3);
          this.btnMoveDown.Name = "btnMoveDown";
          this.btnMoveDown.Size = new System.Drawing.Size(75, 23);
          this.btnMoveDown.TabIndex = 5;
          this.btnMoveDown.Text = "¯";
          this.btnMoveDown.UseVisualStyleBackColor = true;
          this.btnMoveDown.Click += new System.EventHandler(this.btnMoveDown_Click);
          // 
          // btnMoveUp
          // 
          this.btnMoveUp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
          this.btnMoveUp.Font = new System.Drawing.Font("Symbol", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
          this.btnMoveUp.Location = new System.Drawing.Point(674, 3);
          this.btnMoveUp.Name = "btnMoveUp";
          this.btnMoveUp.Size = new System.Drawing.Size(75, 23);
          this.btnMoveUp.TabIndex = 4;
          this.btnMoveUp.Text = "­";
          this.btnMoveUp.UseVisualStyleBackColor = true;
          this.btnMoveUp.Click += new System.EventHandler(this.btnMoveUp_Click);
          // 
          // btnRemove
          // 
          this.btnRemove.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
          this.btnRemove.Location = new System.Drawing.Point(593, 3);
          this.btnRemove.Name = "btnRemove";
          this.btnRemove.Size = new System.Drawing.Size(75, 23);
          this.btnRemove.TabIndex = 3;
          this.btnRemove.Text = "&Remove";
          this.btnRemove.UseVisualStyleBackColor = true;
          this.btnRemove.Click += new System.EventHandler(this.btnRemove_Click);
          // 
          // btnAdd
          // 
          this.btnAdd.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
          this.btnAdd.ForeColor = System.Drawing.SystemColors.ControlText;
          this.btnAdd.Location = new System.Drawing.Point(512, 3);
          this.btnAdd.Name = "btnAdd";
          this.btnAdd.Size = new System.Drawing.Size(75, 23);
          this.btnAdd.TabIndex = 2;
          this.btnAdd.Text = "&Add";
          this.btnAdd.UseVisualStyleBackColor = true;
          this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
          // 
          // tcIDE
          // 
          this.tcIDE.Controls.Add(this.tabReaction);
          this.tcIDE.Controls.Add(this.tabSources);
          this.tcIDE.Controls.Add(this.tabSinks);
          this.tcIDE.Controls.Add(this.tabHX);
          this.tcIDE.Controls.Add(this.tabBlank);
          this.tcIDE.Dock = System.Windows.Forms.DockStyle.Bottom;
          this.tcIDE.Location = new System.Drawing.Point(3, 233);
          this.tcIDE.Name = "tcIDE";
          this.tcIDE.SelectedIndex = 0;
          this.tcIDE.Size = new System.Drawing.Size(838, 197);
          this.tcIDE.TabIndex = 6;
          this.tcIDE.TabStop = false;
          // 
          // tabReaction
          // 
          this.tabReaction.Controls.Add(this.pnlReaction);
          this.tabReaction.Location = new System.Drawing.Point(4, 22);
          this.tabReaction.Name = "tabReaction";
          this.tabReaction.Padding = new System.Windows.Forms.Padding(3);
          this.tabReaction.Size = new System.Drawing.Size(830, 171);
          this.tabReaction.TabIndex = 0;
          this.tabReaction.Text = "Reaction";
          // 
          // pnlReaction
          // 
          this.pnlReaction.AllowDrop = true;
          this.pnlReaction.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
          this.pnlReaction.Controls.Add(this.grpExtent);
          this.pnlReaction.Controls.Add(this.btnBalance);
          this.pnlReaction.Controls.Add(this.txtReactants);
          this.pnlReaction.Controls.Add(this.txtProducts);
          this.pnlReaction.Controls.Add(this.grpHOR);
          this.pnlReaction.Controls.Add(this.txtFormula);
          this.pnlReaction.Controls.Add(this.comboDirection);
          this.pnlReaction.Controls.Add(this.grpOther);
          this.pnlReaction.Dock = System.Windows.Forms.DockStyle.Fill;
          this.pnlReaction.Location = new System.Drawing.Point(3, 3);
          this.pnlReaction.Name = "pnlReaction";
          this.pnlReaction.Size = new System.Drawing.Size(824, 165);
          this.pnlReaction.TabIndex = 1;
          this.pnlReaction.Resize += new System.EventHandler(this.pnlReaction_Resize);
          this.pnlReaction.DragEnter += new System.Windows.Forms.DragEventHandler(this.pnlReaction_DragEnter);
          // 
          // grpExtent
          // 
          this.grpExtent.Controls.Add(this.comboBoxFinalFraction);
          this.grpExtent.Controls.Add(this.lblExtent4);
          this.grpExtent.Controls.Add(this.chkExtentOption);
          this.grpExtent.Controls.Add(this.lblExtent2);
          this.grpExtent.Controls.Add(this.lblExtent3);
          this.grpExtent.Controls.Add(this.numExtentVal3);
          this.grpExtent.Controls.Add(this.numExtentVal2);
          this.grpExtent.Controls.Add(this.lblExtentUnits);
          this.grpExtent.Controls.Add(this.comboExtentSpecie);
          this.grpExtent.Controls.Add(this.lblExtentValue);
          this.grpExtent.Controls.Add(this.numExtentValue);
          this.grpExtent.Controls.Add(this.comboExtentType);
          this.grpExtent.Controls.Add(this.comboExtentSpecie2);
          this.grpExtent.Dock = System.Windows.Forms.DockStyle.Bottom;
          this.grpExtent.Location = new System.Drawing.Point(0, 35);
          this.grpExtent.Name = "grpExtent";
          this.grpExtent.Size = new System.Drawing.Size(820, 42);
          this.grpExtent.TabIndex = 5;
          this.grpExtent.TabStop = false;
          this.grpExtent.Text = "Extent";
          // 
          // comboBoxFinalFraction
          // 
          this.comboBoxFinalFraction.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.comboBoxFinalFraction.Items.AddRange(new object[] {
            "Mass basis",
            "Molar basis"});
          this.comboBoxFinalFraction.Location = new System.Drawing.Point(227, 15);
          this.comboBoxFinalFraction.Name = "comboBoxFinalFraction";
          this.comboBoxFinalFraction.Size = new System.Drawing.Size(95, 21);
          this.comboBoxFinalFraction.TabIndex = 14;
          this.comboBoxFinalFraction.Visible = false;
          this.comboBoxFinalFraction.SelectedIndexChanged += new System.EventHandler(this.comboBoxFinalFraction_SelectedIndexChanged);
          // 
          // lblExtent4
          // 
          this.lblExtent4.AutoSize = true;
          this.lblExtent4.Location = new System.Drawing.Point(806, 18);
          this.lblExtent4.Name = "lblExtent4";
          this.lblExtent4.Size = new System.Drawing.Size(10, 13);
          this.lblExtent4.TabIndex = 13;
          this.lblExtent4.Text = ")";
          this.lblExtent4.Visible = false;
          // 
          // chkExtentOption
          // 
          this.chkExtentOption.AutoSize = true;
          this.chkExtentOption.Location = new System.Drawing.Point(523, 17);
          this.chkExtentOption.Name = "chkExtentOption";
          this.chkExtentOption.Size = new System.Drawing.Size(71, 17);
          this.chkExtentOption.TabIndex = 3;
          this.chkExtentOption.Text = "Stabilised";
          this.chkExtentOption.UseVisualStyleBackColor = true;
          this.chkExtentOption.Visible = false;
          this.chkExtentOption.CheckedChanged += new System.EventHandler(this.chkExtentOption_CheckedChanged);
          // 
          // lblExtent2
          // 
          this.lblExtent2.AutoSize = true;
          this.lblExtent2.Location = new System.Drawing.Point(607, 18);
          this.lblExtent2.Name = "lblExtent2";
          this.lblExtent2.Size = new System.Drawing.Size(46, 13);
          this.lblExtent2.TabIndex = 11;
          this.lblExtent2.Text = "(At Time";
          this.lblExtent2.Visible = false;
          // 
          // lblExtent3
          // 
          this.lblExtent3.AutoSize = true;
          this.lblExtent3.Location = new System.Drawing.Point(714, 18);
          this.lblExtent3.Name = "lblExtent3";
          this.lblExtent3.Size = new System.Drawing.Size(34, 13);
          this.lblExtent3.TabIndex = 10;
          this.lblExtent3.Text = "Val 1:";
          this.lblExtent3.Visible = false;
          // 
          // numExtentVal3
          // 
          this.numExtentVal3.Location = new System.Drawing.Point(751, 15);
          this.numExtentVal3.Name = "numExtentVal3";
          this.numExtentVal3.Size = new System.Drawing.Size(52, 20);
          this.numExtentVal3.TabIndex = 5;
          this.numExtentVal3.Visible = false;
          this.numExtentVal3.TextChanged += new System.EventHandler(this.numExtentVal3_TextChanged);
          // 
          // numExtentVal2
          // 
          this.numExtentVal2.Location = new System.Drawing.Point(659, 15);
          this.numExtentVal2.Name = "numExtentVal2";
          this.numExtentVal2.Size = new System.Drawing.Size(49, 20);
          this.numExtentVal2.TabIndex = 4;
          this.numExtentVal2.Visible = false;
          this.numExtentVal2.TextChanged += new System.EventHandler(this.numExtentVal2_TextChanged);
          // 
          // lblExtentUnits
          // 
          this.lblExtentUnits.Location = new System.Drawing.Point(317, 18);
          this.lblExtentUnits.Name = "lblExtentUnits";
          this.lblExtentUnits.Size = new System.Drawing.Size(51, 16);
          this.lblExtentUnits.TabIndex = 6;
          this.lblExtentUnits.TextAlign = System.Drawing.ContentAlignment.TopCenter;
          // 
          // comboExtentSpecie
          // 
          this.comboExtentSpecie.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.comboExtentSpecie.Enabled = false;
          this.comboExtentSpecie.FormattingEnabled = true;
          this.comboExtentSpecie.Location = new System.Drawing.Point(368, 15);
          this.comboExtentSpecie.Name = "comboExtentSpecie";
          this.comboExtentSpecie.Size = new System.Drawing.Size(149, 21);
          this.comboExtentSpecie.TabIndex = 2;
          this.comboExtentSpecie.SelectedIndexChanged += new System.EventHandler(this.comboExtentSpecie_SelectedIndexChanged);
          // 
          // lblExtentValue
          // 
          this.lblExtentValue.AutoSize = true;
          this.lblExtentValue.Location = new System.Drawing.Point(137, 18);
          this.lblExtentValue.Name = "lblExtentValue";
          this.lblExtentValue.Size = new System.Drawing.Size(34, 13);
          this.lblExtentValue.TabIndex = 3;
          this.lblExtentValue.Text = "Value";
          // 
          // numExtentValue
          // 
          this.numExtentValue.Location = new System.Drawing.Point(177, 15);
          this.numExtentValue.Name = "numExtentValue";
          this.numExtentValue.Size = new System.Drawing.Size(44, 20);
          this.numExtentValue.TabIndex = 1;
          this.numExtentValue.TextChanged += new System.EventHandler(this.numExtentValue_TextChanged);
          // 
          // comboExtentType
          // 
          this.comboExtentType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.comboExtentType.FormattingEnabled = true;
          this.comboExtentType.Items.AddRange(new object[] {
            "Fraction",
            "Ratio",
            "Equilibrium",
            "Final Conc.",
            "Final Frac."});
          this.comboExtentType.Location = new System.Drawing.Point(10, 15);
          this.comboExtentType.Name = "comboExtentType";
          this.comboExtentType.Size = new System.Drawing.Size(121, 21);
          this.comboExtentType.TabIndex = 0;
          this.comboExtentType.SelectedIndexChanged += new System.EventHandler(this.comboExtentType_SelectedIndexChanged);
          // 
          // comboExtentSpecie2
          // 
          this.comboExtentSpecie2.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.comboExtentSpecie2.Enabled = false;
          this.comboExtentSpecie2.FormattingEnabled = true;
          this.comboExtentSpecie2.Location = new System.Drawing.Point(523, 15);
          this.comboExtentSpecie2.Name = "comboExtentSpecie2";
          this.comboExtentSpecie2.Size = new System.Drawing.Size(78, 21);
          this.comboExtentSpecie2.TabIndex = 12;
          this.comboExtentSpecie2.Visible = false;
          this.comboExtentSpecie2.SelectedIndexChanged += new System.EventHandler(this.comboExtentSpecie2_SelectedIndexChanged);
          // 
          // btnBalance
          // 
          this.btnBalance.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
          this.btnBalance.ForeColor = System.Drawing.SystemColors.ControlText;
          this.btnBalance.Location = new System.Drawing.Point(742, 3);
          this.btnBalance.Name = "btnBalance";
          this.btnBalance.Size = new System.Drawing.Size(75, 23);
          this.btnBalance.TabIndex = 3;
          this.btnBalance.Text = "&Balance";
          this.btnBalance.UseVisualStyleBackColor = true;
          this.btnBalance.Click += new System.EventHandler(this.btnBalance_Click);
          // 
          // txtReactants
          // 
          this.txtReactants.AcceptsTab = true;
          this.txtReactants.AllowDrop = true;
          this.txtReactants.CompSelection = true;
          this.txtReactants.DisallowedCharaceters = null;
          this.txtReactants.ForeColor = System.Drawing.SystemColors.WindowText;
          this.txtReactants.HitCounts = ((System.Collections.Generic.Dictionary<object, int>)(resources.GetObject("txtReactants.HitCounts")));
          this.txtReactants.Location = new System.Drawing.Point(5, 5);
          this.txtReactants.Multiline = false;
          this.txtReactants.Name = "txtReactants";
          this.txtReactants.Size = new System.Drawing.Size(307, 20);
          this.txtReactants.TabIndex = 0;
          this.txtReactants.Text = "";
          this.txtReactants.DragDrop += new System.Windows.Forms.DragEventHandler(this.txtReactants_DragDrop);
          this.txtReactants.CompoundDragged += new System.EventHandler(this.txtFormula_CompoundDragged);
          this.txtReactants.Enter += new System.EventHandler(this.FormulaBox_Enter);
          this.txtReactants.Leave += new System.EventHandler(this.FormulaBox_Leave);
          this.txtReactants.DragEnter += new System.Windows.Forms.DragEventHandler(this.txtReactants_DragEnter);
          this.txtReactants.TextChanged += new System.EventHandler(this.txtFormula_TextChanged);
          this.txtReactants.PreCompSelect += new System.ComponentModel.CancelEventHandler(this.txtReactants_PreCompSelect);
          // 
          // txtProducts
          // 
          this.txtProducts.AcceptsTab = true;
          this.txtProducts.AllowDrop = true;
          this.txtProducts.CompSelection = true;
          this.txtProducts.DisallowedCharaceters = null;
          this.txtProducts.ForeColor = System.Drawing.SystemColors.WindowText;
          this.txtProducts.HitCounts = ((System.Collections.Generic.Dictionary<object, int>)(resources.GetObject("txtProducts.HitCounts")));
          this.txtProducts.Location = new System.Drawing.Point(368, 5);
          this.txtProducts.Multiline = false;
          this.txtProducts.Name = "txtProducts";
          this.txtProducts.Size = new System.Drawing.Size(290, 20);
          this.txtProducts.TabIndex = 2;
          this.txtProducts.Text = "";
          this.txtProducts.DragDrop += new System.Windows.Forms.DragEventHandler(this.txtProducts_DragDrop);
          this.txtProducts.CompoundDragged += new System.EventHandler(this.txtFormula_CompoundDragged);
          this.txtProducts.Enter += new System.EventHandler(this.FormulaBox_Enter);
          this.txtProducts.Leave += new System.EventHandler(this.FormulaBox_Leave);
          this.txtProducts.DragEnter += new System.Windows.Forms.DragEventHandler(this.txtProducts_DragEnter);
          this.txtProducts.TextChanged += new System.EventHandler(this.txtFormula_TextChanged);
          this.txtProducts.PreCompSelect += new System.ComponentModel.CancelEventHandler(this.txtProducts_PreCompSelect);
          // 
          // grpHOR
          // 
          this.grpHOR.Controls.Add(this.chkHOROverride);
          this.grpHOR.Controls.Add(this.lblHORPUnits);
          this.grpHOR.Controls.Add(this.numHORP);
          this.grpHOR.Controls.Add(this.lblHORP);
          this.grpHOR.Controls.Add(this.lblHORT);
          this.grpHOR.Controls.Add(this.numHORT);
          this.grpHOR.Controls.Add(this.comboHORConditions);
          this.grpHOR.Controls.Add(this.label2);
          this.grpHOR.Controls.Add(this.comboHORUnits);
          this.grpHOR.Controls.Add(this.comboHORSpecie);
          this.grpHOR.Controls.Add(this.numHORValue);
          this.grpHOR.Controls.Add(this.lblHORValue);
          this.grpHOR.Dock = System.Windows.Forms.DockStyle.Bottom;
          this.grpHOR.Location = new System.Drawing.Point(0, 77);
          this.grpHOR.Name = "grpHOR";
          this.grpHOR.Size = new System.Drawing.Size(820, 42);
          this.grpHOR.TabIndex = 6;
          this.grpHOR.TabStop = false;
          this.grpHOR.Text = "Heat Of Reaction";
          // 
          // chkHOROverride
          // 
          this.chkHOROverride.AutoSize = true;
          this.chkHOROverride.Location = new System.Drawing.Point(10, 17);
          this.chkHOROverride.Name = "chkHOROverride";
          this.chkHOROverride.Size = new System.Drawing.Size(66, 17);
          this.chkHOROverride.TabIndex = 19;
          this.chkHOROverride.Text = "O&verride";
          this.chkHOROverride.UseVisualStyleBackColor = true;
          this.chkHOROverride.CheckedChanged += new System.EventHandler(this.chkHOROverride_SelectedIndexChanged);
          // 
          // lblHORPUnits
          // 
          this.lblHORPUnits.AutoSize = true;
          this.lblHORPUnits.Location = new System.Drawing.Point(711, 18);
          this.lblHORPUnits.Name = "lblHORPUnits";
          this.lblHORPUnits.Size = new System.Drawing.Size(26, 13);
          this.lblHORPUnits.TabIndex = 18;
          this.lblHORPUnits.Text = "kPa";
          this.lblHORPUnits.Visible = false;
          // 
          // numHORP
          // 
          this.numHORP.Location = new System.Drawing.Point(663, 15);
          this.numHORP.Name = "numHORP";
          this.numHORP.Size = new System.Drawing.Size(48, 20);
          this.numHORP.TabIndex = 6;
          this.numHORP.Visible = false;
          this.numHORP.TextChanged += new System.EventHandler(this.numHORP_TextChanged);
          // 
          // lblHORP
          // 
          this.lblHORP.AutoSize = true;
          this.lblHORP.Location = new System.Drawing.Point(626, 18);
          this.lblHORP.Name = "lblHORP";
          this.lblHORP.Size = new System.Drawing.Size(37, 13);
          this.lblHORP.TabIndex = 16;
          this.lblHORP.Text = "°C, P=";
          this.lblHORP.Visible = false;
          // 
          // lblHORT
          // 
          this.lblHORT.AutoSize = true;
          this.lblHORT.Location = new System.Drawing.Point(558, 18);
          this.lblHORT.Name = "lblHORT";
          this.lblHORT.Size = new System.Drawing.Size(20, 13);
          this.lblHORT.TabIndex = 15;
          this.lblHORT.Text = "T=";
          this.lblHORT.Visible = false;
          // 
          // numHORT
          // 
          this.numHORT.Location = new System.Drawing.Point(578, 15);
          this.numHORT.Name = "numHORT";
          this.numHORT.Size = new System.Drawing.Size(48, 20);
          this.numHORT.TabIndex = 5;
          this.numHORT.Visible = false;
          this.numHORT.TextChanged += new System.EventHandler(this.numHORT_TextChanged);
          // 
          // comboHORConditions
          // 
          this.comboHORConditions.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.comboHORConditions.Enabled = false;
          this.comboHORConditions.FormattingEnabled = true;
          this.comboHORConditions.Location = new System.Drawing.Point(474, 15);
          this.comboHORConditions.Name = "comboHORConditions";
          this.comboHORConditions.Size = new System.Drawing.Size(78, 21);
          this.comboHORConditions.TabIndex = 4;
          this.comboHORConditions.SelectedIndexChanged += new System.EventHandler(this.comboHORConditions_SelectedIndexChanged);
          // 
          // label2
          // 
          this.label2.AutoSize = true;
          this.label2.Location = new System.Drawing.Point(451, 18);
          this.label2.Name = "label2";
          this.label2.Size = new System.Drawing.Size(17, 13);
          this.label2.TabIndex = 7;
          this.label2.Text = "At";
          // 
          // comboHORUnits
          // 
          this.comboHORUnits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.comboHORUnits.FormattingEnabled = true;
          this.comboHORUnits.Items.AddRange(new object[] {
            "kJ/mol",
            "kJ/kg"});
          this.comboHORUnits.Location = new System.Drawing.Point(230, 15);
          this.comboHORUnits.Name = "comboHORUnits";
          this.comboHORUnits.Size = new System.Drawing.Size(58, 21);
          this.comboHORUnits.TabIndex = 2;
          this.comboHORUnits.SelectedIndexChanged += new System.EventHandler(this.comboHORUnits_SelectedIndexChanged);
          // 
          // comboHORSpecie
          // 
          this.comboHORSpecie.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.comboHORSpecie.FormattingEnabled = true;
          this.comboHORSpecie.Location = new System.Drawing.Point(294, 15);
          this.comboHORSpecie.Name = "comboHORSpecie";
          this.comboHORSpecie.Size = new System.Drawing.Size(151, 21);
          this.comboHORSpecie.TabIndex = 3;
          this.comboHORSpecie.SelectedIndexChanged += new System.EventHandler(this.comboHORSpecie_SelectedIndexChanged);
          // 
          // numHORValue
          // 
          this.numHORValue.Location = new System.Drawing.Point(177, 15);
          this.numHORValue.Name = "numHORValue";
          this.numHORValue.Size = new System.Drawing.Size(44, 20);
          this.numHORValue.TabIndex = 1;
          this.numHORValue.TextChanged += new System.EventHandler(this.numHORValue_TextChanged);
          // 
          // lblHORValue
          // 
          this.lblHORValue.AutoSize = true;
          this.lblHORValue.Location = new System.Drawing.Point(137, 18);
          this.lblHORValue.Name = "lblHORValue";
          this.lblHORValue.Size = new System.Drawing.Size(34, 13);
          this.lblHORValue.TabIndex = 2;
          this.lblHORValue.Text = "Value";
          // 
          // txtFormula
          // 
          this.txtFormula.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.txtFormula.Location = new System.Drawing.Point(5, 5);
          this.txtFormula.Name = "txtFormula";
          this.txtFormula.Size = new System.Drawing.Size(793, 20);
          this.txtFormula.TabIndex = 4;
          this.txtFormula.Visible = false;
          this.txtFormula.Leave += new System.EventHandler(this.txtFormula_Leave);
          // 
          // comboDirection
          // 
          this.comboDirection.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.comboDirection.FormattingEnabled = true;
          this.comboDirection.Items.AddRange(new object[] {
            "->",
            "=",
            "<-"});
          this.comboDirection.Location = new System.Drawing.Point(318, 5);
          this.comboDirection.Name = "comboDirection";
          this.comboDirection.Size = new System.Drawing.Size(44, 21);
          this.comboDirection.TabIndex = 1;
          this.comboDirection.SelectedIndexChanged += new System.EventHandler(this.comboDirection_SelectedIndexChanged);
          // 
          // grpOther
          // 
          this.grpOther.Controls.Add(this.numSequence);
          this.grpOther.Controls.Add(this.label3);
          this.grpOther.Controls.Add(this.txtComment);
          this.grpOther.Controls.Add(this.chkEnabled);
          this.grpOther.Controls.Add(this.label1);
          this.grpOther.Dock = System.Windows.Forms.DockStyle.Bottom;
          this.grpOther.Location = new System.Drawing.Point(0, 119);
          this.grpOther.Name = "grpOther";
          this.grpOther.Size = new System.Drawing.Size(820, 42);
          this.grpOther.TabIndex = 7;
          this.grpOther.TabStop = false;
          // 
          // numSequence
          // 
          this.numSequence.Enabled = false;
          this.numSequence.Location = new System.Drawing.Point(177, 17);
          this.numSequence.Maximum = new decimal(new int[] {
            32,
            0,
            0,
            0});
          this.numSequence.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
          this.numSequence.Name = "numSequence";
          this.numSequence.Size = new System.Drawing.Size(44, 20);
          this.numSequence.TabIndex = 1;
          this.numSequence.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
          this.numSequence.ValueChanged += new System.EventHandler(this.numSequence_TextChanged);
          // 
          // label3
          // 
          this.label3.AutoSize = true;
          this.label3.Location = new System.Drawing.Point(227, 19);
          this.label3.Name = "label3";
          this.label3.Size = new System.Drawing.Size(51, 13);
          this.label3.TabIndex = 9;
          this.label3.Text = "Comment";
          // 
          // txtComment
          // 
          this.txtComment.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.txtComment.Location = new System.Drawing.Point(284, 16);
          this.txtComment.Name = "txtComment";
          this.txtComment.Size = new System.Drawing.Size(530, 20);
          this.txtComment.TabIndex = 2;
          this.txtComment.TextChanged += new System.EventHandler(this.txtComment_TextChanged);
          // 
          // chkEnabled
          // 
          this.chkEnabled.AutoSize = true;
          this.chkEnabled.Location = new System.Drawing.Point(10, 18);
          this.chkEnabled.Name = "chkEnabled";
          this.chkEnabled.Size = new System.Drawing.Size(65, 17);
          this.chkEnabled.TabIndex = 0;
          this.chkEnabled.Text = "&Enabled";
          this.chkEnabled.UseVisualStyleBackColor = true;
          this.chkEnabled.CheckedChanged += new System.EventHandler(this.chkEnabled_CheckedChanged);
          // 
          // label1
          // 
          this.label1.AutoSize = true;
          this.label1.Location = new System.Drawing.Point(115, 19);
          this.label1.Name = "label1";
          this.label1.Size = new System.Drawing.Size(56, 13);
          this.label1.TabIndex = 5;
          this.label1.Text = "Sequence";
          // 
          // tabSources
          // 
          this.tabSources.Controls.Add(this.grpSources);
          this.tabSources.Location = new System.Drawing.Point(4, 22);
          this.tabSources.Name = "tabSources";
          this.tabSources.Padding = new System.Windows.Forms.Padding(3);
          this.tabSources.Size = new System.Drawing.Size(830, 171);
          this.tabSources.TabIndex = 1;
          this.tabSources.Text = "Sources";
          this.tabSources.UseVisualStyleBackColor = true;
          // 
          // grpSources
          // 
          this.grpSources.Controls.Add(this.txtSources);
          this.grpSources.Controls.Add(this.pnlSourceOptions);
          this.grpSources.Dock = System.Windows.Forms.DockStyle.Fill;
          this.grpSources.Location = new System.Drawing.Point(3, 3);
          this.grpSources.Name = "grpSources";
          this.grpSources.Size = new System.Drawing.Size(824, 165);
          this.grpSources.TabIndex = 3;
          this.grpSources.TabStop = false;
          this.grpSources.Text = "Sources";
          // 
          // txtSources
          // 
          this.txtSources.CompSelection = false;
          this.txtSources.Cursor = System.Windows.Forms.Cursors.IBeam;
          this.txtSources.DetectUrls = false;
          this.txtSources.DisallowedCharaceters = null;
          this.txtSources.Dock = System.Windows.Forms.DockStyle.Fill;
          this.txtSources.HitCounts = ((System.Collections.Generic.Dictionary<object, int>)(resources.GetObject("txtSources.HitCounts")));
          this.txtSources.Location = new System.Drawing.Point(3, 16);
          this.txtSources.Multiline = false;
          this.txtSources.Name = "txtSources";
          this.txtSources.Size = new System.Drawing.Size(818, 117);
          this.txtSources.TabIndex = 1;
          this.txtSources.Text = "";
          this.txtSources.Leave += new System.EventHandler(this.txtSources_Leave);
          this.txtSources.TextChanged += new System.EventHandler(this.txtSources_TextChanged);
          // 
          // pnlSourceOptions
          // 
          this.pnlSourceOptions.Controls.Add(this.label4);
          this.pnlSourceOptions.Controls.Add(this.txtSourceComments);
          this.pnlSourceOptions.Controls.Add(this.chkSourcesEnabled);
          this.pnlSourceOptions.Dock = System.Windows.Forms.DockStyle.Bottom;
          this.pnlSourceOptions.Location = new System.Drawing.Point(3, 133);
          this.pnlSourceOptions.Name = "pnlSourceOptions";
          this.pnlSourceOptions.Size = new System.Drawing.Size(818, 29);
          this.pnlSourceOptions.TabIndex = 2;
          // 
          // label4
          // 
          this.label4.AutoSize = true;
          this.label4.Location = new System.Drawing.Point(228, 9);
          this.label4.Name = "label4";
          this.label4.Size = new System.Drawing.Size(51, 13);
          this.label4.TabIndex = 12;
          this.label4.Text = "Comment";
          // 
          // txtSourceComments
          // 
          this.txtSourceComments.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.txtSourceComments.Location = new System.Drawing.Point(285, 6);
          this.txtSourceComments.Name = "txtSourceComments";
          this.txtSourceComments.Size = new System.Drawing.Size(533, 20);
          this.txtSourceComments.TabIndex = 11;
          this.txtSourceComments.TextChanged += new System.EventHandler(this.txtSourceComments_TextChanged);
          // 
          // chkSourcesEnabled
          // 
          this.chkSourcesEnabled.AutoSize = true;
          this.chkSourcesEnabled.Checked = true;
          this.chkSourcesEnabled.CheckState = System.Windows.Forms.CheckState.Checked;
          this.chkSourcesEnabled.Location = new System.Drawing.Point(3, 8);
          this.chkSourcesEnabled.Name = "chkSourcesEnabled";
          this.chkSourcesEnabled.Size = new System.Drawing.Size(65, 17);
          this.chkSourcesEnabled.TabIndex = 10;
          this.chkSourcesEnabled.Text = "&Enabled";
          this.chkSourcesEnabled.UseVisualStyleBackColor = true;
          this.chkSourcesEnabled.CheckedChanged += new System.EventHandler(this.chkSourcesEnabled_CheckedChanged);
          // 
          // tabSinks
          // 
          this.tabSinks.Controls.Add(this.grpSinks);
          this.tabSinks.Location = new System.Drawing.Point(4, 22);
          this.tabSinks.Name = "tabSinks";
          this.tabSinks.Padding = new System.Windows.Forms.Padding(3);
          this.tabSinks.Size = new System.Drawing.Size(830, 171);
          this.tabSinks.TabIndex = 2;
          this.tabSinks.Text = "Sinks";
          this.tabSinks.UseVisualStyleBackColor = true;
          // 
          // grpSinks
          // 
          this.grpSinks.Controls.Add(this.txtSinks);
          this.grpSinks.Controls.Add(this.pnlSinkOptions);
          this.grpSinks.Dock = System.Windows.Forms.DockStyle.Fill;
          this.grpSinks.Location = new System.Drawing.Point(3, 3);
          this.grpSinks.Name = "grpSinks";
          this.grpSinks.Size = new System.Drawing.Size(824, 165);
          this.grpSinks.TabIndex = 4;
          this.grpSinks.TabStop = false;
          this.grpSinks.Text = "Sinks";
          // 
          // txtSinks
          // 
          this.txtSinks.CompSelection = false;
          this.txtSinks.Cursor = System.Windows.Forms.Cursors.IBeam;
          this.txtSinks.DetectUrls = false;
          this.txtSinks.DisallowedCharaceters = null;
          this.txtSinks.Dock = System.Windows.Forms.DockStyle.Fill;
          this.txtSinks.HitCounts = ((System.Collections.Generic.Dictionary<object, int>)(resources.GetObject("txtSinks.HitCounts")));
          this.txtSinks.Location = new System.Drawing.Point(3, 16);
          this.txtSinks.Multiline = false;
          this.txtSinks.Name = "txtSinks";
          this.txtSinks.Size = new System.Drawing.Size(818, 117);
          this.txtSinks.TabIndex = 2;
          this.txtSinks.Text = "";
          this.txtSinks.Leave += new System.EventHandler(this.txtSinks_Leave);
          this.txtSinks.TextChanged += new System.EventHandler(this.txtSinks_TextChanged);
          // 
          // pnlSinkOptions
          // 
          this.pnlSinkOptions.Controls.Add(this.label5);
          this.pnlSinkOptions.Controls.Add(this.txtSinkComments);
          this.pnlSinkOptions.Controls.Add(this.chkSinksEnabled);
          this.pnlSinkOptions.Dock = System.Windows.Forms.DockStyle.Bottom;
          this.pnlSinkOptions.Location = new System.Drawing.Point(3, 133);
          this.pnlSinkOptions.Name = "pnlSinkOptions";
          this.pnlSinkOptions.Size = new System.Drawing.Size(818, 29);
          this.pnlSinkOptions.TabIndex = 3;
          // 
          // label5
          // 
          this.label5.AutoSize = true;
          this.label5.Location = new System.Drawing.Point(228, 9);
          this.label5.Name = "label5";
          this.label5.Size = new System.Drawing.Size(51, 13);
          this.label5.TabIndex = 12;
          this.label5.Text = "Comment";
          // 
          // txtSinkComments
          // 
          this.txtSinkComments.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.txtSinkComments.Location = new System.Drawing.Point(285, 6);
          this.txtSinkComments.Name = "txtSinkComments";
          this.txtSinkComments.Size = new System.Drawing.Size(530, 20);
          this.txtSinkComments.TabIndex = 11;
          this.txtSinkComments.TextChanged += new System.EventHandler(this.txtSinkComments_TextChanged);
          // 
          // chkSinksEnabled
          // 
          this.chkSinksEnabled.AutoSize = true;
          this.chkSinksEnabled.Checked = true;
          this.chkSinksEnabled.CheckState = System.Windows.Forms.CheckState.Checked;
          this.chkSinksEnabled.Location = new System.Drawing.Point(3, 8);
          this.chkSinksEnabled.Name = "chkSinksEnabled";
          this.chkSinksEnabled.Size = new System.Drawing.Size(65, 17);
          this.chkSinksEnabled.TabIndex = 10;
          this.chkSinksEnabled.Text = "&Enabled";
          this.chkSinksEnabled.UseVisualStyleBackColor = true;
          this.chkSinksEnabled.CheckedChanged += new System.EventHandler(this.chkSinksEnabled_CheckedChanged);
          // 
          // tabHX
          // 
          this.tabHX.Controls.Add(this.grpHX);
          this.tabHX.Location = new System.Drawing.Point(4, 22);
          this.tabHX.Name = "tabHX";
          this.tabHX.Padding = new System.Windows.Forms.Padding(3);
          this.tabHX.Size = new System.Drawing.Size(830, 171);
          this.tabHX.TabIndex = 3;
          this.tabHX.Text = "HX";
          this.tabHX.UseVisualStyleBackColor = true;
          // 
          // grpHX
          // 
          this.grpHX.Controls.Add(this.label6);
          this.grpHX.Controls.Add(this.txtHXComment);
          this.grpHX.Controls.Add(this.chkHXEnabled);
          this.grpHX.Controls.Add(this.lblHXPercent);
          this.grpHX.Controls.Add(this.numHXApproach);
          this.grpHX.Controls.Add(this.lblHXApproach);
          this.grpHX.Controls.Add(this.lblHXUnits);
          this.grpHX.Controls.Add(this.numHX);
          this.grpHX.Controls.Add(this.lblHXValue);
          this.grpHX.Controls.Add(this.comboHXType);
          this.grpHX.Dock = System.Windows.Forms.DockStyle.Top;
          this.grpHX.Location = new System.Drawing.Point(3, 3);
          this.grpHX.Name = "grpHX";
          this.grpHX.Size = new System.Drawing.Size(824, 165);
          this.grpHX.TabIndex = 1;
          this.grpHX.TabStop = false;
          this.grpHX.Text = "Heat Exchange";
          // 
          // label6
          // 
          this.label6.AutoSize = true;
          this.label6.Location = new System.Drawing.Point(231, 142);
          this.label6.Name = "label6";
          this.label6.Size = new System.Drawing.Size(51, 13);
          this.label6.TabIndex = 15;
          this.label6.Text = "Comment";
          // 
          // txtHXComment
          // 
          this.txtHXComment.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.txtHXComment.Location = new System.Drawing.Point(288, 139);
          this.txtHXComment.Name = "txtHXComment";
          this.txtHXComment.Size = new System.Drawing.Size(530, 20);
          this.txtHXComment.TabIndex = 14;
          this.txtHXComment.TextChanged += new System.EventHandler(this.txtHXComment_TextChanged);
          // 
          // chkHXEnabled
          // 
          this.chkHXEnabled.AutoSize = true;
          this.chkHXEnabled.Checked = true;
          this.chkHXEnabled.CheckState = System.Windows.Forms.CheckState.Checked;
          this.chkHXEnabled.Location = new System.Drawing.Point(6, 141);
          this.chkHXEnabled.Name = "chkHXEnabled";
          this.chkHXEnabled.Size = new System.Drawing.Size(65, 17);
          this.chkHXEnabled.TabIndex = 13;
          this.chkHXEnabled.Text = "&Enabled";
          this.chkHXEnabled.UseVisualStyleBackColor = true;
          this.chkHXEnabled.CheckedChanged += new System.EventHandler(this.chkHXEnabled_CheckedChanged);
          // 
          // lblHXPercent
          // 
          this.lblHXPercent.AutoSize = true;
          this.lblHXPercent.Location = new System.Drawing.Point(469, 18);
          this.lblHXPercent.Name = "lblHXPercent";
          this.lblHXPercent.Size = new System.Drawing.Size(15, 13);
          this.lblHXPercent.TabIndex = 11;
          this.lblHXPercent.Text = "%";
          this.lblHXPercent.Visible = false;
          // 
          // numHXApproach
          // 
          this.numHXApproach.DecimalPlaces = 1;
          this.numHXApproach.Location = new System.Drawing.Point(394, 15);
          this.numHXApproach.Name = "numHXApproach";
          this.numHXApproach.Size = new System.Drawing.Size(69, 20);
          this.numHXApproach.TabIndex = 2;
          this.numHXApproach.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
          this.numHXApproach.Visible = false;
          this.numHXApproach.ValueChanged += new System.EventHandler(this.numHXApproach_ValueChanged);
          // 
          // lblHXApproach
          // 
          this.lblHXApproach.AutoSize = true;
          this.lblHXApproach.Location = new System.Drawing.Point(335, 18);
          this.lblHXApproach.Name = "lblHXApproach";
          this.lblHXApproach.Size = new System.Drawing.Size(53, 13);
          this.lblHXApproach.TabIndex = 9;
          this.lblHXApproach.Text = "Approach";
          this.lblHXApproach.Visible = false;
          // 
          // lblHXUnits
          // 
          this.lblHXUnits.AutoSize = true;
          this.lblHXUnits.Location = new System.Drawing.Point(294, 18);
          this.lblHXUnits.Name = "lblHXUnits";
          this.lblHXUnits.Size = new System.Drawing.Size(35, 13);
          this.lblHXUnits.TabIndex = 8;
          this.lblHXUnits.Text = "degC,";
          this.lblHXUnits.Visible = false;
          // 
          // numHX
          // 
          this.numHX.Location = new System.Drawing.Point(210, 15);
          this.numHX.Name = "numHX";
          this.numHX.Size = new System.Drawing.Size(78, 20);
          this.numHX.TabIndex = 1;
          this.numHX.Visible = false;
          this.numHX.TextChanged += new System.EventHandler(this.numHX_TextChanged);
          // 
          // lblHXValue
          // 
          this.lblHXValue.Location = new System.Drawing.Point(137, 18);
          this.lblHXValue.Name = "lblHXValue";
          this.lblHXValue.Size = new System.Drawing.Size(67, 15);
          this.lblHXValue.TabIndex = 5;
          this.lblHXValue.Text = "Value";
          this.lblHXValue.TextAlign = System.Drawing.ContentAlignment.TopRight;
          this.lblHXValue.Visible = false;
          // 
          // comboHXType
          // 
          this.comboHXType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.comboHXType.FormattingEnabled = true;
          this.comboHXType.Items.AddRange(new object[] {
            "Final Temp.",
            "Approach Temp.",
            "Approach Ambient",
            "Power",
            "Electrolysis"});
          this.comboHXType.Location = new System.Drawing.Point(6, 15);
          this.comboHXType.Name = "comboHXType";
          this.comboHXType.Size = new System.Drawing.Size(121, 21);
          this.comboHXType.TabIndex = 0;
          this.comboHXType.SelectedIndexChanged += new System.EventHandler(this.comboHXType_SelectedIndexChanged);
          // 
          // tabBlank
          // 
          this.tabBlank.Controls.Add(this.pnlBlank);
          this.tabBlank.Location = new System.Drawing.Point(4, 22);
          this.tabBlank.Name = "tabBlank";
          this.tabBlank.Padding = new System.Windows.Forms.Padding(3);
          this.tabBlank.Size = new System.Drawing.Size(830, 171);
          this.tabBlank.TabIndex = 4;
          this.tabBlank.Text = "Blank";
          this.tabBlank.UseVisualStyleBackColor = true;
          // 
          // pnlBlank
          // 
          this.pnlBlank.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
          this.pnlBlank.Dock = System.Windows.Forms.DockStyle.Fill;
          this.pnlBlank.Location = new System.Drawing.Point(3, 3);
          this.pnlBlank.Name = "pnlBlank";
          this.pnlBlank.Size = new System.Drawing.Size(824, 165);
          this.pnlBlank.TabIndex = 0;
          // 
          // tabOptions
          // 
          this.tabOptions.Controls.Add(this.grpDescription);
          this.tabOptions.Controls.Add(this.groupBox1);
          this.tabOptions.Location = new System.Drawing.Point(4, 22);
          this.tabOptions.Name = "tabOptions";
          this.tabOptions.Padding = new System.Windows.Forms.Padding(3);
          this.tabOptions.Size = new System.Drawing.Size(844, 433);
          this.tabOptions.TabIndex = 2;
          this.tabOptions.Text = "Options";
          this.tabOptions.UseVisualStyleBackColor = true;
          // 
          // grpDescription
          // 
          this.grpDescription.Controls.Add(this.txtDescription);
          this.grpDescription.Dock = System.Windows.Forms.DockStyle.Fill;
          this.grpDescription.Location = new System.Drawing.Point(3, 70);
          this.grpDescription.Name = "grpDescription";
          this.grpDescription.Size = new System.Drawing.Size(838, 360);
          this.grpDescription.TabIndex = 2;
          this.grpDescription.TabStop = false;
          this.grpDescription.Text = "Description";
          // 
          // txtDescription
          // 
          this.txtDescription.Dock = System.Windows.Forms.DockStyle.Fill;
          this.txtDescription.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
          this.txtDescription.Location = new System.Drawing.Point(3, 16);
          this.txtDescription.Multiline = true;
          this.txtDescription.Name = "txtDescription";
          this.txtDescription.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
          this.txtDescription.Size = new System.Drawing.Size(832, 341);
          this.txtDescription.TabIndex = 0;
          this.txtDescription.TextChanged += new System.EventHandler(this.txtDescription_TextChanged);
          // 
          // groupBox1
          // 
          this.groupBox1.Controls.Add(this.chkFirstReactant);
          this.groupBox1.Controls.Add(this.chkSequence);
          this.groupBox1.Dock = System.Windows.Forms.DockStyle.Top;
          this.groupBox1.Location = new System.Drawing.Point(3, 3);
          this.groupBox1.Name = "groupBox1";
          this.groupBox1.Size = new System.Drawing.Size(838, 67);
          this.groupBox1.TabIndex = 1;
          this.groupBox1.TabStop = false;
          this.groupBox1.Text = "Miscellaneous";
          // 
          // chkFirstReactant
          // 
          this.chkFirstReactant.AutoSize = true;
          this.chkFirstReactant.Checked = true;
          this.chkFirstReactant.CheckState = System.Windows.Forms.CheckState.Checked;
          this.chkFirstReactant.Location = new System.Drawing.Point(10, 42);
          this.chkFirstReactant.Name = "chkFirstReactant";
          this.chkFirstReactant.Size = new System.Drawing.Size(198, 17);
          this.chkFirstReactant.TabIndex = 1;
          this.chkFirstReactant.Text = "Always Use First Reactant for Extent";
          this.chkFirstReactant.UseVisualStyleBackColor = true;
          this.chkFirstReactant.CheckedChanged += new System.EventHandler(this.chkFirstReactant_CheckedChanged);
          // 
          // chkSequence
          // 
          this.chkSequence.AutoSize = true;
          this.chkSequence.Location = new System.Drawing.Point(10, 19);
          this.chkSequence.Name = "chkSequence";
          this.chkSequence.Size = new System.Drawing.Size(122, 17);
          this.chkSequence.TabIndex = 0;
          this.chkSequence.Text = "Sequences Enabled";
          this.chkSequence.UseVisualStyleBackColor = true;
          this.chkSequence.CheckedChanged += new System.EventHandler(this.chkSequence_CheckedChanged);
          // 
          // statusStrip1
          // 
          this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.statusLabel});
          this.statusStrip1.Location = new System.Drawing.Point(4, 463);
          this.statusStrip1.Name = "statusStrip1";
          this.statusStrip1.Size = new System.Drawing.Size(852, 22);
          this.statusStrip1.TabIndex = 8;
          this.statusStrip1.Text = "statusStrip1";
          // 
          // statusLabel
          // 
          this.statusLabel.Name = "statusLabel";
          this.statusLabel.Size = new System.Drawing.Size(0, 17);
          // 
          // FrmReaction
          // 
          this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
          this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
          this.ClientSize = new System.Drawing.Size(860, 489);
          this.Controls.Add(this.tcMain);
          this.Controls.Add(this.statusStrip1);
          this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
          this.KeyPreview = true;
          this.MinimumSize = new System.Drawing.Size(571, 351);
          this.Name = "FrmReaction";
          this.Padding = new System.Windows.Forms.Padding(4);
          this.ShowInTaskbar = false;
          this.Text = "Untitled Reaction Block";
          this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.FrmReaction_FormClosed);
          this.tcMain.ResumeLayout(false);
          this.tabReactions.ResumeLayout(false);
          this.pnlList.ResumeLayout(false);
          this.contextMenuStrip1.ResumeLayout(false);
          this.pnlButtons.ResumeLayout(false);
          this.tcIDE.ResumeLayout(false);
          this.tabReaction.ResumeLayout(false);
          this.pnlReaction.ResumeLayout(false);
          this.pnlReaction.PerformLayout();
          this.grpExtent.ResumeLayout(false);
          this.grpExtent.PerformLayout();
          this.grpHOR.ResumeLayout(false);
          this.grpHOR.PerformLayout();
          this.grpOther.ResumeLayout(false);
          this.grpOther.PerformLayout();
          ((System.ComponentModel.ISupportInitialize)(this.numSequence)).EndInit();
          this.tabSources.ResumeLayout(false);
          this.grpSources.ResumeLayout(false);
          this.pnlSourceOptions.ResumeLayout(false);
          this.pnlSourceOptions.PerformLayout();
          this.tabSinks.ResumeLayout(false);
          this.grpSinks.ResumeLayout(false);
          this.pnlSinkOptions.ResumeLayout(false);
          this.pnlSinkOptions.PerformLayout();
          this.tabHX.ResumeLayout(false);
          this.grpHX.ResumeLayout(false);
          this.grpHX.PerformLayout();
          ((System.ComponentModel.ISupportInitialize)(this.numHXApproach)).EndInit();
          this.tabBlank.ResumeLayout(false);
          this.tabOptions.ResumeLayout(false);
          this.grpDescription.ResumeLayout(false);
          this.grpDescription.PerformLayout();
          this.groupBox1.ResumeLayout(false);
          this.groupBox1.PerformLayout();
          this.statusStrip1.ResumeLayout(false);
          this.statusStrip1.PerformLayout();
          this.ResumeLayout(false);
          this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TabControl tcMain;
        private System.Windows.Forms.TabPage tabReactions;
        private System.Windows.Forms.Panel pnlReaction;
        private Auto_Complete.BoxAutoComplete txtReactants;
        private Auto_Complete.BoxAutoComplete txtProducts;
        private System.Windows.Forms.ComboBox comboDirection;
        private System.Windows.Forms.TextBox txtFormula;
        private System.Windows.Forms.GroupBox grpExtent;
        private System.Windows.Forms.Label lblExtentValue;
        private Reaction_Editor.NumericTextbox numExtentValue;
        private System.Windows.Forms.ComboBox comboExtentType;
        private System.Windows.Forms.GroupBox grpHOR;
        private System.Windows.Forms.ComboBox comboHORSpecie;
        private Reaction_Editor.NumericTextbox numHORValue;
        private System.Windows.Forms.Label lblHORValue;
        private System.Windows.Forms.GroupBox grpOther;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Panel pnlList;
        private System.Windows.Forms.ListView lstReactions;
        private System.Windows.Forms.ColumnHeader clFormula;
        private System.Windows.Forms.ColumnHeader clExtent;
        private System.Windows.Forms.ColumnHeader clHOR;
        private System.Windows.Forms.Panel pnlButtons;
        private System.Windows.Forms.Button btnMoveDown;
        private System.Windows.Forms.Button btnMoveUp;
        private System.Windows.Forms.Button btnRemove;
        private System.Windows.Forms.Button btnAdd;
        private System.Windows.Forms.ComboBox comboExtentSpecie;
        private System.Windows.Forms.Label lblExtentUnits;
        private System.Windows.Forms.Label lblExtent2;
        private System.Windows.Forms.Label lblExtent3;
        private Reaction_Editor.NumericTextbox numExtentVal3;
        private Reaction_Editor.NumericTextbox numExtentVal2;
        private System.Windows.Forms.CheckBox chkExtentOption;
        private System.Windows.Forms.CheckBox chkEnabled;
        private System.Windows.Forms.Label label3;
        private Reaction_Editor.TextBoxEx txtComment;
        private System.Windows.Forms.NumericUpDown numSequence;
        private System.Windows.Forms.TabPage tabOptions;
        private System.Windows.Forms.GroupBox grpDescription;
        private System.Windows.Forms.CheckBox chkSequence;
        private System.Windows.Forms.GroupBox groupBox1;
        private Reaction_Editor.TextBoxEx txtDescription;
        private System.Windows.Forms.ComboBox comboHORUnits;
        private System.Windows.Forms.Button btnCopy;
        private System.Windows.Forms.CheckBox chkFirstReactant;
        private System.Windows.Forms.Label lblHORT;
        private Reaction_Editor.NumericTextbox numHORT;
        private System.Windows.Forms.ComboBox comboHORConditions;
        private System.Windows.Forms.Label label2;
        private Reaction_Editor.NumericTextbox numHORP;
        private System.Windows.Forms.Label lblHORP;
        private System.Windows.Forms.Label lblHORPUnits;
        private System.Windows.Forms.Button btnBalance;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem menuClone;
        private System.Windows.Forms.ToolStripMenuItem menuAddReaction;
        private System.Windows.Forms.ToolStripMenuItem menuRemove;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem menuShowSequence;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel statusLabel;
        private System.Windows.Forms.CheckBox chkHOROverride;
        private System.Windows.Forms.ColumnHeader clNumber;
        private System.Windows.Forms.GroupBox grpHX;
        private System.Windows.Forms.Label lblHXPercent;
        private System.Windows.Forms.NumericUpDown numHXApproach;
        private System.Windows.Forms.Label lblHXApproach;
        private System.Windows.Forms.Label lblHXUnits;
        private Reaction_Editor.NumericTextbox numHX;
        private System.Windows.Forms.Label lblHXValue;
        private System.Windows.Forms.ComboBox comboHXType;
        private Auto_Complete.BoxAutoComplete txtSinks;
        private Auto_Complete.BoxAutoComplete txtSources;
        private System.Windows.Forms.TabControl tcIDE;
        private System.Windows.Forms.TabPage tabReaction;
        private System.Windows.Forms.TabPage tabSources;
        private System.Windows.Forms.TabPage tabSinks;
        private System.Windows.Forms.TabPage tabHX;
        private System.Windows.Forms.TabPage tabBlank;
        private System.Windows.Forms.Panel pnlBlank;
        private System.Windows.Forms.Label label6;
        private Reaction_Editor.TextBoxEx txtHXComment;
        private System.Windows.Forms.CheckBox chkHXEnabled;
        private System.Windows.Forms.GroupBox grpSources;
        private System.Windows.Forms.Panel pnlSourceOptions;
        private System.Windows.Forms.Label label4;
        private Reaction_Editor.TextBoxEx txtSourceComments;
        private System.Windows.Forms.CheckBox chkSourcesEnabled;
        private System.Windows.Forms.GroupBox grpSinks;
        private System.Windows.Forms.Panel pnlSinkOptions;
        private System.Windows.Forms.Label label5;
        private Reaction_Editor.TextBoxEx txtSinkComments;
        private System.Windows.Forms.CheckBox chkSinksEnabled;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem menuRevert;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem2;
        private System.Windows.Forms.ToolStripMenuItem menuCut;
        private System.Windows.Forms.ToolStripMenuItem menuCopy;
        private System.Windows.Forms.ToolStripMenuItem menuPaste;
        private System.Windows.Forms.ComboBox comboExtentSpecie2;
        private System.Windows.Forms.Label lblExtent4;
        private System.Windows.Forms.ToolStripMenuItem menuAddOther;
        private System.Windows.Forms.ToolStripMenuItem menuAddSource;
        private System.Windows.Forms.ToolStripMenuItem menuAddSink;
        private System.Windows.Forms.ToolStripMenuItem menuAddHX;
      private System.Windows.Forms.ComboBox comboBoxFinalFraction;


    }
}