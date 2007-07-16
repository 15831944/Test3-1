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
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabReactions = new System.Windows.Forms.TabPage();
            this.pnlList = new System.Windows.Forms.Panel();
            this.lstReactions = new System.Windows.Forms.ListView();
            this.clFormula = new System.Windows.Forms.ColumnHeader();
            this.clExtent = new System.Windows.Forms.ColumnHeader();
            this.clHOR = new System.Windows.Forms.ColumnHeader();
            this.pnlButtons = new System.Windows.Forms.Panel();
            this.btnCopy = new System.Windows.Forms.Button();
            this.btnMoveDown = new System.Windows.Forms.Button();
            this.btnMoveUp = new System.Windows.Forms.Button();
            this.btnRemove = new System.Windows.Forms.Button();
            this.btnAdd = new System.Windows.Forms.Button();
            this.pnlReaction = new System.Windows.Forms.Panel();
            this.txtReactants = new System.Windows.Forms.TextBox();
            this.comboDirection = new System.Windows.Forms.ComboBox();
            this.txtProducts = new System.Windows.Forms.TextBox();
            this.txtFormula = new System.Windows.Forms.TextBox();
            this.grpExtent = new System.Windows.Forms.GroupBox();
            this.chkExtentStabilised = new System.Windows.Forms.CheckBox();
            this.lblExtent2 = new System.Windows.Forms.Label();
            this.lblExtent3 = new System.Windows.Forms.Label();
            this.numExtentVal3 = new CtrlLib.NumEdit();
            this.numExtentVal2 = new CtrlLib.NumEdit();
            this.comboExtentPhase = new System.Windows.Forms.ComboBox();
            this.lblExtentUnits = new System.Windows.Forms.Label();
            this.comboExtentSpecie = new System.Windows.Forms.ComboBox();
            this.lblExtentValue = new System.Windows.Forms.Label();
            this.numExtentValue = new CtrlLib.NumEdit();
            this.comboExtentType = new System.Windows.Forms.ComboBox();
            this.grpHOR = new System.Windows.Forms.GroupBox();
            this.comboHORUnits = new System.Windows.Forms.ComboBox();
            this.comboHORSpecie = new System.Windows.Forms.ComboBox();
            this.numHORValue = new CtrlLib.NumEdit();
            this.lblHORValue = new System.Windows.Forms.Label();
            this.comboHORType = new System.Windows.Forms.ComboBox();
            this.grpOther = new System.Windows.Forms.GroupBox();
            this.numSequence = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.txtComment = new System.Windows.Forms.TextBox();
            this.chkEnabled = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tabOptions = new System.Windows.Forms.TabPage();
            this.panel1 = new System.Windows.Forms.Panel();
            this.grpSources = new System.Windows.Forms.GroupBox();
            this.txtSources = new System.Windows.Forms.TextBox();
            this.grpSinks = new System.Windows.Forms.GroupBox();
            this.txtSinks = new System.Windows.Forms.TextBox();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.grpDescription = new System.Windows.Forms.GroupBox();
            this.txtDescription = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.chkSequence = new System.Windows.Forms.CheckBox();
            this.grpHX = new System.Windows.Forms.GroupBox();
            this.lblHXUnits = new System.Windows.Forms.Label();
            this.numHX = new CtrlLib.NumEdit();
            this.label2 = new System.Windows.Forms.Label();
            this.comboHXType = new System.Windows.Forms.ComboBox();
            this.tabControl1.SuspendLayout();
            this.tabReactions.SuspendLayout();
            this.pnlList.SuspendLayout();
            this.pnlButtons.SuspendLayout();
            this.pnlReaction.SuspendLayout();
            this.grpExtent.SuspendLayout();
            this.grpHOR.SuspendLayout();
            this.grpOther.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numSequence)).BeginInit();
            this.tabOptions.SuspendLayout();
            this.panel1.SuspendLayout();
            this.grpSources.SuspendLayout();
            this.grpSinks.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.grpDescription.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.grpHX.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabReactions);
            this.tabControl1.Controls.Add(this.tabOptions);
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(4, 4);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(695, 379);
            this.tabControl1.TabIndex = 7;
            // 
            // tabReactions
            // 
            this.tabReactions.Controls.Add(this.pnlList);
            this.tabReactions.Controls.Add(this.pnlReaction);
            this.tabReactions.Location = new System.Drawing.Point(4, 22);
            this.tabReactions.Name = "tabReactions";
            this.tabReactions.Padding = new System.Windows.Forms.Padding(3);
            this.tabReactions.Size = new System.Drawing.Size(687, 353);
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
            this.pnlList.Size = new System.Drawing.Size(681, 188);
            this.pnlList.TabIndex = 1;
            // 
            // lstReactions
            // 
            this.lstReactions.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.clFormula,
            this.clExtent,
            this.clHOR});
            this.lstReactions.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lstReactions.FullRowSelect = true;
            this.lstReactions.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.lstReactions.HideSelection = false;
            this.lstReactions.Location = new System.Drawing.Point(0, 0);
            this.lstReactions.Margin = new System.Windows.Forms.Padding(3, 3, 3, 5);
            this.lstReactions.MultiSelect = false;
            this.lstReactions.Name = "lstReactions";
            this.lstReactions.ShowGroups = false;
            this.lstReactions.Size = new System.Drawing.Size(677, 155);
            this.lstReactions.TabIndex = 0;
            this.lstReactions.UseCompatibleStateImageBehavior = false;
            this.lstReactions.View = System.Windows.Forms.View.Details;
            this.lstReactions.SelectedIndexChanged += new System.EventHandler(this.lstReactions_SelectedIndexChanged);
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
            // pnlButtons
            // 
            this.pnlButtons.Controls.Add(this.btnCopy);
            this.pnlButtons.Controls.Add(this.btnMoveDown);
            this.pnlButtons.Controls.Add(this.btnMoveUp);
            this.pnlButtons.Controls.Add(this.btnRemove);
            this.pnlButtons.Controls.Add(this.btnAdd);
            this.pnlButtons.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnlButtons.Location = new System.Drawing.Point(0, 155);
            this.pnlButtons.Name = "pnlButtons";
            this.pnlButtons.Size = new System.Drawing.Size(677, 29);
            this.pnlButtons.TabIndex = 1;
            // 
            // btnCopy
            // 
            this.btnCopy.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCopy.ForeColor = System.Drawing.SystemColors.ControlText;
            this.btnCopy.Location = new System.Drawing.Point(274, 3);
            this.btnCopy.Name = "btnCopy";
            this.btnCopy.Size = new System.Drawing.Size(75, 23);
            this.btnCopy.TabIndex = 4;
            this.btnCopy.Text = "Copy";
            this.btnCopy.UseVisualStyleBackColor = true;
            this.btnCopy.Click += new System.EventHandler(this.btnCopy_Click);
            // 
            // btnMoveDown
            // 
            this.btnMoveDown.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnMoveDown.Font = new System.Drawing.Font("Symbol", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.btnMoveDown.Location = new System.Drawing.Point(598, 3);
            this.btnMoveDown.Name = "btnMoveDown";
            this.btnMoveDown.Size = new System.Drawing.Size(75, 23);
            this.btnMoveDown.TabIndex = 3;
            this.btnMoveDown.Text = "¯";
            this.btnMoveDown.UseVisualStyleBackColor = true;
            this.btnMoveDown.Click += new System.EventHandler(this.btnMoveDown_Click);
            // 
            // btnMoveUp
            // 
            this.btnMoveUp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnMoveUp.Font = new System.Drawing.Font("Symbol", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.btnMoveUp.Location = new System.Drawing.Point(517, 3);
            this.btnMoveUp.Name = "btnMoveUp";
            this.btnMoveUp.Size = new System.Drawing.Size(75, 23);
            this.btnMoveUp.TabIndex = 2;
            this.btnMoveUp.Text = "­";
            this.btnMoveUp.UseVisualStyleBackColor = true;
            this.btnMoveUp.Click += new System.EventHandler(this.btnMoveUp_Click);
            // 
            // btnRemove
            // 
            this.btnRemove.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnRemove.Location = new System.Drawing.Point(436, 3);
            this.btnRemove.Name = "btnRemove";
            this.btnRemove.Size = new System.Drawing.Size(75, 23);
            this.btnRemove.TabIndex = 1;
            this.btnRemove.Text = "Remove";
            this.btnRemove.UseVisualStyleBackColor = true;
            // 
            // btnAdd
            // 
            this.btnAdd.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnAdd.ForeColor = System.Drawing.SystemColors.ControlText;
            this.btnAdd.Location = new System.Drawing.Point(355, 3);
            this.btnAdd.Name = "btnAdd";
            this.btnAdd.Size = new System.Drawing.Size(75, 23);
            this.btnAdd.TabIndex = 0;
            this.btnAdd.Text = "Add";
            this.btnAdd.UseVisualStyleBackColor = true;
            this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
            // 
            // pnlReaction
            // 
            this.pnlReaction.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.pnlReaction.Controls.Add(this.txtReactants);
            this.pnlReaction.Controls.Add(this.comboDirection);
            this.pnlReaction.Controls.Add(this.txtProducts);
            this.pnlReaction.Controls.Add(this.txtFormula);
            this.pnlReaction.Controls.Add(this.grpExtent);
            this.pnlReaction.Controls.Add(this.grpHOR);
            this.pnlReaction.Controls.Add(this.grpOther);
            this.pnlReaction.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnlReaction.Enabled = false;
            this.pnlReaction.Location = new System.Drawing.Point(3, 191);
            this.pnlReaction.Name = "pnlReaction";
            this.pnlReaction.Size = new System.Drawing.Size(681, 159);
            this.pnlReaction.TabIndex = 7;
            this.pnlReaction.Resize += new System.EventHandler(this.pnlReaction_Resize);
            // 
            // txtReactants
            // 
            this.txtReactants.AllowDrop = true;
            this.txtReactants.ForeColor = System.Drawing.SystemColors.GrayText;
            this.txtReactants.Location = new System.Drawing.Point(3, 5);
            this.txtReactants.Name = "txtReactants";
            this.txtReactants.Size = new System.Drawing.Size(307, 20);
            this.txtReactants.TabIndex = 0;
            this.txtReactants.Text = "Reactants";
            this.txtReactants.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.txtReactants.Enter += new System.EventHandler(this.FormulaBox_Enter);
            this.txtReactants.DragDrop += new System.Windows.Forms.DragEventHandler(this.txtReactants_DragDrop);
            this.txtReactants.Leave += new System.EventHandler(this.FormulaBox_Leave);
            this.txtReactants.DragEnter += new System.Windows.Forms.DragEventHandler(this.txtReactants_DragEnter);
            // 
            // comboDirection
            // 
            this.comboDirection.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboDirection.FormattingEnabled = true;
            this.comboDirection.Items.AddRange(new object[] {
            "->",
            "=",
            "<-"});
            this.comboDirection.Location = new System.Drawing.Point(316, 5);
            this.comboDirection.Name = "comboDirection";
            this.comboDirection.Size = new System.Drawing.Size(44, 21);
            this.comboDirection.TabIndex = 2;
            this.comboDirection.SelectedIndexChanged += new System.EventHandler(this.comboDirection_SelectedIndexChanged);
            // 
            // txtProducts
            // 
            this.txtProducts.AllowDrop = true;
            this.txtProducts.ForeColor = System.Drawing.SystemColors.GrayText;
            this.txtProducts.Location = new System.Drawing.Point(367, 5);
            this.txtProducts.Name = "txtProducts";
            this.txtProducts.Size = new System.Drawing.Size(307, 20);
            this.txtProducts.TabIndex = 1;
            this.txtProducts.Text = "Products";
            this.txtProducts.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.txtProducts.Enter += new System.EventHandler(this.FormulaBox_Enter);
            this.txtProducts.DragDrop += new System.Windows.Forms.DragEventHandler(this.txtProducts_DragDrop);
            this.txtProducts.Leave += new System.EventHandler(this.FormulaBox_Leave);
            this.txtProducts.DragEnter += new System.Windows.Forms.DragEventHandler(this.txtProducts_DragEnter);
            // 
            // txtFormula
            // 
            this.txtFormula.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtFormula.Location = new System.Drawing.Point(3, 5);
            this.txtFormula.Name = "txtFormula";
            this.txtFormula.Size = new System.Drawing.Size(671, 20);
            this.txtFormula.TabIndex = 5;
            this.txtFormula.Visible = false;
            this.txtFormula.Leave += new System.EventHandler(this.txtFormula_Leave);
            // 
            // grpExtent
            // 
            this.grpExtent.Controls.Add(this.chkExtentStabilised);
            this.grpExtent.Controls.Add(this.lblExtent2);
            this.grpExtent.Controls.Add(this.lblExtent3);
            this.grpExtent.Controls.Add(this.numExtentVal3);
            this.grpExtent.Controls.Add(this.numExtentVal2);
            this.grpExtent.Controls.Add(this.comboExtentPhase);
            this.grpExtent.Controls.Add(this.lblExtentUnits);
            this.grpExtent.Controls.Add(this.comboExtentSpecie);
            this.grpExtent.Controls.Add(this.lblExtentValue);
            this.grpExtent.Controls.Add(this.numExtentValue);
            this.grpExtent.Controls.Add(this.comboExtentType);
            this.grpExtent.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.grpExtent.Location = new System.Drawing.Point(0, 29);
            this.grpExtent.Name = "grpExtent";
            this.grpExtent.Size = new System.Drawing.Size(677, 42);
            this.grpExtent.TabIndex = 10;
            this.grpExtent.TabStop = false;
            this.grpExtent.Text = "Extent";
            // 
            // chkExtentStabilised
            // 
            this.chkExtentStabilised.AutoSize = true;
            this.chkExtentStabilised.Location = new System.Drawing.Point(402, 17);
            this.chkExtentStabilised.Name = "chkExtentStabilised";
            this.chkExtentStabilised.Size = new System.Drawing.Size(71, 17);
            this.chkExtentStabilised.TabIndex = 12;
            this.chkExtentStabilised.Text = "Stabilised";
            this.chkExtentStabilised.UseVisualStyleBackColor = true;
            this.chkExtentStabilised.Visible = false;
            this.chkExtentStabilised.CheckedChanged += new System.EventHandler(this.chkExtentStabilised_CheckedChanged);
            // 
            // lblExtent2
            // 
            this.lblExtent2.AutoSize = true;
            this.lblExtent2.Location = new System.Drawing.Point(402, 18);
            this.lblExtent2.Name = "lblExtent2";
            this.lblExtent2.Size = new System.Drawing.Size(43, 13);
            this.lblExtent2.TabIndex = 11;
            this.lblExtent2.Text = "At Time";
            this.lblExtent2.Visible = false;
            // 
            // lblExtent3
            // 
            this.lblExtent3.AutoSize = true;
            this.lblExtent3.Location = new System.Drawing.Point(535, 18);
            this.lblExtent3.Name = "lblExtent3";
            this.lblExtent3.Size = new System.Drawing.Size(34, 13);
            this.lblExtent3.TabIndex = 10;
            this.lblExtent3.Text = "Val 1:";
            this.lblExtent3.Visible = false;
            // 
            // numExtentVal3
            // 
            this.numExtentVal3.InputType = CtrlLib.NumEdit.NumEditType.Double;
            this.numExtentVal3.Location = new System.Drawing.Point(575, 15);
            this.numExtentVal3.Name = "numExtentVal3";
            this.numExtentVal3.Size = new System.Drawing.Size(78, 20);
            this.numExtentVal3.TabIndex = 9;
            this.numExtentVal3.Visible = false;
            this.numExtentVal3.TextChanged += new System.EventHandler(this.numExtentVal3_TextChanged);
            // 
            // numExtentVal2
            // 
            this.numExtentVal2.InputType = CtrlLib.NumEdit.NumEditType.Double;
            this.numExtentVal2.Location = new System.Drawing.Point(451, 15);
            this.numExtentVal2.Name = "numExtentVal2";
            this.numExtentVal2.Size = new System.Drawing.Size(78, 20);
            this.numExtentVal2.TabIndex = 8;
            this.numExtentVal2.Visible = false;
            this.numExtentVal2.TextChanged += new System.EventHandler(this.numExtentVal2_TextChanged);
            // 
            // comboExtentPhase
            // 
            this.comboExtentPhase.FormattingEnabled = true;
            this.comboExtentPhase.Location = new System.Drawing.Point(402, 15);
            this.comboExtentPhase.Name = "comboExtentPhase";
            this.comboExtentPhase.Size = new System.Drawing.Size(78, 21);
            this.comboExtentPhase.TabIndex = 7;
            this.comboExtentPhase.Visible = false;
            this.comboExtentPhase.SelectedIndexChanged += new System.EventHandler(this.comboExtentPhase_SelectedIndexChanged);
            // 
            // lblExtentUnits
            // 
            this.lblExtentUnits.Location = new System.Drawing.Point(254, 18);
            this.lblExtentUnits.Name = "lblExtentUnits";
            this.lblExtentUnits.Size = new System.Drawing.Size(58, 13);
            this.lblExtentUnits.TabIndex = 6;
            this.lblExtentUnits.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // comboExtentSpecie
            // 
            this.comboExtentSpecie.FormattingEnabled = true;
            this.comboExtentSpecie.Location = new System.Drawing.Point(318, 15);
            this.comboExtentSpecie.Name = "comboExtentSpecie";
            this.comboExtentSpecie.Size = new System.Drawing.Size(78, 21);
            this.comboExtentSpecie.TabIndex = 4;
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
            this.numExtentValue.InputType = CtrlLib.NumEdit.NumEditType.Double;
            this.numExtentValue.Location = new System.Drawing.Point(177, 15);
            this.numExtentValue.Name = "numExtentValue";
            this.numExtentValue.Size = new System.Drawing.Size(71, 20);
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
            "Final Frac.",
            "Rate"});
            this.comboExtentType.Location = new System.Drawing.Point(10, 15);
            this.comboExtentType.Name = "comboExtentType";
            this.comboExtentType.Size = new System.Drawing.Size(121, 21);
            this.comboExtentType.TabIndex = 0;
            this.comboExtentType.SelectedIndexChanged += new System.EventHandler(this.comboExtentType_SelectedIndexChanged);
            // 
            // grpHOR
            // 
            this.grpHOR.Controls.Add(this.comboHORUnits);
            this.grpHOR.Controls.Add(this.comboHORSpecie);
            this.grpHOR.Controls.Add(this.numHORValue);
            this.grpHOR.Controls.Add(this.lblHORValue);
            this.grpHOR.Controls.Add(this.comboHORType);
            this.grpHOR.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.grpHOR.Location = new System.Drawing.Point(0, 71);
            this.grpHOR.Name = "grpHOR";
            this.grpHOR.Size = new System.Drawing.Size(677, 42);
            this.grpHOR.TabIndex = 9;
            this.grpHOR.TabStop = false;
            this.grpHOR.Text = "Heat Of Reaction";
            // 
            // comboHORUnits
            // 
            this.comboHORUnits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboHORUnits.FormattingEnabled = true;
            this.comboHORUnits.Items.AddRange(new object[] {
            "kJ/mol",
            "kJ/kg"});
            this.comboHORUnits.Location = new System.Drawing.Point(254, 15);
            this.comboHORUnits.Name = "comboHORUnits";
            this.comboHORUnits.Size = new System.Drawing.Size(58, 21);
            this.comboHORUnits.TabIndex = 6;
            this.comboHORUnits.SelectedIndexChanged += new System.EventHandler(this.comboHORUnits_SelectedIndexChanged);
            // 
            // comboHORSpecie
            // 
            this.comboHORSpecie.FormattingEnabled = true;
            this.comboHORSpecie.Location = new System.Drawing.Point(318, 15);
            this.comboHORSpecie.Name = "comboHORSpecie";
            this.comboHORSpecie.Size = new System.Drawing.Size(78, 21);
            this.comboHORSpecie.TabIndex = 5;
            this.comboHORSpecie.SelectedIndexChanged += new System.EventHandler(this.comboHORSpecie_SelectedIndexChanged);
            // 
            // numHORValue
            // 
            this.numHORValue.InputType = CtrlLib.NumEdit.NumEditType.Double;
            this.numHORValue.Location = new System.Drawing.Point(177, 15);
            this.numHORValue.Name = "numHORValue";
            this.numHORValue.Size = new System.Drawing.Size(71, 20);
            this.numHORValue.TabIndex = 4;
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
            // comboHORType
            // 
            this.comboHORType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboHORType.FormattingEnabled = true;
            this.comboHORType.Items.AddRange(new object[] {
            "Default",
            "Override"});
            this.comboHORType.Location = new System.Drawing.Point(10, 15);
            this.comboHORType.Name = "comboHORType";
            this.comboHORType.Size = new System.Drawing.Size(121, 21);
            this.comboHORType.TabIndex = 1;
            this.comboHORType.SelectedIndexChanged += new System.EventHandler(this.comboHORType_SelectedIndexChanged);
            // 
            // grpOther
            // 
            this.grpOther.Controls.Add(this.numSequence);
            this.grpOther.Controls.Add(this.label3);
            this.grpOther.Controls.Add(this.txtComment);
            this.grpOther.Controls.Add(this.chkEnabled);
            this.grpOther.Controls.Add(this.label1);
            this.grpOther.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.grpOther.Location = new System.Drawing.Point(0, 113);
            this.grpOther.Name = "grpOther";
            this.grpOther.Size = new System.Drawing.Size(677, 42);
            this.grpOther.TabIndex = 7;
            this.grpOther.TabStop = false;
            this.grpOther.Text = "Other Options";
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
            this.numSequence.TabIndex = 10;
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
            this.txtComment.Size = new System.Drawing.Size(387, 20);
            this.txtComment.TabIndex = 8;
            this.txtComment.TextChanged += new System.EventHandler(this.txtComment_TextChanged);
            // 
            // chkEnabled
            // 
            this.chkEnabled.AutoSize = true;
            this.chkEnabled.Location = new System.Drawing.Point(10, 18);
            this.chkEnabled.Name = "chkEnabled";
            this.chkEnabled.Size = new System.Drawing.Size(65, 17);
            this.chkEnabled.TabIndex = 7;
            this.chkEnabled.Text = "Enabled";
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
            // tabOptions
            // 
            this.tabOptions.Controls.Add(this.panel1);
            this.tabOptions.Location = new System.Drawing.Point(4, 22);
            this.tabOptions.Name = "tabOptions";
            this.tabOptions.Padding = new System.Windows.Forms.Padding(3);
            this.tabOptions.Size = new System.Drawing.Size(687, 358);
            this.tabOptions.TabIndex = 1;
            this.tabOptions.Text = "Sources And Sinks";
            this.tabOptions.UseVisualStyleBackColor = true;
            this.tabOptions.Resize += new System.EventHandler(this.tabSources_Sinks_Resize);
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.grpSources);
            this.panel1.Controls.Add(this.grpSinks);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(3, 3);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(681, 352);
            this.panel1.TabIndex = 2;
            // 
            // grpSources
            // 
            this.grpSources.Controls.Add(this.txtSources);
            this.grpSources.Dock = System.Windows.Forms.DockStyle.Fill;
            this.grpSources.Location = new System.Drawing.Point(0, 0);
            this.grpSources.Name = "grpSources";
            this.grpSources.Size = new System.Drawing.Size(681, 248);
            this.grpSources.TabIndex = 3;
            this.grpSources.TabStop = false;
            this.grpSources.Text = "Sources";
            // 
            // txtSources
            // 
            this.txtSources.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtSources.Location = new System.Drawing.Point(3, 16);
            this.txtSources.Multiline = true;
            this.txtSources.Name = "txtSources";
            this.txtSources.Size = new System.Drawing.Size(675, 229);
            this.txtSources.TabIndex = 0;
            this.txtSources.TextChanged += new System.EventHandler(this.txtSources_TextChanged);
            // 
            // grpSinks
            // 
            this.grpSinks.Controls.Add(this.txtSinks);
            this.grpSinks.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.grpSinks.Location = new System.Drawing.Point(0, 248);
            this.grpSinks.Name = "grpSinks";
            this.grpSinks.Size = new System.Drawing.Size(681, 104);
            this.grpSinks.TabIndex = 2;
            this.grpSinks.TabStop = false;
            this.grpSinks.Text = "Sinks";
            // 
            // txtSinks
            // 
            this.txtSinks.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtSinks.Location = new System.Drawing.Point(3, 16);
            this.txtSinks.Multiline = true;
            this.txtSinks.Name = "txtSinks";
            this.txtSinks.Size = new System.Drawing.Size(675, 85);
            this.txtSinks.TabIndex = 1;
            this.txtSinks.TextChanged += new System.EventHandler(this.txtSinks_TextChanged);
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.grpDescription);
            this.tabPage1.Controls.Add(this.groupBox1);
            this.tabPage1.Controls.Add(this.grpHX);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(687, 358);
            this.tabPage1.TabIndex = 2;
            this.tabPage1.Text = "Options";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // grpDescription
            // 
            this.grpDescription.Controls.Add(this.txtDescription);
            this.grpDescription.Dock = System.Windows.Forms.DockStyle.Fill;
            this.grpDescription.Location = new System.Drawing.Point(3, 87);
            this.grpDescription.Name = "grpDescription";
            this.grpDescription.Size = new System.Drawing.Size(681, 268);
            this.grpDescription.TabIndex = 13;
            this.grpDescription.TabStop = false;
            this.grpDescription.Text = "Description";
            // 
            // txtDescription
            // 
            this.txtDescription.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtDescription.Location = new System.Drawing.Point(3, 16);
            this.txtDescription.Multiline = true;
            this.txtDescription.Name = "txtDescription";
            this.txtDescription.Size = new System.Drawing.Size(675, 249);
            this.txtDescription.TabIndex = 0;
            this.txtDescription.TextChanged += new System.EventHandler(this.txtDescription_TextChanged);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.chkSequence);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox1.Location = new System.Drawing.Point(3, 45);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(681, 42);
            this.groupBox1.TabIndex = 14;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Sequences";
            // 
            // chkSequence
            // 
            this.chkSequence.AutoSize = true;
            this.chkSequence.Location = new System.Drawing.Point(10, 19);
            this.chkSequence.Name = "chkSequence";
            this.chkSequence.Size = new System.Drawing.Size(65, 17);
            this.chkSequence.TabIndex = 12;
            this.chkSequence.Text = "Enabled";
            this.chkSequence.UseVisualStyleBackColor = true;
            this.chkSequence.CheckedChanged += new System.EventHandler(this.chkSequence_CheckedChanged);
            // 
            // grpHX
            // 
            this.grpHX.Controls.Add(this.lblHXUnits);
            this.grpHX.Controls.Add(this.numHX);
            this.grpHX.Controls.Add(this.label2);
            this.grpHX.Controls.Add(this.comboHXType);
            this.grpHX.Dock = System.Windows.Forms.DockStyle.Top;
            this.grpHX.Location = new System.Drawing.Point(3, 3);
            this.grpHX.Name = "grpHX";
            this.grpHX.Size = new System.Drawing.Size(681, 42);
            this.grpHX.TabIndex = 11;
            this.grpHX.TabStop = false;
            this.grpHX.Text = "Heat Exchange";
            // 
            // lblHXUnits
            // 
            this.lblHXUnits.AutoSize = true;
            this.lblHXUnits.Location = new System.Drawing.Point(262, 18);
            this.lblHXUnits.Name = "lblHXUnits";
            this.lblHXUnits.Size = new System.Drawing.Size(0, 13);
            this.lblHXUnits.TabIndex = 8;
            // 
            // numHX
            // 
            this.numHX.Enabled = false;
            this.numHX.InputType = CtrlLib.NumEdit.NumEditType.Double;
            this.numHX.Location = new System.Drawing.Point(177, 15);
            this.numHX.Name = "numHX";
            this.numHX.Size = new System.Drawing.Size(78, 20);
            this.numHX.TabIndex = 6;
            this.numHX.TextChanged += new System.EventHandler(this.numHX_TextChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(137, 18);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(34, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Value";
            // 
            // comboHXType
            // 
            this.comboHXType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboHXType.FormattingEnabled = true;
            this.comboHXType.Items.AddRange(new object[] {
            "None",
            "Final Temp.",
            "Approach Temp.",
            "Approach Ambient",
            "Power",
            "Electrolysis"});
            this.comboHXType.Location = new System.Drawing.Point(10, 15);
            this.comboHXType.Name = "comboHXType";
            this.comboHXType.Size = new System.Drawing.Size(121, 21);
            this.comboHXType.TabIndex = 2;
            this.comboHXType.SelectedIndexChanged += new System.EventHandler(this.comboHXType_SelectedIndexChanged);
            // 
            // FrmReaction
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(703, 387);
            this.Controls.Add(this.tabControl1);
            this.MinimumSize = new System.Drawing.Size(571, 351);
            this.Name = "FrmReaction";
            this.Padding = new System.Windows.Forms.Padding(4);
            this.ShowInTaskbar = false;
            this.Text = "Untitled Reaction Block";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.FrmReaction_FormClosed);
            this.Load += new System.EventHandler(this.FrmReaction_Load);
            this.tabControl1.ResumeLayout(false);
            this.tabReactions.ResumeLayout(false);
            this.pnlList.ResumeLayout(false);
            this.pnlButtons.ResumeLayout(false);
            this.pnlReaction.ResumeLayout(false);
            this.pnlReaction.PerformLayout();
            this.grpExtent.ResumeLayout(false);
            this.grpExtent.PerformLayout();
            this.grpHOR.ResumeLayout(false);
            this.grpHOR.PerformLayout();
            this.grpOther.ResumeLayout(false);
            this.grpOther.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numSequence)).EndInit();
            this.tabOptions.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.grpSources.ResumeLayout(false);
            this.grpSources.PerformLayout();
            this.grpSinks.ResumeLayout(false);
            this.grpSinks.PerformLayout();
            this.tabPage1.ResumeLayout(false);
            this.grpDescription.ResumeLayout(false);
            this.grpDescription.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.grpHX.ResumeLayout(false);
            this.grpHX.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabReactions;
        private System.Windows.Forms.Panel pnlReaction;
        private System.Windows.Forms.TextBox txtReactants;
        private System.Windows.Forms.TextBox txtProducts;
        private System.Windows.Forms.ComboBox comboDirection;
        private System.Windows.Forms.TextBox txtFormula;
        private System.Windows.Forms.GroupBox grpExtent;
        private System.Windows.Forms.Label lblExtentValue;
        private CtrlLib.NumEdit numExtentValue;
        private System.Windows.Forms.ComboBox comboExtentType;
        private System.Windows.Forms.GroupBox grpHOR;
        private System.Windows.Forms.ComboBox comboHORSpecie;
        private CtrlLib.NumEdit numHORValue;
        private System.Windows.Forms.Label lblHORValue;
        private System.Windows.Forms.ComboBox comboHORType;
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
        private System.Windows.Forms.TabPage tabOptions;
        private System.Windows.Forms.ComboBox comboExtentSpecie;
        private System.Windows.Forms.ComboBox comboExtentPhase;
        private System.Windows.Forms.Label lblExtentUnits;
        private System.Windows.Forms.Label lblExtent2;
        private System.Windows.Forms.Label lblExtent3;
        private CtrlLib.NumEdit numExtentVal3;
        private CtrlLib.NumEdit numExtentVal2;
        private System.Windows.Forms.CheckBox chkExtentStabilised;
        private System.Windows.Forms.CheckBox chkEnabled;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.GroupBox grpSources;
        private System.Windows.Forms.TextBox txtSources;
        private System.Windows.Forms.GroupBox grpSinks;
        private System.Windows.Forms.TextBox txtSinks;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox txtComment;
        private System.Windows.Forms.NumericUpDown numSequence;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.GroupBox grpDescription;
        private System.Windows.Forms.CheckBox chkSequence;
        private System.Windows.Forms.GroupBox grpHX;
        private System.Windows.Forms.Label lblHXUnits;
        private CtrlLib.NumEdit numHX;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox comboHXType;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox txtDescription;
        private System.Windows.Forms.ComboBox comboHORUnits;
        private System.Windows.Forms.Button btnCopy;


    }
}