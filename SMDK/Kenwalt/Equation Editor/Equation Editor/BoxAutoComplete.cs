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
        protected FrmAutoComplete m_AutoForm;
        protected int m_nStartChar = -1;
        protected bool m_bCompSelected = false;
        protected bool m_bCompSelection;
        protected List<char> m_DisallowedChars = new List<char>();
        protected ToolTip m_DisallowedTip = new ToolTip();
        #endregion Variables

        #region Constructors
        public BoxAutoComplete()
            : base()
        {
            m_AutoForm = new FrmAutoComplete(this);
            m_AutoForm.TopMost = true;
            m_AutoForm.OwnerBox = this;
            m_DisallowedTip.IsBalloon = true;
        }
        #endregion Constructors

        #region Properties
        public List<char> DisallowedCharaceters
        {
            get { return m_DisallowedChars; }
            set { m_DisallowedChars = value; }
        }

        public ListBox.ObjectCollection Items
        {
            get { return m_AutoForm.MainControl.Items; }
        }

        public Form FrmOwner
        {
            set 
            { 
                m_AutoForm.Owner = value;
                value.Move += new EventHandler(Owner_Move);
            }
        }

        void Owner_Move(object sender, EventArgs e)
        {
            if (m_AutoForm.Visible)
            {
                Point p = this.PointToScreen(this.GetPositionFromCharIndex(m_nStartChar));
                p.Y += (int)this.Font.GetHeight();
                m_AutoForm.Location = p;
            }
        }

        public Dictionary<object, int> HitCounts
        {
            get { return m_AutoForm.MainControl.m_HitCounts; }
            set { m_AutoForm.MainControl.m_HitCounts = value; }
        }

        public bool CompSelected
        {
            get { return m_bCompSelected; }
            protected set
            {
                m_bCompSelected = value;
                if (!m_bCompSelected)
                    this.Cursor = Cursors.IBeam;
                else if (GetSelectionRectangle(null).Contains(this.PointToClient(Cursor.Position)))
                    this.Cursor = Cursors.Default;
            }
        }

        public bool CompSelection
        {
            get { return m_bCompSelection; }
            set
            {
                m_bCompSelection = value;
                if (!m_bCompSelection)
                    CompSelected = false;
            }
        }
        #endregion Properties

        #region Events
        public event EventHandler CompoundDragged;
        public event CancelEventHandler PreCompSelect;
        #endregion Events

        #region Overrides

        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            if (m_DisallowedChars != null)
                foreach (char c in m_DisallowedChars)
                    if (e.KeyChar == c)
                        e.Handled = true;
            //We should display a tooltip in the event of c not being permitted.
            if (e.Handled)
                m_DisallowedTip.Show("The characters \"<>=;:\" are not permitted", this);

            if (!e.Handled)
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
                            m_AutoForm.LastSelect = -1;
                            m_AutoForm.SetFilter(m.Groups["Last"].Value);
                            m_nStartChar = this.SelectionStart;
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
            if (m.Msg == Messaging.WM_LBUTTONDOWN && m_bCompSelected && GetSelectionRectangle(null).Contains(this.PointToClient(Cursor.Position)))
                DoCustomLMouseDown(ref m);
            else
                base.WndProc(ref m);
            //Because we want to draw on top of the normal control:
            if (m.Msg == Messaging.WM_PAINT)
                DrawSelectionFrame();
        }

        protected override void  OnMouseDown(MouseEventArgs e)
        {
            m_AutoForm.Hide();
            base.OnMouseDown(e);
        }

        //Although I guess this should be in another derived class, I like it here.
        protected override void OnDoubleClick(EventArgs e)
        {
            CancelEventArgs ce = new CancelEventArgs(false);
            if (m_bCompSelection)
            {
                if (PreCompSelect != null)
                    PreCompSelect(this, ce);
            }
            base.OnDoubleClick(e);
            if (m_bCompSelection && !ce.Cancel)
            {
                
                //Change the selection to select the entire current specie.
                string[] subs = Text.Split(',', '+');
                Point pos = this.PointToClient(Cursor.Position);
                int mouseLoc = this.GetCharIndexFromPosition(pos);
                int i = 0, selectionLoc = 0;
                while (selectionLoc <= mouseLoc && i < subs.Length)
                    selectionLoc += subs[i++].Length + 1;
                if (i != 0)
                    i--;
                int selectionStart = selectionLoc - subs[i].Length - 1 + (subs[i].Length - subs[i].TrimStart().Length);
                this.Select(selectionStart, subs[i].Trim().Length);
                CompSelected = true;
            }
        }

        protected override void OnSelectionChanged(EventArgs e)
        {
            CompSelected = false;
            base.OnSelectionChanged(e);
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);
            if (CompSelected)
            {
                if (GetSelectionRectangle(null).Contains(e.Location))
                    this.Cursor = Cursors.Default;
                else
                    this.Cursor = Cursors.IBeam;
            }
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

        #region Protected Functions
        protected void DrawSelectionFrame()
        {
            if (CompSelected)
            {
                Graphics gfx = this.CreateGraphics();
                Rectangle outsideRect = GetSelectionRectangle(gfx);
                Rectangle insideRect = outsideRect;
                insideRect.X += 1; insideRect.Y += 1; insideRect.Width -= 2; insideRect.Height -= 2;
                ControlPaint.DrawSelectionFrame(gfx, true, outsideRect, insideRect, Color.Red);
            }
        }

        protected Rectangle GetSelectionRectangle(Graphics gfx)
        {
            if (gfx == null) gfx = this.CreateGraphics();
            Point selectionStart = this.GetPositionFromCharIndex(SelectionStart);
            Point selectionEnd = this.GetPositionFromCharIndex(SelectionStart + SelectionLength);
            SizeF selectionSize = gfx.MeasureString(this.SelectedText, this.Font);
            return new Rectangle(selectionStart.X, selectionStart.Y, selectionEnd.X - selectionStart.X, (int)selectionSize.Height);
        }

        protected void DoCustomLMouseDown(ref Message m)
        {
            if (CompoundDragged != null)
                CompoundDragged(this, new EventArgs());
        }
        #endregion Protected Functions

    }
}
