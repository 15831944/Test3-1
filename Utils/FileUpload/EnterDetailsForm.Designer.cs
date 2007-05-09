namespace BasecampAPI
{
  partial class EnterDetailsForm
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
      this.usernameTextBox = new System.Windows.Forms.TextBox();
      this.siteTextBox = new System.Windows.Forms.TextBox();
      this.passwordTextBox = new System.Windows.Forms.TextBox();
      this.Instruction = new System.Windows.Forms.Label();
      this.siteLabel = new System.Windows.Forms.Label();
      this.usernameLabel = new System.Windows.Forms.Label();
      this.passwordLabel = new System.Windows.Forms.Label();
      this.okButton = new System.Windows.Forms.Button();
      this.cancelButton = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // usernameTextBox
      // 
      this.usernameTextBox.Location = new System.Drawing.Point(76, 67);
      this.usernameTextBox.Name = "usernameTextBox";
      this.usernameTextBox.Size = new System.Drawing.Size(204, 20);
      this.usernameTextBox.TabIndex = 1;
      // 
      // siteTextBox
      // 
      this.siteTextBox.Location = new System.Drawing.Point(76, 41);
      this.siteTextBox.Name = "siteTextBox";
      this.siteTextBox.Size = new System.Drawing.Size(204, 20);
      this.siteTextBox.TabIndex = 0;
      // 
      // passwordTextBox
      // 
      this.passwordTextBox.Location = new System.Drawing.Point(76, 93);
      this.passwordTextBox.Name = "passwordTextBox";
      this.passwordTextBox.PasswordChar = '*';
      this.passwordTextBox.Size = new System.Drawing.Size(204, 20);
      this.passwordTextBox.TabIndex = 2;
      // 
      // Instruction
      // 
      this.Instruction.AutoSize = true;
      this.Instruction.Location = new System.Drawing.Point(12, 9);
      this.Instruction.Name = "Instruction";
      this.Instruction.Size = new System.Drawing.Size(200, 13);
      this.Instruction.TabIndex = 0;
      this.Instruction.Text = "Please enter the connection details here:";
      // 
      // siteLabel
      // 
      this.siteLabel.AutoSize = true;
      this.siteLabel.Location = new System.Drawing.Point(12, 44);
      this.siteLabel.Name = "siteLabel";
      this.siteLabel.Size = new System.Drawing.Size(28, 13);
      this.siteLabel.TabIndex = 0;
      this.siteLabel.Text = "Site:";
      // 
      // usernameLabel
      // 
      this.usernameLabel.AutoSize = true;
      this.usernameLabel.Location = new System.Drawing.Point(12, 70);
      this.usernameLabel.Name = "usernameLabel";
      this.usernameLabel.Size = new System.Drawing.Size(58, 13);
      this.usernameLabel.TabIndex = 0;
      this.usernameLabel.Text = "Username:";
      // 
      // passwordLabel
      // 
      this.passwordLabel.AutoSize = true;
      this.passwordLabel.Location = new System.Drawing.Point(12, 96);
      this.passwordLabel.Name = "passwordLabel";
      this.passwordLabel.Size = new System.Drawing.Size(56, 13);
      this.passwordLabel.TabIndex = 0;
      this.passwordLabel.Text = "Password:";
      // 
      // okButton
      // 
      this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.okButton.Location = new System.Drawing.Point(124, 133);
      this.okButton.Name = "okButton";
      this.okButton.Size = new System.Drawing.Size(75, 23);
      this.okButton.TabIndex = 4;
      this.okButton.Text = "OK";
      this.okButton.UseVisualStyleBackColor = true;
      // 
      // cancelButton
      // 
      this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.cancelButton.Location = new System.Drawing.Point(205, 133);
      this.cancelButton.Name = "cancelButton";
      this.cancelButton.Size = new System.Drawing.Size(75, 23);
      this.cancelButton.TabIndex = 5;
      this.cancelButton.Text = "Cancel";
      this.cancelButton.UseVisualStyleBackColor = true;
      // 
      // EnterDetailsForm
      // 
      this.AcceptButton = this.okButton;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.CancelButton = this.cancelButton;
      this.ClientSize = new System.Drawing.Size(292, 168);
      this.Controls.Add(this.cancelButton);
      this.Controls.Add(this.okButton);
      this.Controls.Add(this.passwordLabel);
      this.Controls.Add(this.usernameLabel);
      this.Controls.Add(this.siteLabel);
      this.Controls.Add(this.Instruction);
      this.Controls.Add(this.passwordTextBox);
      this.Controls.Add(this.siteTextBox);
      this.Controls.Add(this.usernameTextBox);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "EnterDetailsForm";
      this.Text = "Enter Details";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Label Instruction;
    private System.Windows.Forms.Label siteLabel;
    private System.Windows.Forms.Label usernameLabel;
    private System.Windows.Forms.Label passwordLabel;
    private System.Windows.Forms.Button okButton;
    private System.Windows.Forms.Button cancelButton;
    public System.Windows.Forms.TextBox usernameTextBox;
    public System.Windows.Forms.TextBox siteTextBox;
    public System.Windows.Forms.TextBox passwordTextBox;
  }
}