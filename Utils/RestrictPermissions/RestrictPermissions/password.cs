using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace RestrictPermissions
{
	/// <summary>
	/// Summary description for password.
	/// </summary>
	public class passwordDlg : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.TextBox passField;
		private System.Windows.Forms.TextBox confirmField;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label entropyStatus;
		private System.Windows.Forms.Button okBtn;
		private System.Windows.Forms.Button cancelBtn;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public passwordDlg()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(passwordDlg));
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.passField = new System.Windows.Forms.TextBox();
			this.confirmField = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.entropyStatus = new System.Windows.Forms.Label();
			this.okBtn = new System.Windows.Forms.Button();
			this.cancelBtn = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.label1.Location = new System.Drawing.Point(8, 8);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(136, 16);
			this.label1.TabIndex = 0;
			this.label1.Text = "Please enter a password.";
			// 
			// label2
			// 
			this.label2.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.label2.Location = new System.Drawing.Point(8, 32);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(304, 40);
			this.label2.TabIndex = 1;
			this.label2.Text = "It is recommended that you enter a strong password by using a combination of A-Z," +
				" a-z, 0-9, and punctuation marks.";
			// 
			// label3
			// 
			this.label3.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.label3.Location = new System.Drawing.Point(8, 80);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(48, 16);
			this.label3.TabIndex = 2;
			this.label3.Text = "Password:";
			this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// passField
			// 
			this.passField.Location = new System.Drawing.Point(64, 80);
			this.passField.MaxLength = 64;
			this.passField.Name = "passField";
			this.passField.PasswordChar = '*';
			this.passField.Size = new System.Drawing.Size(200, 20);
			this.passField.TabIndex = 3;
			this.passField.Text = "";
			this.passField.TextChanged += new System.EventHandler(this.passField_TextChanged);
			// 
			// confirmField
			// 
			this.confirmField.Location = new System.Drawing.Point(64, 104);
			this.confirmField.MaxLength = 64;
			this.confirmField.Name = "confirmField";
			this.confirmField.PasswordChar = '*';
			this.confirmField.Size = new System.Drawing.Size(200, 20);
			this.confirmField.TabIndex = 5;
			this.confirmField.Text = "";
			// 
			// label4
			// 
			this.label4.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.label4.Location = new System.Drawing.Point(16, 104);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(40, 16);
			this.label4.TabIndex = 4;
			this.label4.Text = "Confirm:";
			this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// entropyStatus
			// 
			this.entropyStatus.Location = new System.Drawing.Point(8, 136);
			this.entropyStatus.Name = "entropyStatus";
			this.entropyStatus.Size = new System.Drawing.Size(312, 32);
			this.entropyStatus.TabIndex = 6;
			this.entropyStatus.Text = "Waiting for password entry...";
			this.entropyStatus.Click += new System.EventHandler(this.entropyStatus_Click);
			// 
			// okBtn
			// 
			this.okBtn.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.okBtn.Location = new System.Drawing.Point(160, 168);
			this.okBtn.Name = "okBtn";
			this.okBtn.TabIndex = 7;
			this.okBtn.Text = "Ok";
			this.okBtn.Click += new System.EventHandler(this.okBtn_Click);
			// 
			// cancelBtn
			// 
			this.cancelBtn.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cancelBtn.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.cancelBtn.Location = new System.Drawing.Point(240, 168);
			this.cancelBtn.Name = "cancelBtn";
			this.cancelBtn.TabIndex = 8;
			this.cancelBtn.Text = "Cancel";
			// 
			// passwordDlg
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(320, 197);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.cancelBtn,
																		  this.okBtn,
																		  this.entropyStatus,
																		  this.confirmField,
																		  this.label4,
																		  this.passField,
																		  this.label3,
																		  this.label2,
																		  this.label1});
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "passwordDlg";
			this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Please Enter a Password";
			this.Load += new System.EventHandler(this.passwordDlg_Load);
			this.ResumeLayout(false);

		}
		#endregion

		private void passwordDlg_Load(object sender, System.EventArgs e)
		{
		
		}

		private void okBtn_Click(object sender, System.EventArgs e)
		{
			if( passField.TextLength == 0 )
			{
				MessageBox.Show( this, 
					"Please enter a valid password here.", "Input Error", 
					MessageBoxButtons.OK, MessageBoxIcon.Error );
				
				return;
			}

			if( confirmField.TextLength == 0 )
			{
				MessageBox.Show( this, 
					"Please re-enter the password to confirm it.", "Input Error", 
					MessageBoxButtons.OK, MessageBoxIcon.Error );
								
				return;
			}

			if( !passField.Text.Equals( confirmField.Text ) )
			{
				// Fields don't match
				MessageBox.Show( this, 
					"The passwords do not match up. Please try again.", "Passwords don't match!", 
					MessageBoxButtons.OK, MessageBoxIcon.Error );
								
				return;
			}
			
			// If we get here, then its ok to dispose of the dialog and set DialogResult to OK
			DialogResult = DialogResult.OK;
			Close();
		
		}

		public System.String getPass()
		{
			return passField.Text;
		}

		private void passField_TextChanged(object sender, System.EventArgs e)
		{
			checkEffectiveBitSize( passField.TextLength );
		}

		private void checkEffectiveBitSize( int passSize )
		{
			int charSet = 0;
			string passStrength = "";

			charSet = getCharSetUsed( passField.Text );

			double result = Math.Log( Math.Pow( charSet, passSize ) ) / Math.Log( 2 );

			if( result <= 32 )
			{
				passStrength = "weak;";
			}
			else if( result <= 64 )
			{
				passStrength = "mediocre;";
			}
			else if( result <= 128 )
			{
				passStrength = "OK;";
			}
			else if( result > 128 )
			{
				passStrength = "great;";
			}

			entropyStatus.Text = "Your password is " + passStrength +
				" it is equivalent to a " + Math.Round( result, 0 ) + "-bit key.";
				
		}

		private int getCharSetUsed( string pass )
		{
			int ret = 0;

			if( containsNumbers( pass ) )
			{
				ret += 10;
			}

			if( containsLowerCaseChars( pass ) )
			{
				ret += 26;
			}

			if( containsUpperCaseChars( pass ) )
			{
				ret += 26;
			}

			if( containsPunctuation( pass ) )
			{
				ret += 31;
			}

			return ret;
		}

		private bool containsNumbers( string str )
		{
			Regex pattern = new Regex( @"[\d]" );
			return pattern.IsMatch( str );
		}

		private bool containsLowerCaseChars( string str )
		{
			Regex pattern = new Regex( "[a-z]" );
			return pattern.IsMatch( str );
		}

		private bool containsUpperCaseChars( string str )
		{
			Regex pattern = new Regex( "[A-Z]" );
			return pattern.IsMatch( str );
		}

		private bool containsPunctuation( string str )
		{
			// regular expression include _ as a valid char for alphanumeric.. 
			// so we need to explicity state that its considered punctuation.
			Regex pattern = new Regex( @"[\W|_]" );
			return pattern.IsMatch( str );
		}

		private void entropyStatus_Click(object sender, System.EventArgs e)
		{
		
		}
	}
			
}
