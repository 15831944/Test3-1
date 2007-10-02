namespace Reaction_Editor
{
    partial class FrmChangeDialogue
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
            this.txtCurReactants = new System.Windows.Forms.RichTextBox();
            this.txtCurProducts = new System.Windows.Forms.RichTextBox();
            this.txtCurDirection = new System.Windows.Forms.RichTextBox();
            this.txtMessage = new System.Windows.Forms.RichTextBox();
            this.txtCurExtent = new System.Windows.Forms.RichTextBox();
            this.txtCurSeq = new System.Windows.Forms.RichTextBox();
            this.txtCurHOR = new System.Windows.Forms.RichTextBox();
            this.grpCurrentReaction = new System.Windows.Forms.GroupBox();
            this.grpNewReaction = new System.Windows.Forms.GroupBox();
            this.txtNewReactants = new System.Windows.Forms.RichTextBox();
            this.txtNewHOR = new System.Windows.Forms.RichTextBox();
            this.txtNewProducts = new System.Windows.Forms.RichTextBox();
            this.txtNewSeq = new System.Windows.Forms.RichTextBox();
            this.txtNewDirection = new System.Windows.Forms.RichTextBox();
            this.txtNewExtent = new System.Windows.Forms.RichTextBox();
            this.btnOk = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.txtCurGeneric = new System.Windows.Forms.RichTextBox();
            this.txtNewGeneric = new System.Windows.Forms.RichTextBox();
            this.grpCurrentReaction.SuspendLayout();
            this.grpNewReaction.SuspendLayout();
            this.SuspendLayout();
            // 
            // txtCurReactants
            // 
            this.txtCurReactants.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtCurReactants.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.txtCurReactants.Location = new System.Drawing.Point(6, 19);
            this.txtCurReactants.Multiline = false;
            this.txtCurReactants.Name = "txtCurReactants";
            this.txtCurReactants.ReadOnly = true;
            this.txtCurReactants.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.txtCurReactants.Size = new System.Drawing.Size(284, 20);
            this.txtCurReactants.TabIndex = 0;
            this.txtCurReactants.Text = "CurReactants";
            this.txtCurReactants.Enter += new System.EventHandler(this.richTextBox4_Enter);
            // 
            // txtCurProducts
            // 
            this.txtCurProducts.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtCurProducts.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.txtCurProducts.Location = new System.Drawing.Point(350, 19);
            this.txtCurProducts.Multiline = false;
            this.txtCurProducts.Name = "txtCurProducts";
            this.txtCurProducts.ReadOnly = true;
            this.txtCurProducts.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.txtCurProducts.Size = new System.Drawing.Size(271, 20);
            this.txtCurProducts.TabIndex = 1;
            this.txtCurProducts.Text = "CurProducts";
            this.txtCurProducts.Enter += new System.EventHandler(this.richTextBox4_Enter);
            // 
            // txtCurDirection
            // 
            this.txtCurDirection.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtCurDirection.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.txtCurDirection.Location = new System.Drawing.Point(296, 19);
            this.txtCurDirection.Multiline = false;
            this.txtCurDirection.Name = "txtCurDirection";
            this.txtCurDirection.ReadOnly = true;
            this.txtCurDirection.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.txtCurDirection.Size = new System.Drawing.Size(24, 20);
            this.txtCurDirection.TabIndex = 2;
            this.txtCurDirection.Text = "->";
            this.txtCurDirection.Enter += new System.EventHandler(this.richTextBox4_Enter);
            // 
            // txtMessage
            // 
            this.txtMessage.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtMessage.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.txtMessage.Location = new System.Drawing.Point(11, 12);
            this.txtMessage.Multiline = false;
            this.txtMessage.Name = "txtMessage";
            this.txtMessage.ReadOnly = true;
            this.txtMessage.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.txtMessage.Size = new System.Drawing.Size(627, 20);
            this.txtMessage.TabIndex = 3;
            this.txtMessage.Text = "Are you sure you wish to make the change:";
            this.txtMessage.Enter += new System.EventHandler(this.richTextBox4_Enter);
            // 
            // txtCurExtent
            // 
            this.txtCurExtent.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtCurExtent.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtCurExtent.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.txtCurExtent.Location = new System.Drawing.Point(6, 45);
            this.txtCurExtent.Multiline = false;
            this.txtCurExtent.Name = "txtCurExtent";
            this.txtCurExtent.ReadOnly = true;
            this.txtCurExtent.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.txtCurExtent.Size = new System.Drawing.Size(615, 20);
            this.txtCurExtent.TabIndex = 4;
            this.txtCurExtent.Text = "curExtent";
            this.txtCurExtent.Enter += new System.EventHandler(this.richTextBox4_Enter);
            // 
            // txtCurSeq
            // 
            this.txtCurSeq.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtCurSeq.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtCurSeq.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.txtCurSeq.Location = new System.Drawing.Point(6, 97);
            this.txtCurSeq.Multiline = false;
            this.txtCurSeq.Name = "txtCurSeq";
            this.txtCurSeq.ReadOnly = true;
            this.txtCurSeq.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.txtCurSeq.Size = new System.Drawing.Size(615, 20);
            this.txtCurSeq.TabIndex = 5;
            this.txtCurSeq.Text = "curSeq";
            this.txtCurSeq.Enter += new System.EventHandler(this.richTextBox4_Enter);
            // 
            // txtCurHOR
            // 
            this.txtCurHOR.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtCurHOR.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtCurHOR.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.txtCurHOR.Location = new System.Drawing.Point(6, 71);
            this.txtCurHOR.Multiline = false;
            this.txtCurHOR.Name = "txtCurHOR";
            this.txtCurHOR.ReadOnly = true;
            this.txtCurHOR.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.txtCurHOR.Size = new System.Drawing.Size(615, 20);
            this.txtCurHOR.TabIndex = 6;
            this.txtCurHOR.Text = "curHOR";
            this.txtCurHOR.Enter += new System.EventHandler(this.richTextBox4_Enter);
            // 
            // grpCurrentReaction
            // 
            this.grpCurrentReaction.Controls.Add(this.txtCurReactants);
            this.grpCurrentReaction.Controls.Add(this.txtCurHOR);
            this.grpCurrentReaction.Controls.Add(this.txtCurProducts);
            this.grpCurrentReaction.Controls.Add(this.txtCurSeq);
            this.grpCurrentReaction.Controls.Add(this.txtCurDirection);
            this.grpCurrentReaction.Controls.Add(this.txtCurExtent);
            this.grpCurrentReaction.Controls.Add(this.txtCurGeneric);
            this.grpCurrentReaction.Location = new System.Drawing.Point(12, 38);
            this.grpCurrentReaction.Name = "grpCurrentReaction";
            this.grpCurrentReaction.Size = new System.Drawing.Size(630, 128);
            this.grpCurrentReaction.TabIndex = 7;
            this.grpCurrentReaction.TabStop = false;
            this.grpCurrentReaction.Text = "Current Reaction";
            // 
            // grpNewReaction
            // 
            this.grpNewReaction.Controls.Add(this.txtNewReactants);
            this.grpNewReaction.Controls.Add(this.txtNewHOR);
            this.grpNewReaction.Controls.Add(this.txtNewProducts);
            this.grpNewReaction.Controls.Add(this.txtNewSeq);
            this.grpNewReaction.Controls.Add(this.txtNewDirection);
            this.grpNewReaction.Controls.Add(this.txtNewExtent);
            this.grpNewReaction.Controls.Add(this.txtNewGeneric);
            this.grpNewReaction.Location = new System.Drawing.Point(12, 172);
            this.grpNewReaction.Name = "grpNewReaction";
            this.grpNewReaction.Size = new System.Drawing.Size(630, 131);
            this.grpNewReaction.TabIndex = 8;
            this.grpNewReaction.TabStop = false;
            this.grpNewReaction.Text = "New Reaction";
            // 
            // txtNewReactants
            // 
            this.txtNewReactants.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtNewReactants.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.txtNewReactants.Location = new System.Drawing.Point(6, 19);
            this.txtNewReactants.Multiline = false;
            this.txtNewReactants.Name = "txtNewReactants";
            this.txtNewReactants.ReadOnly = true;
            this.txtNewReactants.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.txtNewReactants.Size = new System.Drawing.Size(284, 20);
            this.txtNewReactants.TabIndex = 0;
            this.txtNewReactants.Text = "2 Na3AlF6(s) + Na2CO3(l) + 10 Ca[OH]2(s) + 5 H2O(l) ";
            this.txtNewReactants.Enter += new System.EventHandler(this.richTextBox4_Enter);
            // 
            // txtNewHOR
            // 
            this.txtNewHOR.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtNewHOR.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtNewHOR.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.txtNewHOR.Location = new System.Drawing.Point(6, 71);
            this.txtNewHOR.Multiline = false;
            this.txtNewHOR.Name = "txtNewHOR";
            this.txtNewHOR.ReadOnly = true;
            this.txtNewHOR.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.txtNewHOR.Size = new System.Drawing.Size(615, 20);
            this.txtNewHOR.TabIndex = 6;
            this.txtNewHOR.Text = "newHOR";
            this.txtNewHOR.Enter += new System.EventHandler(this.richTextBox4_Enter);
            // 
            // txtNewProducts
            // 
            this.txtNewProducts.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtNewProducts.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.txtNewProducts.Location = new System.Drawing.Point(350, 19);
            this.txtNewProducts.Multiline = false;
            this.txtNewProducts.Name = "txtNewProducts";
            this.txtNewProducts.ReadOnly = true;
            this.txtNewProducts.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.txtNewProducts.Size = new System.Drawing.Size(271, 20);
            this.txtNewProducts.TabIndex = 1;
            this.txtNewProducts.Text = "3CaO.Al2O3.CaCO3.11H2O(s) + 6 CaF2(s) + 8 NaOH(l)";
            this.txtNewProducts.Enter += new System.EventHandler(this.richTextBox4_Enter);
            // 
            // txtNewSeq
            // 
            this.txtNewSeq.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtNewSeq.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtNewSeq.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.txtNewSeq.Location = new System.Drawing.Point(6, 97);
            this.txtNewSeq.Multiline = false;
            this.txtNewSeq.Name = "txtNewSeq";
            this.txtNewSeq.ReadOnly = true;
            this.txtNewSeq.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.txtNewSeq.Size = new System.Drawing.Size(615, 20);
            this.txtNewSeq.TabIndex = 5;
            this.txtNewSeq.Text = "newSeq";
            this.txtNewSeq.Enter += new System.EventHandler(this.richTextBox4_Enter);
            // 
            // txtNewDirection
            // 
            this.txtNewDirection.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtNewDirection.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.txtNewDirection.Location = new System.Drawing.Point(296, 19);
            this.txtNewDirection.Multiline = false;
            this.txtNewDirection.Name = "txtNewDirection";
            this.txtNewDirection.ReadOnly = true;
            this.txtNewDirection.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.txtNewDirection.Size = new System.Drawing.Size(24, 20);
            this.txtNewDirection.TabIndex = 2;
            this.txtNewDirection.Text = "=";
            this.txtNewDirection.Enter += new System.EventHandler(this.richTextBox4_Enter);
            // 
            // txtNewExtent
            // 
            this.txtNewExtent.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtNewExtent.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtNewExtent.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.txtNewExtent.Location = new System.Drawing.Point(6, 45);
            this.txtNewExtent.Multiline = false;
            this.txtNewExtent.Name = "txtNewExtent";
            this.txtNewExtent.ReadOnly = true;
            this.txtNewExtent.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.None;
            this.txtNewExtent.Size = new System.Drawing.Size(615, 20);
            this.txtNewExtent.TabIndex = 4;
            this.txtNewExtent.Text = "newExtent";
            this.txtNewExtent.Enter += new System.EventHandler(this.richTextBox4_Enter);
            // 
            // btnOk
            // 
            this.btnOk.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOk.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOk.Location = new System.Drawing.Point(486, 309);
            this.btnOk.Name = "btnOk";
            this.btnOk.Size = new System.Drawing.Size(75, 23);
            this.btnOk.TabIndex = 9;
            this.btnOk.Text = "OK";
            this.btnOk.UseVisualStyleBackColor = true;
            this.btnOk.Enter += new System.EventHandler(this.btnOk_Enter);
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(567, 309);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 10;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Enter += new System.EventHandler(this.btnOk_Enter);
            // 
            // txtCurGeneric
            // 
            this.txtCurGeneric.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtCurGeneric.Location = new System.Drawing.Point(6, 16);
            this.txtCurGeneric.Name = "txtCurGeneric";
            this.txtCurGeneric.ReadOnly = true;
            this.txtCurGeneric.Size = new System.Drawing.Size(615, 49);
            this.txtCurGeneric.TabIndex = 7;
            this.txtCurGeneric.Text = "curGeneric";
            // 
            // txtNewGeneric
            // 
            this.txtNewGeneric.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtNewGeneric.Location = new System.Drawing.Point(6, 19);
            this.txtNewGeneric.Name = "txtNewGeneric";
            this.txtNewGeneric.ReadOnly = true;
            this.txtNewGeneric.Size = new System.Drawing.Size(615, 49);
            this.txtNewGeneric.TabIndex = 8;
            this.txtNewGeneric.Text = "curGeneric";
            // 
            // FrmChangeDialogue
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(654, 344);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOk);
            this.Controls.Add(this.grpNewReaction);
            this.Controls.Add(this.grpCurrentReaction);
            this.Controls.Add(this.txtMessage);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FrmChangeDialogue";
            this.Text = "FrmChangeDialogue";
            this.grpCurrentReaction.ResumeLayout(false);
            this.grpNewReaction.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.RichTextBox txtCurReactants;
        private System.Windows.Forms.RichTextBox txtCurProducts;
        private System.Windows.Forms.RichTextBox txtCurDirection;
        private System.Windows.Forms.RichTextBox txtMessage;
        private System.Windows.Forms.RichTextBox txtCurExtent;
        private System.Windows.Forms.RichTextBox txtCurSeq;
        private System.Windows.Forms.RichTextBox txtCurHOR;
        private System.Windows.Forms.GroupBox grpCurrentReaction;
        private System.Windows.Forms.GroupBox grpNewReaction;
        private System.Windows.Forms.RichTextBox txtNewReactants;
        private System.Windows.Forms.RichTextBox txtNewHOR;
        private System.Windows.Forms.RichTextBox txtNewProducts;
        private System.Windows.Forms.RichTextBox txtNewSeq;
        private System.Windows.Forms.RichTextBox txtNewDirection;
        private System.Windows.Forms.RichTextBox txtNewExtent;
        private System.Windows.Forms.Button btnOk;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.RichTextBox txtCurGeneric;
        private System.Windows.Forms.RichTextBox txtNewGeneric;
    }
}