namespace Configuration_Editor
{
    partial class FrmSortOptions
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
            this.chkByPhase = new System.Windows.Forms.CheckBox();
            this.chkPreservePageAndLabels = new System.Windows.Forms.CheckBox();
            this.chkPreserveCalculations = new System.Windows.Forms.CheckBox();
            this.radioAscending = new System.Windows.Forms.RadioButton();
            this.radioDescending = new System.Windows.Forms.RadioButton();
            this.grpLexiographicOrder = new System.Windows.Forms.GroupBox();
            this.grpPhaseOrder = new System.Windows.Forms.GroupBox();
            this.lstPhaseOrder = new System.Windows.Forms.ListBox();
            this.grpApplicable = new System.Windows.Forms.GroupBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnOK = new System.Windows.Forms.Button();
            this.grpLexiographicOrder.SuspendLayout();
            this.grpPhaseOrder.SuspendLayout();
            this.grpApplicable.SuspendLayout();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // chkByPhase
            // 
            this.chkByPhase.AutoSize = true;
            this.chkByPhase.Checked = true;
            this.chkByPhase.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkByPhase.Location = new System.Drawing.Point(6, 19);
            this.chkByPhase.Name = "chkByPhase";
            this.chkByPhase.Size = new System.Drawing.Size(93, 17);
            this.chkByPhase.TabIndex = 1;
            this.chkByPhase.Text = "Sort By Phase";
            this.chkByPhase.UseVisualStyleBackColor = true;
            this.chkByPhase.CheckedChanged += new System.EventHandler(this.chkByPhase_CheckedChanged);
            // 
            // chkPreservePageAndLabels
            // 
            this.chkPreservePageAndLabels.AutoSize = true;
            this.chkPreservePageAndLabels.Checked = true;
            this.chkPreservePageAndLabels.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkPreservePageAndLabels.Location = new System.Drawing.Point(6, 42);
            this.chkPreservePageAndLabels.Name = "chkPreservePageAndLabels";
            this.chkPreservePageAndLabels.Size = new System.Drawing.Size(196, 17);
            this.chkPreservePageAndLabels.TabIndex = 2;
            this.chkPreservePageAndLabels.Text = "Preserve Page And Label Locations";
            this.chkPreservePageAndLabels.UseVisualStyleBackColor = true;
            this.chkPreservePageAndLabels.CheckedChanged += new System.EventHandler(this.chkPreservePageAndLabels_CheckedChanged);
            // 
            // chkPreserveCalculations
            // 
            this.chkPreserveCalculations.AutoSize = true;
            this.chkPreserveCalculations.Location = new System.Drawing.Point(6, 65);
            this.chkPreserveCalculations.Name = "chkPreserveCalculations";
            this.chkPreserveCalculations.Size = new System.Drawing.Size(172, 17);
            this.chkPreserveCalculations.TabIndex = 3;
            this.chkPreserveCalculations.Text = "Preserve Calculation Locations";
            this.chkPreserveCalculations.UseVisualStyleBackColor = true;
            this.chkPreserveCalculations.CheckedChanged += new System.EventHandler(this.chkPreserveCalculations_CheckedChanged);
            // 
            // radioAscending
            // 
            this.radioAscending.AutoSize = true;
            this.radioAscending.Checked = true;
            this.radioAscending.Location = new System.Drawing.Point(6, 19);
            this.radioAscending.Name = "radioAscending";
            this.radioAscending.Size = new System.Drawing.Size(75, 17);
            this.radioAscending.TabIndex = 4;
            this.radioAscending.TabStop = true;
            this.radioAscending.Text = "Ascending";
            this.radioAscending.UseVisualStyleBackColor = true;
            this.radioAscending.CheckedChanged += new System.EventHandler(this.radioAscending_CheckedChanged);
            // 
            // radioDescending
            // 
            this.radioDescending.AutoSize = true;
            this.radioDescending.Location = new System.Drawing.Point(6, 42);
            this.radioDescending.Name = "radioDescending";
            this.radioDescending.Size = new System.Drawing.Size(82, 17);
            this.radioDescending.TabIndex = 5;
            this.radioDescending.Text = "Descending";
            this.radioDescending.UseVisualStyleBackColor = true;
            // 
            // grpLexiographicOrder
            // 
            this.grpLexiographicOrder.Controls.Add(this.radioDescending);
            this.grpLexiographicOrder.Controls.Add(this.radioAscending);
            this.grpLexiographicOrder.Location = new System.Drawing.Point(12, 108);
            this.grpLexiographicOrder.Name = "grpLexiographicOrder";
            this.grpLexiographicOrder.Size = new System.Drawing.Size(215, 66);
            this.grpLexiographicOrder.TabIndex = 6;
            this.grpLexiographicOrder.TabStop = false;
            this.grpLexiographicOrder.Text = "Lexiographic Order";
            // 
            // grpPhaseOrder
            // 
            this.grpPhaseOrder.Controls.Add(this.lstPhaseOrder);
            this.grpPhaseOrder.Location = new System.Drawing.Point(12, 180);
            this.grpPhaseOrder.Name = "grpPhaseOrder";
            this.grpPhaseOrder.Size = new System.Drawing.Size(215, 62);
            this.grpPhaseOrder.TabIndex = 7;
            this.grpPhaseOrder.TabStop = false;
            this.grpPhaseOrder.Text = "Phase Order";
            // 
            // lstPhaseOrder
            // 
            this.lstPhaseOrder.AllowDrop = true;
            this.lstPhaseOrder.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lstPhaseOrder.FormattingEnabled = true;
            this.lstPhaseOrder.Location = new System.Drawing.Point(3, 16);
            this.lstPhaseOrder.Name = "lstPhaseOrder";
            this.lstPhaseOrder.Size = new System.Drawing.Size(209, 43);
            this.lstPhaseOrder.TabIndex = 0;
            this.lstPhaseOrder.MouseUp += new System.Windows.Forms.MouseEventHandler(this.lstPhaseOrder_MouseUp);
            this.lstPhaseOrder.MouseMove += new System.Windows.Forms.MouseEventHandler(this.lstPhaseOrder_MouseMove);
            this.lstPhaseOrder.MouseDown += new System.Windows.Forms.MouseEventHandler(this.lstPhaseOrder_MouseDown);
            // 
            // grpApplicable
            // 
            this.grpApplicable.Controls.Add(this.chkPreserveCalculations);
            this.grpApplicable.Controls.Add(this.chkPreservePageAndLabels);
            this.grpApplicable.Controls.Add(this.chkByPhase);
            this.grpApplicable.Location = new System.Drawing.Point(12, 12);
            this.grpApplicable.Name = "grpApplicable";
            this.grpApplicable.Size = new System.Drawing.Size(215, 90);
            this.grpApplicable.TabIndex = 8;
            this.grpApplicable.TabStop = false;
            this.grpApplicable.Text = "Applicable Objects";
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.btnCancel);
            this.panel1.Controls.Add(this.btnOK);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(0, 244);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(240, 42);
            this.panel1.TabIndex = 9;
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(153, 7);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 1;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // btnOK
            // 
            this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOK.Location = new System.Drawing.Point(72, 7);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 0;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            // 
            // FrmSortOptions
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(240, 286);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.grpApplicable);
            this.Controls.Add(this.grpPhaseOrder);
            this.Controls.Add(this.grpLexiographicOrder);
            this.Name = "FrmSortOptions";
            this.Text = "Advanced Sort";
            this.grpLexiographicOrder.ResumeLayout(false);
            this.grpLexiographicOrder.PerformLayout();
            this.grpPhaseOrder.ResumeLayout(false);
            this.grpApplicable.ResumeLayout(false);
            this.grpApplicable.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.CheckBox chkByPhase;
        private System.Windows.Forms.CheckBox chkPreservePageAndLabels;
        private System.Windows.Forms.CheckBox chkPreserveCalculations;
        private System.Windows.Forms.RadioButton radioAscending;
        private System.Windows.Forms.RadioButton radioDescending;
        private System.Windows.Forms.GroupBox grpLexiographicOrder;
        private System.Windows.Forms.GroupBox grpPhaseOrder;
        private System.Windows.Forms.ListBox lstPhaseOrder;
        private System.Windows.Forms.GroupBox grpApplicable;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnOK;
    }
}