namespace Configuration_Editor
{
    partial class FrmAddSelector
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
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.panel1 = new System.Windows.Forms.Panel();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnOk = new System.Windows.Forms.Button();
            this.radioAttribute = new System.Windows.Forms.RadioButton();
            this.radioPageBreak = new System.Windows.Forms.RadioButton();
            this.radioLabel = new System.Windows.Forms.RadioButton();
            this.label1 = new System.Windows.Forms.Label();
            this.radioCalculation = new System.Windows.Forms.RadioButton();
            this.tableLayoutPanel1.SuspendLayout();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.AutoSize = true;
            this.tableLayoutPanel1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.tableLayoutPanel1.ColumnCount = 1;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Controls.Add(this.panel1, 0, 5);
            this.tableLayoutPanel1.Controls.Add(this.radioAttribute, 0, 4);
            this.tableLayoutPanel1.Controls.Add(this.radioPageBreak, 0, 3);
            this.tableLayoutPanel1.Controls.Add(this.radioLabel, 0, 2);
            this.tableLayoutPanel1.Controls.Add(this.label1, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.radioCalculation, 0, 1);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.Padding = new System.Windows.Forms.Padding(8);
            this.tableLayoutPanel1.RowCount = 6;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(197, 156);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // panel1
            // 
            this.panel1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.panel1.Controls.Add(this.btnCancel);
            this.panel1.Controls.Add(this.btnOk);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(11, 116);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(175, 32);
            this.panel1.TabIndex = 4;
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(90, 3);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 5;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // btnOk
            // 
            this.btnOk.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOk.Location = new System.Drawing.Point(9, 3);
            this.btnOk.Name = "btnOk";
            this.btnOk.Size = new System.Drawing.Size(75, 23);
            this.btnOk.TabIndex = 4;
            this.btnOk.Text = "&OK";
            this.btnOk.UseVisualStyleBackColor = true;
            // 
            // radioAttribute
            // 
            this.radioAttribute.AutoSize = true;
            this.radioAttribute.Location = new System.Drawing.Point(11, 93);
            this.radioAttribute.Name = "radioAttribute";
            this.radioAttribute.Size = new System.Drawing.Size(64, 17);
            this.radioAttribute.TabIndex = 3;
            this.radioAttribute.TabStop = true;
            this.radioAttribute.Text = "&Attribute";
            this.radioAttribute.UseVisualStyleBackColor = true;
            this.radioAttribute.CheckedChanged += new System.EventHandler(this.radioCalculation_CheckedChanged);
            // 
            // radioPageBreak
            // 
            this.radioPageBreak.AutoSize = true;
            this.radioPageBreak.Location = new System.Drawing.Point(11, 70);
            this.radioPageBreak.Name = "radioPageBreak";
            this.radioPageBreak.Size = new System.Drawing.Size(81, 17);
            this.radioPageBreak.TabIndex = 2;
            this.radioPageBreak.TabStop = true;
            this.radioPageBreak.Text = "&Page Break";
            this.radioPageBreak.UseVisualStyleBackColor = true;
            this.radioPageBreak.CheckedChanged += new System.EventHandler(this.radioCalculation_CheckedChanged);
            // 
            // radioLabel
            // 
            this.radioLabel.AutoSize = true;
            this.radioLabel.Location = new System.Drawing.Point(11, 47);
            this.radioLabel.Name = "radioLabel";
            this.radioLabel.Size = new System.Drawing.Size(51, 17);
            this.radioLabel.TabIndex = 1;
            this.radioLabel.TabStop = true;
            this.radioLabel.Text = "&Label";
            this.radioLabel.UseVisualStyleBackColor = true;
            this.radioLabel.CheckedChanged += new System.EventHandler(this.radioCalculation_CheckedChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(11, 8);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(72, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Please Select";
            // 
            // radioCalculation
            // 
            this.radioCalculation.AutoSize = true;
            this.radioCalculation.Checked = true;
            this.radioCalculation.Location = new System.Drawing.Point(11, 24);
            this.radioCalculation.Name = "radioCalculation";
            this.radioCalculation.Size = new System.Drawing.Size(77, 17);
            this.radioCalculation.TabIndex = 0;
            this.radioCalculation.TabStop = true;
            this.radioCalculation.Text = "&Calculation";
            this.radioCalculation.UseVisualStyleBackColor = true;
            this.radioCalculation.CheckedChanged += new System.EventHandler(this.radioCalculation_CheckedChanged);
            // 
            // FrmAddSelector
            // 
            this.AcceptButton = this.btnOk;
            this.AutoSize = true;
            this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(197, 156);
            this.Controls.Add(this.tableLayoutPanel1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FrmAddSelector";
            this.Text = "Add";
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnOk;
        public System.Windows.Forms.RadioButton radioAttribute;
        public System.Windows.Forms.RadioButton radioPageBreak;
        public System.Windows.Forms.RadioButton radioLabel;
        public System.Windows.Forms.RadioButton radioCalculation;
    }
}