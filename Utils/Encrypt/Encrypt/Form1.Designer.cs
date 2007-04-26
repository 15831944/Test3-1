namespace Encrypt
{
    partial class EncryptProjectForm
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
          this.EncryptProjectButton = new System.Windows.Forms.Button();
          this.CreateBackupCheckBox = new System.Windows.Forms.CheckBox();
          this.EncryptPGMFilesCheckBox = new System.Windows.Forms.CheckBox();
          this.EncryptRCTFilesCheckBox = new System.Windows.Forms.CheckBox();
          this.EncryptDXFFiles = new System.Windows.Forms.CheckBox();
          this.ProjectListBox = new System.Windows.Forms.ListBox();
          this.ProjectsLabel = new System.Windows.Forms.Label();
          this.EncryptGroupBox = new System.Windows.Forms.GroupBox();
          this.StatusStrip = new System.Windows.Forms.StatusStrip();
          this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
          this.ProgressToolStripProgressBar = new System.Windows.Forms.ToolStripProgressBar();
          this.EncryptGroupBox.SuspendLayout();
          this.StatusStrip.SuspendLayout();
          this.SuspendLayout();
          // 
          // EncryptProjectButton
          // 
          this.EncryptProjectButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.EncryptProjectButton.Location = new System.Drawing.Point(12, 180);
          this.EncryptProjectButton.Name = "EncryptProjectButton";
          this.EncryptProjectButton.Size = new System.Drawing.Size(380, 23);
          this.EncryptProjectButton.TabIndex = 1;
          this.EncryptProjectButton.Text = "Encrypt Project";
          this.EncryptProjectButton.UseVisualStyleBackColor = true;
          this.EncryptProjectButton.Click += new System.EventHandler(this.EncryptProjectButton_Click);
          // 
          // CreateBackupCheckBox
          // 
          this.CreateBackupCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
          this.CreateBackupCheckBox.Checked = true;
          this.CreateBackupCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
          this.CreateBackupCheckBox.Location = new System.Drawing.Point(295, 28);
          this.CreateBackupCheckBox.Name = "CreateBackupCheckBox";
          this.CreateBackupCheckBox.Size = new System.Drawing.Size(97, 17);
          this.CreateBackupCheckBox.TabIndex = 2;
          this.CreateBackupCheckBox.Text = "Create Backup";
          this.CreateBackupCheckBox.UseVisualStyleBackColor = true;
          // 
          // EncryptPGMFilesCheckBox
          // 
          this.EncryptPGMFilesCheckBox.AutoSize = true;
          this.EncryptPGMFilesCheckBox.Checked = true;
          this.EncryptPGMFilesCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
          this.EncryptPGMFilesCheckBox.Location = new System.Drawing.Point(6, 16);
          this.EncryptPGMFilesCheckBox.Name = "EncryptPGMFilesCheckBox";
          this.EncryptPGMFilesCheckBox.Size = new System.Drawing.Size(74, 17);
          this.EncryptPGMFilesCheckBox.TabIndex = 3;
          this.EncryptPGMFilesCheckBox.Text = "PGM Files";
          this.EncryptPGMFilesCheckBox.UseVisualStyleBackColor = true;
          // 
          // EncryptRCTFilesCheckBox
          // 
          this.EncryptRCTFilesCheckBox.AutoSize = true;
          this.EncryptRCTFilesCheckBox.Checked = true;
          this.EncryptRCTFilesCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
          this.EncryptRCTFilesCheckBox.Location = new System.Drawing.Point(86, 16);
          this.EncryptRCTFilesCheckBox.Name = "EncryptRCTFilesCheckBox";
          this.EncryptRCTFilesCheckBox.Size = new System.Drawing.Size(72, 17);
          this.EncryptRCTFilesCheckBox.TabIndex = 4;
          this.EncryptRCTFilesCheckBox.Text = "RCT Files";
          this.EncryptRCTFilesCheckBox.UseVisualStyleBackColor = true;
          // 
          // EncryptDXFFiles
          // 
          this.EncryptDXFFiles.AutoSize = true;
          this.EncryptDXFFiles.Checked = true;
          this.EncryptDXFFiles.CheckState = System.Windows.Forms.CheckState.Checked;
          this.EncryptDXFFiles.Location = new System.Drawing.Point(164, 16);
          this.EncryptDXFFiles.Name = "EncryptDXFFiles";
          this.EncryptDXFFiles.Size = new System.Drawing.Size(71, 17);
          this.EncryptDXFFiles.TabIndex = 5;
          this.EncryptDXFFiles.Text = "DXF Files";
          this.EncryptDXFFiles.UseVisualStyleBackColor = true;
          // 
          // ProjectListBox
          // 
          this.ProjectListBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.ProjectListBox.FormattingEnabled = true;
          this.ProjectListBox.HorizontalScrollbar = true;
          this.ProjectListBox.Location = new System.Drawing.Point(12, 79);
          this.ProjectListBox.Name = "ProjectListBox";
          this.ProjectListBox.Size = new System.Drawing.Size(380, 95);
          this.ProjectListBox.TabIndex = 6;
          this.ProjectListBox.DoubleClick += new System.EventHandler(this.ProjectListBox_DoubleClick);
          // 
          // ProjectsLabel
          // 
          this.ProjectsLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.ProjectsLabel.AutoSize = true;
          this.ProjectsLabel.Location = new System.Drawing.Point(12, 63);
          this.ProjectsLabel.Name = "ProjectsLabel";
          this.ProjectsLabel.Size = new System.Drawing.Size(48, 13);
          this.ProjectsLabel.TabIndex = 7;
          this.ProjectsLabel.Text = "Projects:";
          // 
          // EncryptGroupBox
          // 
          this.EncryptGroupBox.Controls.Add(this.EncryptPGMFilesCheckBox);
          this.EncryptGroupBox.Controls.Add(this.EncryptRCTFilesCheckBox);
          this.EncryptGroupBox.Controls.Add(this.EncryptDXFFiles);
          this.EncryptGroupBox.Location = new System.Drawing.Point(15, 12);
          this.EncryptGroupBox.Name = "EncryptGroupBox";
          this.EncryptGroupBox.Size = new System.Drawing.Size(238, 43);
          this.EncryptGroupBox.TabIndex = 9;
          this.EncryptGroupBox.TabStop = false;
          this.EncryptGroupBox.Text = "Encrypt";
          // 
          // StatusStrip
          // 
          this.StatusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ProgressToolStripProgressBar,
            this.toolStripStatusLabel1});
          this.StatusStrip.Location = new System.Drawing.Point(0, 206);
          this.StatusStrip.Name = "StatusStrip";
          this.StatusStrip.Size = new System.Drawing.Size(404, 22);
          this.StatusStrip.SizingGrip = false;
          this.StatusStrip.TabIndex = 10;
          // 
          // toolStripStatusLabel1
          // 
          this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
          this.toolStripStatusLabel1.Size = new System.Drawing.Size(0, 17);
          // 
          // ProgressToolStripProgressBar
          // 
          this.ProgressToolStripProgressBar.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
          this.ProgressToolStripProgressBar.Name = "ProgressToolStripProgressBar";
          this.ProgressToolStripProgressBar.Size = new System.Drawing.Size(100, 16);
          this.ProgressToolStripProgressBar.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
          // 
          // EncryptProjectForm
          // 
          this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
          this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
          this.ClientSize = new System.Drawing.Size(404, 228);
          this.Controls.Add(this.StatusStrip);
          this.Controls.Add(this.EncryptGroupBox);
          this.Controls.Add(this.ProjectsLabel);
          this.Controls.Add(this.ProjectListBox);
          this.Controls.Add(this.CreateBackupCheckBox);
          this.Controls.Add(this.EncryptProjectButton);
          this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
          this.Name = "EncryptProjectForm";
          this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
          this.Text = "Encrypt Project";
          this.Load += new System.EventHandler(this.EncryptProjectForm_Load);
          this.EncryptGroupBox.ResumeLayout(false);
          this.EncryptGroupBox.PerformLayout();
          this.StatusStrip.ResumeLayout(false);
          this.StatusStrip.PerformLayout();
          this.ResumeLayout(false);
          this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button EncryptProjectButton;
      private System.Windows.Forms.CheckBox CreateBackupCheckBox;
      private System.Windows.Forms.CheckBox EncryptPGMFilesCheckBox;
      private System.Windows.Forms.CheckBox EncryptRCTFilesCheckBox;
      private System.Windows.Forms.CheckBox EncryptDXFFiles;
      private System.Windows.Forms.ListBox ProjectListBox;
      private System.Windows.Forms.Label ProjectsLabel;
      private System.Windows.Forms.GroupBox EncryptGroupBox;
      private System.Windows.Forms.StatusStrip StatusStrip;
      private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
      private System.Windows.Forms.ToolStripProgressBar ProgressToolStripProgressBar;
    }
}

