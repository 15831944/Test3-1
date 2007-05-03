namespace ISSHelper
{
  partial class ISSHelperForm
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
      this.baseFolderTextBox = new System.Windows.Forms.TextBox();
      this.resultsTextBox = new System.Windows.Forms.TextBox();
      this.setBaseFolderButton = new System.Windows.Forms.Button();
      this.baseFolderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
      this.foldersCheckedListBox = new System.Windows.Forms.CheckedListBox();
      this.SuspendLayout();
      // 
      // baseFolderTextBox
      // 
      this.baseFolderTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.baseFolderTextBox.Location = new System.Drawing.Point(136, 14);
      this.baseFolderTextBox.Name = "baseFolderTextBox";
      this.baseFolderTextBox.ReadOnly = true;
      this.baseFolderTextBox.Size = new System.Drawing.Size(644, 20);
      this.baseFolderTextBox.TabIndex = 0;
      // 
      // resultsTextBox
      // 
      this.resultsTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.resultsTextBox.Location = new System.Drawing.Point(12, 246);
      this.resultsTextBox.Multiline = true;
      this.resultsTextBox.Name = "resultsTextBox";
      this.resultsTextBox.ReadOnly = true;
      this.resultsTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.resultsTextBox.Size = new System.Drawing.Size(768, 308);
      this.resultsTextBox.TabIndex = 1;
      // 
      // setBaseFolderButton
      // 
      this.setBaseFolderButton.Location = new System.Drawing.Point(12, 12);
      this.setBaseFolderButton.Name = "setBaseFolderButton";
      this.setBaseFolderButton.Size = new System.Drawing.Size(118, 23);
      this.setBaseFolderButton.TabIndex = 3;
      this.setBaseFolderButton.Text = "Set Base Folder";
      this.setBaseFolderButton.UseVisualStyleBackColor = true;
      this.setBaseFolderButton.Click += new System.EventHandler(this.SetBaseFolderButton_Click);
      // 
      // baseFolderBrowserDialog
      // 
      this.baseFolderBrowserDialog.ShowNewFolderButton = false;
      // 
      // foldersCheckedListBox
      // 
      this.foldersCheckedListBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.foldersCheckedListBox.CheckOnClick = true;
      this.foldersCheckedListBox.FormattingEnabled = true;
      this.foldersCheckedListBox.Location = new System.Drawing.Point(12, 41);
      this.foldersCheckedListBox.Name = "foldersCheckedListBox";
      this.foldersCheckedListBox.Size = new System.Drawing.Size(768, 199);
      this.foldersCheckedListBox.Sorted = true;
      this.foldersCheckedListBox.TabIndex = 4;
      this.foldersCheckedListBox.ThreeDCheckBoxes = true;
      this.foldersCheckedListBox.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.foldersCheckedListBox_ItemCheck);
      // 
      // ISSHelperForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(792, 566);
      this.Controls.Add(this.foldersCheckedListBox);
      this.Controls.Add(this.resultsTextBox);
      this.Controls.Add(this.baseFolderTextBox);
      this.Controls.Add(this.setBaseFolderButton);
      this.MinimumSize = new System.Drawing.Size(300, 200);
      this.Name = "ISSHelperForm";
      this.Text = "ISS Helper";
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.ISSHelperForm_FormClosing);
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.TextBox baseFolderTextBox;
    private System.Windows.Forms.TextBox resultsTextBox;
    private System.Windows.Forms.Button setBaseFolderButton;
    private System.Windows.Forms.FolderBrowserDialog baseFolderBrowserDialog;
    private System.Windows.Forms.CheckedListBox foldersCheckedListBox;
  }
}

