using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;
using System.Drawing;
using Reaction_Editor;
using System.ComponentModel;
using Be.Windows.Forms;

namespace Auto_Complete
{
    class BoxAutoComplete : RichTextBoxEx
    {
        #region Variables
        protected FrmAutoComplete m_AutoForm = new FrmAutoComplete();
        #endregion Variables

        #region Constructors
        public BoxAutoComplete()
            : base()
        {
            m_AutoForm.TopMost = true;
            m_AutoForm.OwnerBox = this;
        }
        #endregion Constructors

        #region Properties
        public ListBox.ObjectCollection Items
        {
            get { return m_AutoForm.MainControl.Items; }
        }

        public Form FrmOwner
        {
            set { m_AutoForm.Owner = value; }
        }

        public Dictionary<object, int> HitCounts
        {
            get { return m_AutoForm.MainControl.m_HitCounts; }
            set { m_AutoForm.MainControl.m_HitCounts = value; }
        }
        #endregion Properties

        #region Overrides
        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            if (!m_AutoForm.Visible)
            {
                if (char.IsLetter(e.KeyChar) || e.KeyChar == '[' || e.KeyChar == '(')
                {
                    string s = this.Text.Substring(0, this.SelectionStart) + e.KeyChar;
                    Match m = FrmAutoComplete.s_AutoCompleteOpeningRegex.Match(s);
                    if (m.Success)
                    {
                        //m_AutoForm.AssociatedControl = this;
                        m_AutoForm.SetFilter(m.Groups["Last"].Value);
                        Point p = this.PointToScreen(this.GetPositionFromCharIndex(this.SelectionStart));
                        p.Y += (int)this.Font.GetHeight();

                        Control c = this.Parent;
                        while (c.Parent != null)
                            c = c.Parent;
                        m_AutoForm.Show();
                        m_AutoForm.Location = p;
                        ((Form)c).Activate();
                    }
                }
            }
            else //We have the autocomplete box open already, update it or clear it.
            {
                string s = this.Text.Substring(0, this.SelectionStart) + e.KeyChar;
                Match m = FrmAutoComplete.s_AutoCompleteRegex.Match(s);
                if (m.Success)
                    m_AutoForm.SetFilter(m.Groups["Last"].Value);
                else
                {
                    if (e.KeyChar == ' ' || e.KeyChar == ',' || e.KeyChar == '+')
                        if (m_AutoForm.HotSelected)
                            m_AutoForm.InsertText();
                    m_AutoForm.Hide();
                }
            }
            base.OnKeyPress(e);
        }

        //Handles up, down (Sends to auto complete), left, right, and esc (Hides autocomplete)
        protected override void OnKeyDown(KeyEventArgs e)
        {
            bool handled = false;
            if (e.KeyCode == Keys.Left || e.KeyCode == Keys.Right)
                m_AutoForm.Hide();

            if (e.KeyCode == Keys.Up || e.KeyCode == Keys.Down)
            {
                m_AutoForm.HandleKey(e);
                e.Handled = true;
            }

            if (e.KeyCode == Keys.Escape)
            {
                m_AutoForm.Hide();
                e.Handled = true;
            }
            if (!handled)
                base.OnKeyDown(e);
        }

        internal bool DoProcessCmdKey(ref Message m, Keys keyData) { return ProcessCmdKey(ref m, keyData); }
        protected override bool ProcessCmdKey(ref Message m, Keys keyData)
        {
            bool handled = false;
            if (m_AutoForm.Visible)
            {
                if (keyData == Keys.Tab || keyData == Keys.Enter || keyData == Keys.Return)
                {
                    m_AutoForm.InsertText();
                    handled = true;
                }
            }

            //To be included later (number tabbing):
            /*else if (keyData == (Keys.Tab | Keys.Shift))
            {
                Match m = s_NumRegexRtoL.Match(this.Text, this.SelectionStart);
                if (m.Success)
                {
                    handled = true;
                    this.Select(m.Index, m.Length);
                }
            }
            else if (keyData == Keys.Tab)
            {
                Match m = s_NumRegexLtoR.Match(this.Text, this.SelectionStart + this.SelectionLength);
                if (m.Success)
                {
                    handled = true;
                    this.Select(m.Index, m.Length);
                }
            }*/
            return handled || base.ProcessCmdKey(ref m, keyData);
        }

        protected override void WndProc(ref Message m)
        {
            if (m.Msg == Messaging.WM_KILLFOCUS)
            {
                if (!(m_AutoForm.IsDisposed || m.WParam == null
                    || m.WParam == m_AutoForm.Handle || m.WParam == m_AutoForm.MainControl.Handle))
                    m_AutoForm.Hide();
            }
            base.WndProc(ref m);
        }

        protected override void  OnMouseDown(MouseEventArgs e)
        {
            m_AutoForm.Hide();
 	        base.OnMouseDown(e);
        }
        #endregion Overrides

        #region Internal Functions
        internal void HandleKeyDown(KeyEventArgs e)
        {
            Messaging.SendMessage(Handle, Messaging.WM_KEYDOWN, (IntPtr)e.KeyValue, (IntPtr)0);
        }

        internal void HandleKeyPress(KeyPressEventArgs e)
        {
            Messaging.SendMessage(Handle, Messaging.WM_CHAR, (IntPtr)e.KeyChar, (IntPtr)0);
        }

        internal void DoOnClosing(CancelEventArgs e)
        {
            ((Form)this.Parent).Close();
            e.Cancel = true;
        }
        #endregion Internal Functions

        #region Public Functions
        public void HideSuggestions()
        {
            m_AutoForm.Hide();
        }
        #endregion Public Functions
    }
}
