namespace Reaction_Editor
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
            this.radioReaction = new System.Windows.Forms.RadioButton();
            this.radioSource = new System.Windows.Forms.RadioButton();
            this.radioSink = new System.Windows.Forms.RadioButton();
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.radioHX = new System.Windows.Forms.RadioButton();
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // radioReaction
            // 
            this.radioReaction.AutoSize = true;
            this.radioReaction.Checked = true;
            this.radioReaction.Cursor = System.Windows.Forms.Cursors.Default;
            this.radioReaction.Location = new System.Drawing.Point(12, 29);
            this.radioReaction.Name = "radioReaction";
            this.radioReaction.Size = new System.Drawing.Size(68, 17);
            this.radioReaction.TabIndex = 0;
            this.radioReaction.TabStop = true;
            this.radioReaction.Text = "&Reaction";
            this.radioReaction.UseVisualStyleBackColor = true;
            // 
            // radioSource
            // 
            this.radioSource.AutoSize = true;
            this.radioSource.Location = new System.Drawing.Point(12, 52);
            this.radioSource.Name = "radioSource";
            this.radioSource.Size = new System.Drawing.Size(59, 17);
            this.radioSource.TabIndex = 1;
            this.radioSource.Text = "So&urce";
            this.radioSource.UseVisualStyleBackColor = true;
            // 
            // radioSink
            // 
            this.radioSink.AutoSize = true;
            this.radioSink.Location = new System.Drawing.Point(12, 75);
            this.radioSink.Name = "radioSink";
            this.radioSink.Size = new System.Drawing.Size(46, 17);
            this.radioSink.TabIndex = 2;
            this.radioSink.Text = "S&ink";
            this.radioSink.UseVisualStyleBackColor = true;
            // 
            // btnOK
            // 
            this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOK.Location = new System.Drawing.Point(12, 121);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 3;
            this.btnOK.Text = "&OK";
            this.btnOK.UseVisualStyleBackColor = true;
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(93, 121);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 4;
            this.btnCancel.Text = "&Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // radioHX
            // 
            this.radioHX.AutoSize = true;
            this.radioHX.Location = new System.Drawing.Point(12, 98);
            this.radioHX.Name = "radioHX";
            this.radioHX.Size = new System.Drawing.Size(99, 17);
            this.radioHX.TabIndex = 5;
            this.radioHX.Text = "&Heat Exchange";
            this.radioHX.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(75, 13);
            this.label1.TabIndex = 6;
            this.label1.Text = "Please Select:";
            // 
            // FrmAddSelector
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(177, 151);
            this.ControlBox = false;
            this.Controls.Add(this.label1);
            this.Controls.Add(this.radioHX);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.radioSink);
            this.Controls.Add(this.radioSource);
            this.Controls.Add(this.radioReaction);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "FrmAddSelector";
            this.Text = "Add";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Label label1;
        public System.Windows.Forms.RadioButton radioReaction;
        public System.Windows.Forms.RadioButton radioSource;
        public System.Windows.Forms.RadioButton radioSink;
        public System.Windows.Forms.RadioButton radioHX;
    }
}