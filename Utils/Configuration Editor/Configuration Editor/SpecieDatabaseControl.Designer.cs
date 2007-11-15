namespace Configuration_Editor
{
    partial class SpecieDatabaseControl
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
            this.tlpTempDependantRadios = new System.Windows.Forms.TableLayoutPanel();
            this.label37 = new System.Windows.Forms.Label();
            this.groupBox8 = new System.Windows.Forms.GroupBox();
            this.lblCritPressUnits = new System.Windows.Forms.Label();
            this.lblCritVolUnits = new System.Windows.Forms.Label();
            this.lblCritTempUnits = new System.Windows.Forms.Label();
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
            this.lblMaxTempUnits = new System.Windows.Forms.Label();
            this.lblMinTUnits = new System.Windows.Forms.Label();
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
            this.errorProvider1 = new System.Windows.Forms.ErrorProvider(this.components);
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
            ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.splitContainer2);
            this.groupBox6.Controls.Add(this.pnlTempDependantRadios);
            this.groupBox6.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox6.Location = new System.Drawing.Point(0, 306);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(603, 204);
            this.groupBox6.TabIndex = 9;
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
            this.splitContainer2.Size = new System.Drawing.Size(470, 185);
            this.splitContainer2.SplitterDistance = 188;
            this.splitContainer2.TabIndex = 10;
            this.splitContainer2.TabStop = false;
            // 
            // graph1
            // 
            this.graph1.BackColor = System.Drawing.Color.LightGray;
            this.graph1.Border = 5;
            this.graph1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.graph1.CheckSize = 5;
            this.graph1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.graph1.DrawYLabel = true;
            this.graph1.Formatters = null;
            this.graph1.Location = new System.Drawing.Point(0, 0);
            this.graph1.MaxChecks = 6;
            this.graph1.MaxXValue = 0;
            this.graph1.MinXValue = 0;
            this.graph1.Name = "graph1";
            this.graph1.Size = new System.Drawing.Size(188, 185);
            this.graph1.TabIndex = 6;
            this.graph1.UnselectedTransparency = 64;
            this.graph1.XLabel = "Temperature";
            this.graph1.XUnits = "temperature";
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
            this.tlpEquations.Size = new System.Drawing.Size(278, 185);
            this.tlpEquations.TabIndex = 1;
            // 
            // txtMaxTemp3
            // 
            this.txtMaxTemp3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMaxTemp3.Location = new System.Drawing.Point(231, 115);
            this.txtMaxTemp3.Name = "txtMaxTemp3";
            this.txtMaxTemp3.Size = new System.Drawing.Size(44, 20);
            this.txtMaxTemp3.TabIndex = 11;
            this.txtMaxTemp3.Validated += new System.EventHandler(this.txtFormula0_Validated);
            this.txtMaxTemp3.Validating += new System.ComponentModel.CancelEventHandler(this.txtMinTemp3_Validating);
            this.txtMaxTemp3.TextChanged += new System.EventHandler(this.txtFormula0_TextChanged);
            // 
            // txtMinTemp3
            // 
            this.txtMinTemp3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMinTemp3.Location = new System.Drawing.Point(166, 115);
            this.txtMinTemp3.Name = "txtMinTemp3";
            this.txtMinTemp3.Size = new System.Drawing.Size(44, 20);
            this.txtMinTemp3.TabIndex = 10;
            this.txtMinTemp3.Validated += new System.EventHandler(this.txtFormula0_Validated);
            this.txtMinTemp3.Validating += new System.ComponentModel.CancelEventHandler(this.txtMinTemp3_Validating);
            this.txtMinTemp3.TextChanged += new System.EventHandler(this.txtFormula0_TextChanged);
            // 
            // txtMaxTemp2
            // 
            this.txtMaxTemp2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMaxTemp2.Location = new System.Drawing.Point(231, 89);
            this.txtMaxTemp2.Name = "txtMaxTemp2";
            this.txtMaxTemp2.Size = new System.Drawing.Size(44, 20);
            this.txtMaxTemp2.TabIndex = 8;
            this.txtMaxTemp2.Validated += new System.EventHandler(this.txtFormula0_Validated);
            this.txtMaxTemp2.Validating += new System.ComponentModel.CancelEventHandler(this.txtMinTemp3_Validating);
            this.txtMaxTemp2.TextChanged += new System.EventHandler(this.txtFormula0_TextChanged);
            // 
            // txtMinTemp2
            // 
            this.txtMinTemp2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMinTemp2.Location = new System.Drawing.Point(166, 89);
            this.txtMinTemp2.Name = "txtMinTemp2";
            this.txtMinTemp2.Size = new System.Drawing.Size(44, 20);
            this.txtMinTemp2.TabIndex = 7;
            this.txtMinTemp2.Validated += new System.EventHandler(this.txtFormula0_Validated);
            this.txtMinTemp2.Validating += new System.ComponentModel.CancelEventHandler(this.txtMinTemp3_Validating);
            this.txtMinTemp2.TextChanged += new System.EventHandler(this.txtFormula0_TextChanged);
            // 
            // txtMaxTemp1
            // 
            this.txtMaxTemp1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMaxTemp1.Location = new System.Drawing.Point(231, 63);
            this.txtMaxTemp1.Name = "txtMaxTemp1";
            this.txtMaxTemp1.Size = new System.Drawing.Size(44, 20);
            this.txtMaxTemp1.TabIndex = 5;
            this.txtMaxTemp1.Validated += new System.EventHandler(this.txtFormula0_Validated);
            this.txtMaxTemp1.Validating += new System.ComponentModel.CancelEventHandler(this.txtMinTemp3_Validating);
            this.txtMaxTemp1.TextChanged += new System.EventHandler(this.txtFormula0_TextChanged);
            // 
            // txtMinTemp1
            // 
            this.txtMinTemp1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMinTemp1.Location = new System.Drawing.Point(166, 63);
            this.txtMinTemp1.Name = "txtMinTemp1";
            this.txtMinTemp1.Size = new System.Drawing.Size(44, 20);
            this.txtMinTemp1.TabIndex = 4;
            this.txtMinTemp1.Validated += new System.EventHandler(this.txtFormula0_Validated);
            this.txtMinTemp1.Validating += new System.ComponentModel.CancelEventHandler(this.txtMinTemp3_Validating);
            this.txtMinTemp1.TextChanged += new System.EventHandler(this.txtFormula0_TextChanged);
            // 
            // txtMaxTemp0
            // 
            this.txtMaxTemp0.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMaxTemp0.Location = new System.Drawing.Point(231, 37);
            this.txtMaxTemp0.Name = "txtMaxTemp0";
            this.txtMaxTemp0.Size = new System.Drawing.Size(44, 20);
            this.txtMaxTemp0.TabIndex = 2;
            this.txtMaxTemp0.Validated += new System.EventHandler(this.txtFormula0_Validated);
            this.txtMaxTemp0.Validating += new System.ComponentModel.CancelEventHandler(this.txtMinTemp3_Validating);
            this.txtMaxTemp0.TextChanged += new System.EventHandler(this.txtFormula0_TextChanged);
            // 
            // txtMinTemp0
            // 
            this.txtMinTemp0.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtMinTemp0.Location = new System.Drawing.Point(166, 37);
            this.txtMinTemp0.Name = "txtMinTemp0";
            this.txtMinTemp0.Size = new System.Drawing.Size(44, 20);
            this.txtMinTemp0.TabIndex = 1;
            this.txtMinTemp0.Validated += new System.EventHandler(this.txtFormula0_Validated);
            this.txtMinTemp0.Validating += new System.ComponentModel.CancelEventHandler(this.txtMinTemp3_Validating);
            this.txtMinTemp0.TextChanged += new System.EventHandler(this.txtFormula0_TextChanged);
            // 
            // label38
            // 
            this.label38.AutoSize = true;
            this.label38.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label38.Location = new System.Drawing.Point(3, 0);
            this.label38.Name = "label38";
            this.label38.Padding = new System.Windows.Forms.Padding(4);
            this.label38.Size = new System.Drawing.Size(157, 34);
            this.label38.TabIndex = 7;
            this.label38.Text = "Equation";
            this.label38.TextAlign = System.Drawing.ContentAlignment.BottomLeft;
            // 
            // label39
            // 
            this.label39.AutoSize = true;
            this.tlpEquations.SetColumnSpan(this.label39, 3);
            this.label39.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label39.Location = new System.Drawing.Point(166, 0);
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
            this.label40.Location = new System.Drawing.Point(216, 34);
            this.label40.Name = "label40";
            this.label40.Size = new System.Drawing.Size(9, 13);
            this.label40.TabIndex = 9;
            this.label40.Text = "-";
            // 
            // label45
            // 
            this.label45.AutoSize = true;
            this.label45.Location = new System.Drawing.Point(216, 86);
            this.label45.Name = "label45";
            this.label45.Size = new System.Drawing.Size(9, 13);
            this.label45.TabIndex = 12;
            this.label45.Text = "-";
            // 
            // label41
            // 
            this.label41.AutoSize = true;
            this.label41.Location = new System.Drawing.Point(216, 60);
            this.label41.Name = "label41";
            this.label41.Size = new System.Drawing.Size(9, 13);
            this.label41.TabIndex = 10;
            this.label41.Text = "-";
            // 
            // label42
            // 
            this.label42.AutoSize = true;
            this.label42.Location = new System.Drawing.Point(216, 112);
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
            this.txtFormula0.Size = new System.Drawing.Size(157, 20);
            this.txtFormula0.TabIndex = 0;
            this.txtFormula0.Validated += new System.EventHandler(this.txtFormula0_Validated);
            this.txtFormula0.Validating += new System.ComponentModel.CancelEventHandler(this.txtFormula0_Validating);
            this.txtFormula0.TextChanged += new System.EventHandler(this.txtFormula0_TextChanged);
            // 
            // txtFormula1
            // 
            this.txtFormula1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtFormula1.Location = new System.Drawing.Point(3, 63);
            this.txtFormula1.Name = "txtFormula1";
            this.txtFormula1.Size = new System.Drawing.Size(157, 20);
            this.txtFormula1.TabIndex = 3;
            this.txtFormula1.Validated += new System.EventHandler(this.txtFormula0_Validated);
            this.txtFormula1.Validating += new System.ComponentModel.CancelEventHandler(this.txtFormula0_Validating);
            this.txtFormula1.TextChanged += new System.EventHandler(this.txtFormula0_TextChanged);
            // 
            // txtFormula2
            // 
            this.txtFormula2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtFormula2.Location = new System.Drawing.Point(3, 89);
            this.txtFormula2.Name = "txtFormula2";
            this.txtFormula2.Size = new System.Drawing.Size(157, 20);
            this.txtFormula2.TabIndex = 6;
            this.txtFormula2.Validated += new System.EventHandler(this.txtFormula0_Validated);
            this.txtFormula2.Validating += new System.ComponentModel.CancelEventHandler(this.txtFormula0_Validating);
            this.txtFormula2.TextChanged += new System.EventHandler(this.txtFormula0_TextChanged);
            // 
            // txtFormula3
            // 
            this.txtFormula3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtFormula3.Location = new System.Drawing.Point(3, 115);
            this.txtFormula3.Name = "txtFormula3";
            this.txtFormula3.Size = new System.Drawing.Size(157, 20);
            this.txtFormula3.TabIndex = 9;
            this.txtFormula3.Validated += new System.EventHandler(this.txtFormula0_Validated);
            this.txtFormula3.Validating += new System.ComponentModel.CancelEventHandler(this.txtFormula0_Validating);
            this.txtFormula3.TextChanged += new System.EventHandler(this.txtFormula0_TextChanged);
            // 
            // pnlTempDependantRadios
            // 
            this.pnlTempDependantRadios.Controls.Add(this.tlpTempDependantRadios);
            this.pnlTempDependantRadios.Controls.Add(this.label37);
            this.pnlTempDependantRadios.Dock = System.Windows.Forms.DockStyle.Left;
            this.pnlTempDependantRadios.Location = new System.Drawing.Point(3, 16);
            this.pnlTempDependantRadios.Name = "pnlTempDependantRadios";
            this.pnlTempDependantRadios.Size = new System.Drawing.Size(127, 185);
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
            this.tlpTempDependantRadios.Size = new System.Drawing.Size(127, 172);
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
            this.groupBox8.Controls.Add(this.lblCritPressUnits);
            this.groupBox8.Controls.Add(this.lblCritVolUnits);
            this.groupBox8.Controls.Add(this.lblCritTempUnits);
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
            this.groupBox8.Location = new System.Drawing.Point(0, 182);
            this.groupBox8.Name = "groupBox8";
            this.groupBox8.Size = new System.Drawing.Size(603, 124);
            this.groupBox8.TabIndex = 8;
            this.groupBox8.TabStop = false;
            this.groupBox8.Text = "Thermodynamic Properties";
            // 
            // lblCritPressUnits
            // 
            this.lblCritPressUnits.AutoSize = true;
            this.lblCritPressUnits.Location = new System.Drawing.Point(255, 48);
            this.lblCritPressUnits.Name = "lblCritPressUnits";
            this.lblCritPressUnits.Size = new System.Drawing.Size(18, 13);
            this.lblCritPressUnits.TabIndex = 24;
            this.lblCritPressUnits.Text = "°C";
            // 
            // lblCritVolUnits
            // 
            this.lblCritVolUnits.AutoSize = true;
            this.lblCritVolUnits.Location = new System.Drawing.Point(255, 74);
            this.lblCritVolUnits.Name = "lblCritVolUnits";
            this.lblCritVolUnits.Size = new System.Drawing.Size(18, 13);
            this.lblCritVolUnits.TabIndex = 23;
            this.lblCritVolUnits.Text = "°C";
            // 
            // lblCritTempUnits
            // 
            this.lblCritTempUnits.AutoSize = true;
            this.lblCritTempUnits.Location = new System.Drawing.Point(532, 48);
            this.lblCritTempUnits.Name = "lblCritTempUnits";
            this.lblCritTempUnits.Size = new System.Drawing.Size(18, 13);
            this.lblCritTempUnits.TabIndex = 21;
            this.lblCritTempUnits.Text = "°C";
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(125, 97);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(407, 20);
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
            this.txtPhaseChange.Location = new System.Drawing.Point(402, 19);
            this.txtPhaseChange.Name = "txtPhaseChange";
            this.txtPhaseChange.Size = new System.Drawing.Size(130, 20);
            this.txtPhaseChange.TabIndex = 1;
            // 
            // label48
            // 
            this.label48.AutoSize = true;
            this.label48.Location = new System.Drawing.Point(319, 22);
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
            this.txtAccentricity.Location = new System.Drawing.Point(402, 71);
            this.txtAccentricity.Name = "txtAccentricity";
            this.txtAccentricity.Size = new System.Drawing.Size(130, 20);
            this.txtAccentricity.TabIndex = 5;
            // 
            // txtCritTemp
            // 
            this.txtCritTemp.Location = new System.Drawing.Point(402, 45);
            this.txtCritTemp.Name = "txtCritTemp";
            this.txtCritTemp.Size = new System.Drawing.Size(130, 20);
            this.txtCritTemp.TabIndex = 3;
            // 
            // label47
            // 
            this.label47.AutoSize = true;
            this.label47.Location = new System.Drawing.Point(334, 74);
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
            this.label43.Location = new System.Drawing.Point(295, 48);
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
            this.txtDensityCorrection.Location = new System.Drawing.Point(0, 131);
            this.txtDensityCorrection.Name = "txtDensityCorrection";
            this.txtDensityCorrection.Size = new System.Drawing.Size(603, 51);
            this.txtDensityCorrection.TabIndex = 7;
            this.txtDensityCorrection.TabStop = false;
            this.txtDensityCorrection.Text = "Source";
            // 
            // txtCheckedBy
            // 
            this.txtCheckedBy.Location = new System.Drawing.Point(402, 19);
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
            this.label34.Location = new System.Drawing.Point(331, 22);
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
            this.groupBox4.Controls.Add(this.lblMaxTempUnits);
            this.groupBox4.Controls.Add(this.lblMinTUnits);
            this.groupBox4.Controls.Add(this.comboOccurence);
            this.groupBox4.Controls.Add(this.label33);
            this.groupBox4.Controls.Add(this.txtMaxTemperature);
            this.groupBox4.Controls.Add(this.txtMinTemperature);
            this.groupBox4.Controls.Add(this.txtElementalComposition);
            this.groupBox4.Controls.Add(this.label29);
            this.groupBox4.Controls.Add(this.label27);
            this.groupBox4.Controls.Add(this.label28);
            this.groupBox4.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox4.Location = new System.Drawing.Point(0, 49);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(603, 82);
            this.groupBox4.TabIndex = 6;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "General Properties";
            // 
            // lblMaxTempUnits
            // 
            this.lblMaxTempUnits.AutoSize = true;
            this.lblMaxTempUnits.Location = new System.Drawing.Point(532, 53);
            this.lblMaxTempUnits.Name = "lblMaxTempUnits";
            this.lblMaxTempUnits.Size = new System.Drawing.Size(18, 13);
            this.lblMaxTempUnits.TabIndex = 5;
            this.lblMaxTempUnits.Text = "°C";
            // 
            // lblMinTUnits
            // 
            this.lblMinTUnits.AutoSize = true;
            this.lblMinTUnits.Location = new System.Drawing.Point(255, 53);
            this.lblMinTUnits.Name = "lblMinTUnits";
            this.lblMinTUnits.Size = new System.Drawing.Size(18, 13);
            this.lblMinTUnits.TabIndex = 4;
            this.lblMinTUnits.Text = "°C";
            // 
            // comboOccurence
            // 
            this.comboOccurence.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboOccurence.FormattingEnabled = true;
            this.comboOccurence.Items.AddRange(new object[] {
            "s",
            "l",
            "g"});
            this.comboOccurence.Location = new System.Drawing.Point(402, 18);
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
            this.txtMaxTemperature.Location = new System.Drawing.Point(402, 50);
            this.txtMaxTemperature.Name = "txtMaxTemperature";
            this.txtMaxTemperature.Size = new System.Drawing.Size(130, 20);
            this.txtMaxTemperature.TabIndex = 3;
            // 
            // txtMinTemperature
            // 
            this.txtMinTemperature.Location = new System.Drawing.Point(125, 50);
            this.txtMinTemperature.Name = "txtMinTemperature";
            this.txtMinTemperature.Size = new System.Drawing.Size(130, 20);
            this.txtMinTemperature.TabIndex = 2;
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
            this.label29.Location = new System.Drawing.Point(336, 21);
            this.label29.Name = "label29";
            this.label29.Size = new System.Drawing.Size(60, 13);
            this.label29.TabIndex = 3;
            this.label29.Text = "Occurence";
            // 
            // label27
            // 
            this.label27.AutoSize = true;
            this.label27.Location = new System.Drawing.Point(282, 53);
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
            this.grpNomenclature.Location = new System.Drawing.Point(0, 0);
            this.grpNomenclature.Name = "grpNomenclature";
            this.grpNomenclature.Size = new System.Drawing.Size(603, 49);
            this.grpNomenclature.TabIndex = 5;
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
            this.comboPhase.Validated += new System.EventHandler(this.comboPhase_Validated);
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
            // errorProvider1
            // 
            this.errorProvider1.ContainerControl = this;
            // 
            // SpecieDatabaseControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox6);
            this.Controls.Add(this.groupBox8);
            this.Controls.Add(this.txtDensityCorrection);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.grpNomenclature);
            this.Name = "SpecieDatabaseControl";
            this.Size = new System.Drawing.Size(603, 510);
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
            ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.SplitContainer splitContainer2;
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
        private System.Windows.Forms.Label label38;
        private System.Windows.Forms.Label label39;
        private System.Windows.Forms.Label label40;
        private System.Windows.Forms.Label label45;
        private System.Windows.Forms.Label label41;
        private System.Windows.Forms.Label label42;
        private System.Windows.Forms.TextBox txtFormula0;
        private System.Windows.Forms.TextBox txtFormula1;
        private System.Windows.Forms.TextBox txtFormula2;
        private System.Windows.Forms.TextBox txtFormula3;
        private System.Windows.Forms.Panel pnlTempDependantRadios;
        private System.Windows.Forms.TableLayoutPanel tlpTempDependantRadios;
        private System.Windows.Forms.Label label37;
        private System.Windows.Forms.GroupBox groupBox8;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label51;
        private System.Windows.Forms.TextBox txtPhaseChange;
        private System.Windows.Forms.Label label48;
        private System.Windows.Forms.TextBox txtSolvent;
        private System.Windows.Forms.Label label36;
        private System.Windows.Forms.TextBox txtAccentricity;
        private System.Windows.Forms.TextBox txtCritTemp;
        private System.Windows.Forms.Label label47;
        private System.Windows.Forms.TextBox txtCritVol;
        private System.Windows.Forms.Label label46;
        private System.Windows.Forms.TextBox txtCritPressure;
        private System.Windows.Forms.Label label43;
        private System.Windows.Forms.Label label44;
        private System.Windows.Forms.GroupBox txtDensityCorrection;
        private System.Windows.Forms.TextBox txtCheckedBy;
        private System.Windows.Forms.Label label35;
        private System.Windows.Forms.Label label34;
        private System.Windows.Forms.TextBox txtSource;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.ComboBox comboOccurence;
        private System.Windows.Forms.Label label33;
        private System.Windows.Forms.TextBox txtMaxTemperature;
        private System.Windows.Forms.TextBox txtMinTemperature;
        private System.Windows.Forms.TextBox txtElementalComposition;
        private System.Windows.Forms.Label label29;
        private System.Windows.Forms.Label label27;
        private System.Windows.Forms.Label label28;
        private System.Windows.Forms.GroupBox grpNomenclature;
        private System.Windows.Forms.ComboBox comboPhase;
        private System.Windows.Forms.Label label25;
        private System.Windows.Forms.TextBox txtName;
        private System.Windows.Forms.Label label30;
        private System.Windows.Forms.TextBox txtSymbol;
        private System.Windows.Forms.Label label31;
        private System.Windows.Forms.Label label32;
        private System.Windows.Forms.ErrorProvider errorProvider1;
        private System.Windows.Forms.Label lblCritTempUnits;
        private System.Windows.Forms.Label lblMaxTempUnits;
        private System.Windows.Forms.Label lblMinTUnits;
        private System.Windows.Forms.Label lblCritPressUnits;
        private System.Windows.Forms.Label lblCritVolUnits;
    }
}
