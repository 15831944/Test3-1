namespace Configuration_Editor
{
    partial class ProjectVectorControl
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
          this.components = new System.ComponentModel.Container();
          System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProjectVectorControl));
          this.grpProjectList = new System.Windows.Forms.GroupBox();
          this.pnlActiveList = new System.Windows.Forms.Panel();
          this.lstProjectVector = new System.Windows.Forms.ListView();
          this.chProjSymbol = new System.Windows.Forms.ColumnHeader();
          this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
          this.menuAdd = new System.Windows.Forms.ToolStripMenuItem();
          this.menuRemove = new System.Windows.Forms.ToolStripMenuItem();
          this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripSeparator();
          this.menuSort = new System.Windows.Forms.ToolStripMenuItem();
          this.pnlListButtons = new System.Windows.Forms.Panel();
          this.btnAdd = new System.Windows.Forms.Button();
          this.btnRemove = new System.Windows.Forms.Button();
          this.btnMoveUp = new System.Windows.Forms.Button();
          this.btnMoveDown = new System.Windows.Forms.Button();
          this.pnlActive = new System.Windows.Forms.Panel();
          this.tcIDE = new System.Windows.Forms.TabControl();
          this.tabIDESpecie = new System.Windows.Forms.TabPage();
          this.grpSpecie = new System.Windows.Forms.GroupBox();
          this.grpTempRange = new System.Windows.Forms.GroupBox();
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
          this.txtCalcDesc = new System.Windows.Forms.TextBox();
          this.label23 = new System.Windows.Forms.Label();
          this.txtCalculation = new Auto_Complete.BoxAutoComplete();
          this.tabIDELabel = new System.Windows.Forms.TabPage();
          this.grpTextValue = new System.Windows.Forms.GroupBox();
          this.txtText = new System.Windows.Forms.TextBox();
          this.tabIDEBlank = new System.Windows.Forms.TabPage();
          this.pnlBlank = new System.Windows.Forms.Panel();
          this.tabIDEAttribute = new System.Windows.Forms.TabPage();
          this.grpAttribute = new System.Windows.Forms.GroupBox();
          this.txtAttDimension = new System.Windows.Forms.TextBox();
          this.label50 = new System.Windows.Forms.Label();
          this.txtAttParent = new System.Windows.Forms.TextBox();
          this.label49 = new System.Windows.Forms.Label();
          this.comboAttributeType = new System.Windows.Forms.ComboBox();
          this.label8 = new System.Windows.Forms.Label();
          this.txtAttributeName = new System.Windows.Forms.TextBox();
          this.label7 = new System.Windows.Forms.Label();
          this.grpProjectList.SuspendLayout();
          this.pnlActiveList.SuspendLayout();
          this.contextMenuStrip1.SuspendLayout();
          this.pnlListButtons.SuspendLayout();
          this.pnlActive.SuspendLayout();
          this.tcIDE.SuspendLayout();
          this.tabIDESpecie.SuspendLayout();
          this.grpSpecie.SuspendLayout();
          this.grpTempRange.SuspendLayout();
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
          this.SuspendLayout();
          // 
          // grpProjectList
          // 
          this.grpProjectList.Controls.Add(this.pnlActiveList);
          this.grpProjectList.Controls.Add(this.pnlActive);
          this.grpProjectList.Dock = System.Windows.Forms.DockStyle.Fill;
          this.grpProjectList.Location = new System.Drawing.Point(0, 0);
          this.grpProjectList.Name = "grpProjectList";
          this.grpProjectList.Size = new System.Drawing.Size(467, 454);
          this.grpProjectList.TabIndex = 1;
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
          this.pnlActiveList.Size = new System.Drawing.Size(461, 276);
          this.pnlActiveList.TabIndex = 2;
          // 
          // lstProjectVector
          // 
          this.lstProjectVector.AllowDrop = true;
          this.lstProjectVector.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chProjSymbol});
          this.lstProjectVector.ContextMenuStrip = this.contextMenuStrip1;
          this.lstProjectVector.Dock = System.Windows.Forms.DockStyle.Fill;
          this.lstProjectVector.FullRowSelect = true;
          this.lstProjectVector.HideSelection = false;
          this.lstProjectVector.Location = new System.Drawing.Point(0, 0);
          this.lstProjectVector.Name = "lstProjectVector";
          this.lstProjectVector.Size = new System.Drawing.Size(457, 241);
          this.lstProjectVector.TabIndex = 0;
          this.lstProjectVector.UseCompatibleStateImageBehavior = false;
          this.lstProjectVector.View = System.Windows.Forms.View.Details;
          this.lstProjectVector.Resize += new System.EventHandler(this.lstProjectVector_Resize);
          this.lstProjectVector.SelectedIndexChanged += new System.EventHandler(this.lstProjectVector_SelectedIndexChanged);
          this.lstProjectVector.DragDrop += new System.Windows.Forms.DragEventHandler(this.lstProjectVector_DragDrop);
          this.lstProjectVector.DragEnter += new System.Windows.Forms.DragEventHandler(this.lstProjectVector_DragEnter);
          this.lstProjectVector.KeyDown += new System.Windows.Forms.KeyEventHandler(this.lstProjectVector_KeyDown);
          this.lstProjectVector.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.lstProjectVector_ItemDrag);
          this.lstProjectVector.DragOver += new System.Windows.Forms.DragEventHandler(this.lstProjectVector_DragOver);
          // 
          // chProjSymbol
          // 
          this.chProjSymbol.Text = "Symbol";
          this.chProjSymbol.Width = 442;
          // 
          // contextMenuStrip1
          // 
          this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuAdd,
            this.menuRemove,
            this.toolStripMenuItem1,
            this.menuSort});
          this.contextMenuStrip1.Name = "contextMenuStrip1";
          this.contextMenuStrip1.Size = new System.Drawing.Size(125, 76);
          this.contextMenuStrip1.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip1_Opening);
          // 
          // menuAdd
          // 
          this.menuAdd.Name = "menuAdd";
          this.menuAdd.Size = new System.Drawing.Size(124, 22);
          this.menuAdd.Text = "&Add...";
          this.menuAdd.Click += new System.EventHandler(this.menuAdd_Click);
          // 
          // menuRemove
          // 
          this.menuRemove.Name = "menuRemove";
          this.menuRemove.Size = new System.Drawing.Size(124, 22);
          this.menuRemove.Text = "&Remove";
          this.menuRemove.Click += new System.EventHandler(this.btnRemove_Click);
          // 
          // toolStripMenuItem1
          // 
          this.toolStripMenuItem1.Name = "toolStripMenuItem1";
          this.toolStripMenuItem1.Size = new System.Drawing.Size(121, 6);
          // 
          // menuSort
          // 
          this.menuSort.Name = "menuSort";
          this.menuSort.Size = new System.Drawing.Size(124, 22);
          this.menuSort.Text = "&Sort";
          this.menuSort.Click += new System.EventHandler(this.sortToolStripMenuItem_Click);
          // 
          // pnlListButtons
          // 
          this.pnlListButtons.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
          this.pnlListButtons.Controls.Add(this.btnAdd);
          this.pnlListButtons.Controls.Add(this.btnRemove);
          this.pnlListButtons.Controls.Add(this.btnMoveUp);
          this.pnlListButtons.Controls.Add(this.btnMoveDown);
          this.pnlListButtons.Dock = System.Windows.Forms.DockStyle.Bottom;
          this.pnlListButtons.Location = new System.Drawing.Point(0, 241);
          this.pnlListButtons.Name = "pnlListButtons";
          this.pnlListButtons.Size = new System.Drawing.Size(457, 31);
          this.pnlListButtons.TabIndex = 1;
          // 
          // btnAdd
          // 
          this.btnAdd.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
          this.btnAdd.Location = new System.Drawing.Point(136, 5);
          this.btnAdd.Name = "btnAdd";
          this.btnAdd.Size = new System.Drawing.Size(75, 23);
          this.btnAdd.TabIndex = 0;
          this.btnAdd.Text = "&Add";
          this.btnAdd.UseVisualStyleBackColor = true;
          this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
          // 
          // btnRemove
          // 
          this.btnRemove.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
          this.btnRemove.Enabled = false;
          this.btnRemove.Location = new System.Drawing.Point(217, 5);
          this.btnRemove.Name = "btnRemove";
          this.btnRemove.Size = new System.Drawing.Size(75, 23);
          this.btnRemove.TabIndex = 1;
          this.btnRemove.Text = "&Remove";
          this.btnRemove.UseVisualStyleBackColor = true;
          this.btnRemove.Click += new System.EventHandler(this.btnRemove_Click);
          // 
          // btnMoveUp
          // 
          this.btnMoveUp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
          this.btnMoveUp.Enabled = false;
          this.btnMoveUp.Font = new System.Drawing.Font("Symbol", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
          this.btnMoveUp.Location = new System.Drawing.Point(298, 5);
          this.btnMoveUp.Name = "btnMoveUp";
          this.btnMoveUp.Size = new System.Drawing.Size(75, 23);
          this.btnMoveUp.TabIndex = 2;
          this.btnMoveUp.Text = "­";
          this.btnMoveUp.UseVisualStyleBackColor = true;
          this.btnMoveUp.Click += new System.EventHandler(this.btnMoveUp_Click);
          // 
          // btnMoveDown
          // 
          this.btnMoveDown.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
          this.btnMoveDown.Enabled = false;
          this.btnMoveDown.Font = new System.Drawing.Font("Symbol", 8.25F);
          this.btnMoveDown.Location = new System.Drawing.Point(379, 5);
          this.btnMoveDown.Name = "btnMoveDown";
          this.btnMoveDown.Size = new System.Drawing.Size(75, 23);
          this.btnMoveDown.TabIndex = 3;
          this.btnMoveDown.Text = "¯";
          this.btnMoveDown.UseVisualStyleBackColor = true;
          this.btnMoveDown.Click += new System.EventHandler(this.btnMoveDown_Click);
          // 
          // pnlActive
          // 
          this.pnlActive.Controls.Add(this.tcIDE);
          this.pnlActive.Dock = System.Windows.Forms.DockStyle.Bottom;
          this.pnlActive.Location = new System.Drawing.Point(3, 292);
          this.pnlActive.Name = "pnlActive";
          this.pnlActive.Size = new System.Drawing.Size(461, 159);
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
          this.tcIDE.Size = new System.Drawing.Size(461, 159);
          this.tcIDE.TabIndex = 1;
          // 
          // tabIDESpecie
          // 
          this.tabIDESpecie.Controls.Add(this.grpSpecie);
          this.tabIDESpecie.Location = new System.Drawing.Point(4, 22);
          this.tabIDESpecie.Name = "tabIDESpecie";
          this.tabIDESpecie.Padding = new System.Windows.Forms.Padding(3);
          this.tabIDESpecie.Size = new System.Drawing.Size(453, 133);
          this.tabIDESpecie.TabIndex = 0;
          this.tabIDESpecie.Text = "Specie";
          this.tabIDESpecie.UseVisualStyleBackColor = true;
          // 
          // grpSpecie
          // 
          this.grpSpecie.Controls.Add(this.grpTempRange);
          this.grpSpecie.Controls.Add(this.chkIdeal);
          this.grpSpecie.Dock = System.Windows.Forms.DockStyle.Fill;
          this.grpSpecie.Location = new System.Drawing.Point(3, 3);
          this.grpSpecie.Name = "grpSpecie";
          this.grpSpecie.Size = new System.Drawing.Size(447, 127);
          this.grpSpecie.TabIndex = 1;
          this.grpSpecie.TabStop = false;
          this.grpSpecie.Text = "grpSpecie";
          // 
          // grpTempRange
          // 
          this.grpTempRange.Controls.Add(this.tableLayoutPanel1);
          this.grpTempRange.Location = new System.Drawing.Point(104, 19);
          this.grpTempRange.Name = "grpTempRange";
          this.grpTempRange.Size = new System.Drawing.Size(334, 100);
          this.grpTempRange.TabIndex = 1;
          this.grpTempRange.TabStop = false;
          this.grpTempRange.Text = "Temperature Range (°C)";
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
          this.numLowerTempTolerance.TabIndex = 1;
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
          this.numGreaterTempTolerance.TabIndex = 2;
          this.numGreaterTempTolerance.ValueChanged += new System.EventHandler(this.numGreaterTempTolerance_ValueChanged);
          // 
          // chkIdeal
          // 
          this.chkIdeal.Location = new System.Drawing.Point(6, 19);
          this.chkIdeal.Name = "chkIdeal";
          this.chkIdeal.Size = new System.Drawing.Size(92, 36);
          this.chkIdeal.TabIndex = 0;
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
          this.tabIDECalculation.Size = new System.Drawing.Size(453, 133);
          this.tabIDECalculation.TabIndex = 1;
          this.tabIDECalculation.Text = "Calculation";
          this.tabIDECalculation.UseVisualStyleBackColor = true;
          // 
          // grpCalculation
          // 
          this.grpCalculation.Controls.Add(this.txtCalcSymbol);
          this.grpCalculation.Controls.Add(this.label26);
          this.grpCalculation.Controls.Add(this.label24);
          this.grpCalculation.Controls.Add(this.txtCalcDesc);
          this.grpCalculation.Controls.Add(this.label23);
          this.grpCalculation.Controls.Add(this.txtCalculation);
          this.grpCalculation.Dock = System.Windows.Forms.DockStyle.Fill;
          this.grpCalculation.Location = new System.Drawing.Point(3, 3);
          this.grpCalculation.Name = "grpCalculation";
          this.grpCalculation.Size = new System.Drawing.Size(447, 127);
          this.grpCalculation.TabIndex = 1;
          this.grpCalculation.TabStop = false;
          this.grpCalculation.Text = "Calculation";
          // 
          // txtCalcSymbol
          // 
          this.txtCalcSymbol.Location = new System.Drawing.Point(77, 15);
          this.txtCalcSymbol.Name = "txtCalcSymbol";
          this.txtCalcSymbol.Size = new System.Drawing.Size(100, 20);
          this.txtCalcSymbol.TabIndex = 1;
          this.txtCalcSymbol.TextChanged += new System.EventHandler(this.txtCalcSymbol_TextChanged);
          this.txtCalcSymbol.Validated += new System.EventHandler(this.FireCheckCalculations);
          this.txtCalcSymbol.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PreventCommas);
          // 
          // label26
          // 
          this.label26.AutoSize = true;
          this.label26.Location = new System.Drawing.Point(30, 18);
          this.label26.Name = "label26";
          this.label26.Size = new System.Drawing.Size(41, 13);
          this.label26.TabIndex = 4;
          this.label26.Text = "Symbol";
          // 
          // label24
          // 
          this.label24.AutoSize = true;
          this.label24.Location = new System.Drawing.Point(9, 44);
          this.label24.Name = "label24";
          this.label24.Size = new System.Drawing.Size(62, 13);
          this.label24.TabIndex = 3;
          this.label24.Text = "Calculation:";
          // 
          // txtCalcDesc
          // 
          this.txtCalcDesc.Location = new System.Drawing.Point(249, 15);
          this.txtCalcDesc.Name = "txtCalcDesc";
          this.txtCalcDesc.Size = new System.Drawing.Size(100, 20);
          this.txtCalcDesc.TabIndex = 0;
          this.txtCalcDesc.TextChanged += new System.EventHandler(this.txtCalcDesc_TextChanged);
          this.txtCalcDesc.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PreventCommas);
          // 
          // label23
          // 
          this.label23.AutoSize = true;
          this.label23.Location = new System.Drawing.Point(217, 18);
          this.label23.Name = "label23";
          this.label23.Size = new System.Drawing.Size(26, 13);
          this.label23.TabIndex = 1;
          this.label23.Text = "Tag";
          // 
          // txtCalculation
          // 
          this.txtCalculation.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.txtCalculation.CompSelection = false;
          this.txtCalculation.Cursor = System.Windows.Forms.Cursors.IBeam;
          this.txtCalculation.HitCounts = ((System.Collections.Generic.Dictionary<object, int>)(resources.GetObject("txtCalculation.HitCounts")));
          this.txtCalculation.Location = new System.Drawing.Point(77, 41);
          this.txtCalculation.Name = "txtCalculation";
          this.txtCalculation.Size = new System.Drawing.Size(367, 83);
          this.txtCalculation.TabIndex = 2;
          this.txtCalculation.Text = "";
          this.txtCalculation.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PreventCommas);
          this.txtCalculation.TextChanged += new System.EventHandler(this.txtCalculation_TextChanged);
          // 
          // tabIDELabel
          // 
          this.tabIDELabel.Controls.Add(this.grpTextValue);
          this.tabIDELabel.Location = new System.Drawing.Point(4, 22);
          this.tabIDELabel.Name = "tabIDELabel";
          this.tabIDELabel.Padding = new System.Windows.Forms.Padding(3);
          this.tabIDELabel.Size = new System.Drawing.Size(453, 133);
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
          this.grpTextValue.Size = new System.Drawing.Size(447, 127);
          this.grpTextValue.TabIndex = 0;
          this.grpTextValue.TabStop = false;
          this.grpTextValue.Text = "Value";
          // 
          // txtText
          // 
          this.txtText.Dock = System.Windows.Forms.DockStyle.Fill;
          this.txtText.Location = new System.Drawing.Point(3, 16);
          this.txtText.Name = "txtText";
          this.txtText.Size = new System.Drawing.Size(441, 20);
          this.txtText.TabIndex = 0;
          this.txtText.TextChanged += new System.EventHandler(this.txtText_TextChanged);
          this.txtText.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PreventCommas);
          // 
          // tabIDEBlank
          // 
          this.tabIDEBlank.Controls.Add(this.pnlBlank);
          this.tabIDEBlank.Location = new System.Drawing.Point(4, 22);
          this.tabIDEBlank.Name = "tabIDEBlank";
          this.tabIDEBlank.Padding = new System.Windows.Forms.Padding(3);
          this.tabIDEBlank.Size = new System.Drawing.Size(453, 133);
          this.tabIDEBlank.TabIndex = 3;
          this.tabIDEBlank.Text = "Blank";
          this.tabIDEBlank.UseVisualStyleBackColor = true;
          // 
          // pnlBlank
          // 
          this.pnlBlank.Dock = System.Windows.Forms.DockStyle.Fill;
          this.pnlBlank.Location = new System.Drawing.Point(3, 3);
          this.pnlBlank.Name = "pnlBlank";
          this.pnlBlank.Size = new System.Drawing.Size(447, 127);
          this.pnlBlank.TabIndex = 0;
          // 
          // tabIDEAttribute
          // 
          this.tabIDEAttribute.Controls.Add(this.grpAttribute);
          this.tabIDEAttribute.Location = new System.Drawing.Point(4, 22);
          this.tabIDEAttribute.Name = "tabIDEAttribute";
          this.tabIDEAttribute.Padding = new System.Windows.Forms.Padding(3);
          this.tabIDEAttribute.Size = new System.Drawing.Size(453, 133);
          this.tabIDEAttribute.TabIndex = 4;
          this.tabIDEAttribute.Text = "Attribute";
          this.tabIDEAttribute.UseVisualStyleBackColor = true;
          // 
          // grpAttribute
          // 
          this.grpAttribute.Controls.Add(this.txtAttDimension);
          this.grpAttribute.Controls.Add(this.label50);
          this.grpAttribute.Controls.Add(this.txtAttParent);
          this.grpAttribute.Controls.Add(this.label49);
          this.grpAttribute.Controls.Add(this.comboAttributeType);
          this.grpAttribute.Controls.Add(this.label8);
          this.grpAttribute.Controls.Add(this.txtAttributeName);
          this.grpAttribute.Controls.Add(this.label7);
          this.grpAttribute.Dock = System.Windows.Forms.DockStyle.Fill;
          this.grpAttribute.Location = new System.Drawing.Point(3, 3);
          this.grpAttribute.Name = "grpAttribute";
          this.grpAttribute.Size = new System.Drawing.Size(447, 127);
          this.grpAttribute.TabIndex = 0;
          this.grpAttribute.TabStop = false;
          this.grpAttribute.Text = "Attribute Details";
          // 
          // txtAttDimension
          // 
          this.txtAttDimension.Location = new System.Drawing.Point(68, 48);
          this.txtAttDimension.Name = "txtAttDimension";
          this.txtAttDimension.Size = new System.Drawing.Size(121, 20);
          this.txtAttDimension.TabIndex = 6;
          this.txtAttDimension.TextChanged += new System.EventHandler(this.txtAttDimension_TextChanged);
          this.txtAttDimension.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PreventCommas);
          // 
          // label50
          // 
          this.label50.AutoSize = true;
          this.label50.Location = new System.Drawing.Point(6, 51);
          this.label50.Name = "label50";
          this.label50.Size = new System.Drawing.Size(56, 13);
          this.label50.TabIndex = 5;
          this.label50.Text = "Dimension";
          // 
          // txtAttParent
          // 
          this.txtAttParent.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.txtAttParent.Location = new System.Drawing.Point(249, 19);
          this.txtAttParent.Name = "txtAttParent";
          this.txtAttParent.Size = new System.Drawing.Size(121, 20);
          this.txtAttParent.TabIndex = 4;
          this.txtAttParent.TextChanged += new System.EventHandler(this.txtAttParent_TextChanged);
          this.txtAttParent.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PreventCommas);
          // 
          // label49
          // 
          this.label49.AutoSize = true;
          this.label49.Location = new System.Drawing.Point(205, 22);
          this.label49.Name = "label49";
          this.label49.Size = new System.Drawing.Size(38, 13);
          this.label49.TabIndex = 3;
          this.label49.Text = "Parent";
          // 
          // comboAttributeType
          // 
          this.comboAttributeType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.comboAttributeType.FormattingEnabled = true;
          this.comboAttributeType.Items.AddRange(new object[] {
            "Extensive",
            "Intensive"});
          this.comboAttributeType.Location = new System.Drawing.Point(249, 45);
          this.comboAttributeType.Name = "comboAttributeType";
          this.comboAttributeType.Size = new System.Drawing.Size(121, 21);
          this.comboAttributeType.TabIndex = 1;
          this.comboAttributeType.SelectedIndexChanged += new System.EventHandler(this.comboAttributeType_SelectedIndexChanged);
          // 
          // label8
          // 
          this.label8.AutoSize = true;
          this.label8.Location = new System.Drawing.Point(212, 48);
          this.label8.Name = "label8";
          this.label8.Size = new System.Drawing.Size(31, 13);
          this.label8.TabIndex = 2;
          this.label8.Text = "Type";
          // 
          // txtAttributeName
          // 
          this.txtAttributeName.Location = new System.Drawing.Point(68, 19);
          this.txtAttributeName.Name = "txtAttributeName";
          this.txtAttributeName.Size = new System.Drawing.Size(121, 20);
          this.txtAttributeName.TabIndex = 0;
          this.txtAttributeName.TextChanged += new System.EventHandler(this.txtAttributeName_TextChanged);
          this.txtAttributeName.Validated += new System.EventHandler(this.FireCheckCalculations);
          this.txtAttributeName.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.PreventCommas);
          // 
          // label7
          // 
          this.label7.AutoSize = true;
          this.label7.Location = new System.Drawing.Point(27, 22);
          this.label7.Name = "label7";
          this.label7.Size = new System.Drawing.Size(35, 13);
          this.label7.TabIndex = 0;
          this.label7.Text = "Name";
          // 
          // ProjectVectorControl
          // 
          this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
          this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
          this.Controls.Add(this.grpProjectList);
          this.Name = "ProjectVectorControl";
          this.Size = new System.Drawing.Size(467, 454);
          this.grpProjectList.ResumeLayout(false);
          this.pnlActiveList.ResumeLayout(false);
          this.contextMenuStrip1.ResumeLayout(false);
          this.pnlListButtons.ResumeLayout(false);
          this.pnlActive.ResumeLayout(false);
          this.tcIDE.ResumeLayout(false);
          this.tabIDESpecie.ResumeLayout(false);
          this.grpSpecie.ResumeLayout(false);
          this.grpTempRange.ResumeLayout(false);
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
          this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox grpProjectList;
        private System.Windows.Forms.Panel pnlActiveList;
        private System.Windows.Forms.ListView lstProjectVector;
      private System.Windows.Forms.ColumnHeader chProjSymbol;
        private System.Windows.Forms.Panel pnlListButtons;
        private System.Windows.Forms.Button btnAdd;
        private System.Windows.Forms.Button btnRemove;
        private System.Windows.Forms.Button btnMoveUp;
        private System.Windows.Forms.Button btnMoveDown;
        private System.Windows.Forms.Panel pnlActive;
        private System.Windows.Forms.TabControl tcIDE;
        private System.Windows.Forms.TabPage tabIDESpecie;
        private System.Windows.Forms.GroupBox grpSpecie;
        private System.Windows.Forms.GroupBox grpTempRange;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.NumericUpDown numLowerTempTolerance;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox txtDefinedMinTemp;
        private System.Windows.Forms.TextBox txtDefinedMaxTemp;
        private System.Windows.Forms.TextBox txtAllowedMaxTemp;
        private System.Windows.Forms.TextBox txtAllowedMinTemp;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.NumericUpDown numGreaterTempTolerance;
        private System.Windows.Forms.CheckBox chkIdeal;
        private System.Windows.Forms.TabPage tabIDECalculation;
        private System.Windows.Forms.GroupBox grpCalculation;
        private System.Windows.Forms.TextBox txtCalcSymbol;
        private System.Windows.Forms.Label label26;
        private System.Windows.Forms.Label label24;
        private System.Windows.Forms.TextBox txtCalcDesc;
        private System.Windows.Forms.Label label23;
        private Auto_Complete.BoxAutoComplete txtCalculation;
        private System.Windows.Forms.TabPage tabIDELabel;
        private System.Windows.Forms.GroupBox grpTextValue;
        private System.Windows.Forms.TextBox txtText;
        private System.Windows.Forms.TabPage tabIDEBlank;
        private System.Windows.Forms.Panel pnlBlank;
        private System.Windows.Forms.TabPage tabIDEAttribute;
        private System.Windows.Forms.GroupBox grpAttribute;
        private System.Windows.Forms.TextBox txtAttDimension;
        private System.Windows.Forms.Label label50;
        private System.Windows.Forms.TextBox txtAttParent;
        private System.Windows.Forms.Label label49;
        private System.Windows.Forms.ComboBox comboAttributeType;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox txtAttributeName;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem menuAdd;
        private System.Windows.Forms.ToolStripMenuItem menuRemove;
        private System.Windows.Forms.ToolStripSeparator toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem menuSort;
    }
}
