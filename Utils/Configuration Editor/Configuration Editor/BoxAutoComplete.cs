using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;
using System.Drawing;
using System.ComponentModel;
using Be.Windows.Forms;
using System.Runtime.InteropServices;
using Messaging;

namespace Auto_Complete
{
    internal partial class FrmAutoComplete : Form
    {
        public static Regex s_AutoCompleteRegex = new Regex(
            @"(\s|\+|^|,|\(|\*|-)(?<Last>(\[[A-Za-z0-9()\[\]]+?|[A-Za-z0-9\[\]]+?)(?<Back>[\b]?)$)", 
            RegexOptions.ExplicitCapture | RegexOptions.Compiled);
        public static Regex s_AutoCompleteOpeningRegex = new Regex(@"(\s|\+|^|,|\(|\*|-)(?<Last>[A-Za-z0-9()\[\]]$)", RegexOptions.ExplicitCapture | RegexOptions.Compiled);

        protected int m_nLastSelect;
        protected BoxAutoComplete m_OwnerBox;
        protected bool m_bFocused = false;
        protected int m_nUseIndex = 0;

        private System.Windows.Forms.Panel panel1;
        public Auto_Complete.ListBoxEx MainControl;

        internal FrmAutoComplete(BoxAutoComplete ownerbox)
        {
            InitializeComponent();
            OwnerBox = ownerbox;
            MainControl.SelectedIndexChanged += new EventHandler(MainControl_SelectedIndexChanged);
            MainControl.DoubleClick += new EventHandler(MainControl_DoubleClick);
        }

        private void InitializeComponent()
        {
            this.panel1 = new System.Windows.Forms.Panel();
            this.MainControl = new Auto_Complete.ListBoxEx();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.SystemColors.Control;
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.MainControl);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.ForeColor = System.Drawing.SystemColors.ControlText;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.panel1.Size = new System.Drawing.Size(137, 71);
            this.panel1.TabIndex = 0;
            // 
            // MainControl
            // 
            this.MainControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.MainControl.FormattingEnabled = true;
            this.MainControl.Location = new System.Drawing.Point(0, 0);
            this.MainControl.Name = "MainControl";
            this.MainControl.Size = new System.Drawing.Size(135, 69);
            this.MainControl.Sorted = true;
            this.MainControl.TabIndex = 1;
            this.MainControl.Click += new System.EventHandler(this.MainControl_Click);
            // 
            // FrmAutoComplete
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(137, 71);
            this.ControlBox = false;
            this.Controls.Add(this.panel1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.KeyPreview = true;
            this.Name = "FrmAutoComplete";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.Text = "Form1";
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);
        }

        void MainControl_DoubleClick(object sender, EventArgs e)
        {
            InsertText();
        }

        void MainControl_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (MainControl.SelectedIndex != -1)
                LastSelect = MainControl.SelectedIndex;
        }

        public BoxAutoComplete OwnerBox
        {
            set
            {
                m_OwnerBox = value;
                MainControl.m_OwnerBox = value;
            }
            get { return m_OwnerBox; }
        }

        public int LastSelect
        {
            set
            {
                m_nLastSelect = value;
            }
            get { return m_nLastSelect; }
        }

        protected Form m_ReselectForm;
        public Form ReselectForm
        {
            set { m_ReselectForm = value; }
            get { return m_ReselectForm; }
        }

        public FrmAutoComplete()
        {
            InitializeComponent();
        }

        protected override void OnShown(EventArgs e)
        {
            //MainControl.SelectedIndex = -1;
            base.OnShown(e);
        }

        public void SetFilter(string filter)
        {
            MainControl.SelectedIndex = MainControl.FindString(filter);
        }

        public bool HotSelected
        {
            get { return MainControl.SelectedItem != null; }
        }

        public object SelectedValue
        {
            get
            {
                if (MainControl.SelectedItem != null)
                    return MainControl.SelectedItem;
                else if (LastSelect >= 0)
                    return MainControl.Items[LastSelect];
                else return null;
            }
        }

        internal void HandleKey(KeyEventArgs e)
        {
            Messaging.Messaging.SendMessage(MainControl.Handle, Messaging.Messaging.WM_KEYDOWN, (IntPtr)e.KeyValue, (IntPtr)0);
        }

        public void InsertText()
        {
            if (SelectedValue != null)
            {
                Match m = s_AutoCompleteRegex.Match(m_OwnerBox.Text.Substring(0, m_OwnerBox.SelectionStart));
                m_OwnerBox.Select(m.Groups["Last"].Index, m.Groups["Last"].Length);
                m_OwnerBox.SelectedText = SelectedValue.ToString();
                if (MainControl.m_HitCounts != null)
                    MainControl.m_HitCounts[SelectedValue] = m_nUseIndex++;
            }
            this.Hide();
        }

        private void MainControl_Click(object sender, EventArgs e)
        {
            if (m_ReselectForm != null)
                m_ReselectForm.Activate();
        }

        //Doesn't intercept Alt+F4:
        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            return m_OwnerBox.DoProcessCmdKey(ref msg, keyData);
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            m_OwnerBox.DoOnClosing(e);
        }

        protected override void WndProc(ref Message m)
        {
            if (m.Msg == Messaging.Messaging.WM_ACTIVATEAPP && m.WParam == (IntPtr)0)
            {
                this.Hide();
            }
            base.WndProc(ref m);
        }
    }

    internal class ListBoxEx : ListBox
    {
        internal BoxAutoComplete m_OwnerBox;
        internal Dictionary<object, int> m_HitCounts = new Dictionary<object, int>();

        public new int FindString(String s)
        {
            object lastObj = null;

            int exact = base.FindStringExact(s);
            if (exact != ListBox.NoMatches)
                return exact;

            int lastUse = -1;
            foreach (KeyValuePair<object, int> kvp in m_HitCounts)
                if (kvp.Key.ToString().ToLower().StartsWith(s.ToLower()) && kvp.Value > lastUse)
                {
                    lastObj = kvp.Key;
                    lastUse = kvp.Value;
                }
            if (lastObj != null)
                return Items.IndexOf(lastObj);
            else
                return base.FindString(s);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Up || e.KeyCode == Keys.Down)
                base.OnKeyDown(e);
            else
                m_OwnerBox.HandleKeyDown(e);
        }

        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            m_OwnerBox.HandleKeyPress(e);
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            return m_OwnerBox.DoProcessCmdKey(ref msg, keyData);
        }
    }

    class BoxAutoComplete : RichTextBoxEx
    {
        #region Variables
        protected FrmAutoComplete m_AutoForm;
        protected int m_nStartChar = -1;
        protected bool m_bCompSelected = false;
        protected bool m_bCompSelection;
        protected int m_SquareBracketCount = 0;
        protected char m_OpeningChar;
        #endregion Variables

        #region Constructors
        public BoxAutoComplete()
            : base()
        {
            m_AutoForm = new FrmAutoComplete(this);
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
        #endregion Events

        #region Overrides

        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            if (!m_AutoForm.Visible)
            {
                if (char.IsLetter(e.KeyChar) || e.KeyChar == '[')
                {
                    string s = this.Text.Substring(0, this.SelectionStart) + e.KeyChar;
                    Match m = FrmAutoComplete.s_AutoCompleteOpeningRegex.Match(s);
                    if (m.Success)
                    {
                        m_OpeningChar = e.KeyChar;
                        if (e.KeyChar == '[')
                            m_SquareBracketCount = 1;
                        else
                            m_SquareBracketCount = 0;

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

                if (e.KeyChar == '[') m_SquareBracketCount++; //Although this would be possible to include in the regex, it's a bit tough.
                else if (e.KeyChar == ']') m_SquareBracketCount--;

                if (m.Success)
                {
                    if (m_OpeningChar != '[' && e.KeyChar == '(')
                    {
                        if (m_AutoForm.HotSelected)
                            m_AutoForm.InsertText();
                        m_AutoForm.Hide();
                    }
                    else if (m_OpeningChar == '[' && m_SquareBracketCount == 0)
                    {
                        if (m_AutoForm.HotSelected)
                            m_AutoForm.InsertText();
                        m_AutoForm.Hide();
                        e.Handled = true; //The user can automatically insert a variable by typing ']'.
                    }
                    else
                        m_AutoForm.SetFilter(m.Groups["Last"].Value);
                }
                else
                {
                    if (e.KeyChar == ' ')
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
            if (m.Msg == Messaging.Messaging.WM_KILLFOCUS)
            {
                if (!(m_AutoForm.IsDisposed || m.WParam == null
                    || m.WParam == m_AutoForm.Handle || m.WParam == m_AutoForm.MainControl.Handle))
                    m_AutoForm.Hide();
            }
            if (m.Msg == Messaging.Messaging.WM_LBUTTONDOWN && m_bCompSelected && GetSelectionRectangle(null).Contains(this.PointToClient(Cursor.Position)))
                DoCustomLMouseDown(ref m);
            else
                base.WndProc(ref m);
            //Because we want to draw on top of the normal control:
            if (m.Msg == Messaging.Messaging.WM_PAINT)
                DrawSelectionFrame();
        }

        protected override void  OnMouseDown(MouseEventArgs e)
        {
            m_AutoForm.Hide();
            base.OnMouseDown(e);
        }

        //Although I guess this should be in another derived class, I like it here.
#if false
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
#endif

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
            Messaging.Messaging.SendMessage(Handle, Messaging.Messaging.WM_KEYDOWN, (IntPtr)e.KeyValue, (IntPtr)0);
        }

        internal void HandleKeyPress(KeyPressEventArgs e)
        {
            Messaging.Messaging.SendMessage(Handle, Messaging.Messaging.WM_CHAR, (IntPtr)e.KeyChar, (IntPtr)0);
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

namespace Be.Windows.Forms
{
    /// <summary>
    /// RichTextBoxEx is derived from RichTextBox and supports XP Visual Styles.
    /// </summary>
    public class RichTextBoxEx : RichTextBox
    {
        /// <summary>
        /// Contains the size of the visual style borders
        /// </summary>
        NativeMethods.RECT borderRect;


        public RichTextBoxEx()
        {
        }

        /// <summary>
        /// Filter some message we need to draw the border.
        /// </summary>
        protected override void WndProc(ref Message m)
        {
            switch (m.Msg)
            {
                case NativeMethods.WM_NCPAINT: // the border painting is done here.
                    WmNcpaint(ref m);
                    break;
                case NativeMethods.WM_NCCALCSIZE: // the size of the client area is calcuated here.
                    WmNccalcsize(ref m);
                    break;
                case NativeMethods.WM_THEMECHANGED: // Updates styles when the theme is changing.
                    UpdateStyles();
                    break;
                default:
                    base.WndProc(ref m);
                    break;
            }
        }

        /// <summary>
        /// Calculates the size of the window frame and client area of the RichTextBox
        /// </summary>
        void WmNccalcsize(ref Message m)
        {
            // let the richtextbox control draw the scrollbar if necessary.
            base.WndProc(ref m);

            // we visual styles are not enabled and BorderStyle is not Fixed3D then we have nothing more to do.
            if (!this.RenderWithVisualStyles())
                return;

            // contains detailed information about WM_NCCALCSIZE message
            NativeMethods.NCCALCSIZE_PARAMS par = new NativeMethods.NCCALCSIZE_PARAMS();

            // contains the window frame RECT
            NativeMethods.RECT windowRect;

            if (m.WParam == IntPtr.Zero) // LParam points to a RECT struct
            {
                windowRect = (NativeMethods.RECT)Marshal.PtrToStructure(m.LParam, typeof(NativeMethods.RECT));
            }
            else // LParam points to a NCCALCSIZE_PARAMS struct
            {
                par = (NativeMethods.NCCALCSIZE_PARAMS)Marshal.PtrToStructure(m.LParam, typeof(NativeMethods.NCCALCSIZE_PARAMS));
                windowRect = par.rgrc0;
            }

            // contains the client area of the control
            NativeMethods.RECT contentRect;

            // get the DC
            IntPtr hDC = NativeMethods.GetWindowDC(this.Handle);

            // open theme data
            IntPtr hTheme = NativeMethods.OpenThemeData(this.Handle, "EDIT");

            // find out how much space the borders needs
            if (NativeMethods.GetThemeBackgroundContentRect(hTheme, hDC, NativeMethods.EP_EDITTEXT, NativeMethods.ETS_NORMAL
                , ref windowRect
                , out contentRect) == NativeMethods.S_OK)
            {
                // shrink the client area the make more space for containing text.
                contentRect.Inflate(-1, -1);

                // remember the space of the borders
                this.borderRect = new NativeMethods.RECT(contentRect.Left - windowRect.Left
                    , contentRect.Top - windowRect.Top
                    , windowRect.Right - contentRect.Right
                    , windowRect.Bottom - contentRect.Bottom);

                // update LParam of the message with the new client area
                if (m.WParam == IntPtr.Zero)
                {
                    Marshal.StructureToPtr(contentRect, m.LParam, false);
                }
                else
                {
                    par.rgrc0 = contentRect;
                    Marshal.StructureToPtr(par, m.LParam, false);
                }

                // force the control to redraw it큦 client area
                m.Result = new IntPtr(NativeMethods.WVR_REDRAW);
            }

            // release theme data handle
            NativeMethods.CloseThemeData(hTheme);

            // release DC
            NativeMethods.ReleaseDC(this.Handle, hDC);
        }

        /// <summary>
        /// The border painting is done here.
        /// </summary>
        void WmNcpaint(ref Message m)
        {
            base.WndProc(ref m);

            if (!this.RenderWithVisualStyles())
            {
                return;
            }

            /////////////////////////////////////////////////////////////////////////////
            // Get the DC of the window frame and paint the border using uxTheme API큦
            /////////////////////////////////////////////////////////////////////////////

            // set the part id to TextBox
            int partId = NativeMethods.EP_EDITTEXT;

            // set the state id of the current TextBox
            int stateId;
            if (this.Enabled)
                if (this.ReadOnly)
                    stateId = NativeMethods.ETS_READONLY;
                else
                    stateId = NativeMethods.ETS_NORMAL;
            else
                stateId = NativeMethods.ETS_DISABLED;

            // define the windows frame rectangle of the TextBox
            NativeMethods.RECT windowRect;
            NativeMethods.GetWindowRect(this.Handle, out windowRect);
            windowRect.Right -= windowRect.Left; windowRect.Bottom -= windowRect.Top;
            windowRect.Top = windowRect.Left = 0;

            // get the device context of the window frame
            IntPtr hDC = NativeMethods.GetWindowDC(this.Handle);

            // define a rectangle inside the borders and exclude it from the DC
            NativeMethods.RECT clientRect = windowRect;
            clientRect.Left += this.borderRect.Left;
            clientRect.Top += this.borderRect.Top;
            clientRect.Right -= this.borderRect.Right;
            clientRect.Bottom -= this.borderRect.Bottom;
            NativeMethods.ExcludeClipRect(hDC, clientRect.Left, clientRect.Top, clientRect.Right, clientRect.Bottom);

            // open theme data
            IntPtr hTheme = NativeMethods.OpenThemeData(this.Handle, "EDIT");

            // make sure the background is updated when transparent background is used.
            if (NativeMethods.IsThemeBackgroundPartiallyTransparent(hTheme
                , NativeMethods.EP_EDITTEXT, NativeMethods.ETS_NORMAL) != 0)
            {
                NativeMethods.DrawThemeParentBackground(this.Handle, hDC, ref windowRect);
            }

            // draw background
            NativeMethods.DrawThemeBackground(hTheme, hDC, partId, stateId, ref windowRect, IntPtr.Zero);

            // close theme data
            NativeMethods.CloseThemeData(hTheme);

            // release dc
            NativeMethods.ReleaseDC(this.Handle, hDC);

            // we have processed the message so set the result to zero
            m.Result = IntPtr.Zero;
        }

        /// <summary>
        /// Returns true, when visual styles are enabled in this application.
        /// </summary>
        bool VisualStylesEnabled()
        {
            // Check if RenderWithVisualStyles property is available in the Application class (New feature in NET 2.0)
            Type t = typeof(Application);
            System.Reflection.PropertyInfo pi = t.GetProperty("RenderWithVisualStyles");

            if (pi == null)
            {
                // NET 1.1
                OperatingSystem os = System.Environment.OSVersion;
                if (os.Platform == PlatformID.Win32NT && (((os.Version.Major == 5) && (os.Version.Minor >= 1)) || (os.Version.Major > 5)))
                {
                    NativeMethods.DLLVersionInfo version = new NativeMethods.DLLVersionInfo();
                    version.cbSize = Marshal.SizeOf(typeof(NativeMethods.DLLVersionInfo));
                    if (NativeMethods.DllGetVersion(ref version) == 0)
                    {
                        return (version.dwMajorVersion > 5) && NativeMethods.IsThemeActive() && NativeMethods.IsAppThemed();
                    }
                }

                return false;
            }
            else
            {
                // NET 2.0
                bool result = (bool)pi.GetValue(null, null);
                return result;
            }
        }

        /// <summary>
        /// Return true, when this control should render with visual styles.
        /// </summary>
        /// <returns></returns>
        bool RenderWithVisualStyles()
        {
            return (this.BorderStyle == BorderStyle.Fixed3D && this.VisualStylesEnabled());
        }

        /// <summary>
        /// Update the control parameters.
        /// </summary>
        protected override CreateParams CreateParams
        {
            get
            {
                CreateParams p = base.CreateParams;

                // remove the Fixed3D border style
                if (this.RenderWithVisualStyles() && (p.ExStyle & NativeMethods.WS_EX_CLIENTEDGE) == NativeMethods.WS_EX_CLIENTEDGE)
                    p.ExStyle ^= NativeMethods.WS_EX_CLIENTEDGE;

                return p;
            }
        }

    }

    internal sealed class NativeMethods
    {
        static NativeMethods()
        {
        }

        //////////////////////////////////////////////////////////////////
        // api큦 to get device context of the window frame
        //////////////////////////////////////////////////////////////////
        [DllImport("user32.dll")]
        public static extern IntPtr GetWindowDC(IntPtr hWnd);

        [DllImport("user32.dll")]
        public static extern int ReleaseDC(IntPtr hWnd, IntPtr hDC);

        [DllImport("user32.dll")]
        public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);

        [DllImport("gdi32.dll")]
        public static extern int ExcludeClipRect(IntPtr hdc, int nLeftRect, int nTopRect,
            int nRightRect, int nBottomRect);

        //////////////////////////////////////////////////////////////////
        // api큦 for xp visual styles 
        //////////////////////////////////////////////////////////////////
        [StructLayout(LayoutKind.Sequential)]
        public struct DLLVersionInfo
        {
            public int cbSize;
            public int dwMajorVersion;
            public int dwMinorVersion;
            public int dwBuildNumber;
            public int dwPlatformID;
        }

        [DllImport("UxTheme.dll", CharSet = CharSet.Auto)]
        public static extern bool IsAppThemed();

        [DllImport("UxTheme.dll", CharSet = CharSet.Auto)]
        public static extern bool IsThemeActive();

        [DllImport("comctl32.dll", CharSet = CharSet.Auto)]
        public static extern int DllGetVersion(ref DLLVersionInfo version);

        [DllImport("uxtheme.dll", ExactSpelling = true, CharSet = CharSet.Unicode)]
        public static extern IntPtr OpenThemeData(IntPtr hWnd, String classList);

        [DllImport("uxtheme.dll", ExactSpelling = true)]
        public extern static Int32 CloseThemeData(IntPtr hTheme);

        [DllImport("uxtheme", ExactSpelling = true)]
        public extern static Int32 DrawThemeBackground(IntPtr hTheme, IntPtr hdc, int iPartId,
            int iStateId, ref RECT pRect, IntPtr pClipRect);

        [DllImport("uxtheme", ExactSpelling = true)]
        public extern static int IsThemeBackgroundPartiallyTransparent(IntPtr hTheme, int iPartId, int iStateId);

        [DllImport("uxtheme", ExactSpelling = true)]
        public extern static Int32 GetThemeBackgroundContentRect(IntPtr hTheme, IntPtr hdc
            , int iPartId, int iStateId, ref RECT pBoundingRect, out RECT pContentRect);

        [DllImport("uxtheme", ExactSpelling = true)]
        public extern static Int32 DrawThemeParentBackground(IntPtr hWnd, IntPtr hdc, ref RECT pRect);

        [DllImport("uxtheme", ExactSpelling = true)]
        public extern static Int32 DrawThemeBackground(IntPtr hTheme, IntPtr hdc, int iPartId,
            int iStateId, ref RECT pRect, ref RECT pClipRect);

        public const int S_OK = 0x0;

        public const int EP_EDITTEXT = 1;
        public const int ETS_DISABLED = 4;
        public const int ETS_NORMAL = 1;
        public const int ETS_READONLY = 6;

        public const int WM_THEMECHANGED = 0x031A;
        public const int WM_NCPAINT = 0x85;
        public const int WM_NCCALCSIZE = 0x83;

        public const int WS_EX_CLIENTEDGE = 0x200;
        public const int WVR_HREDRAW = 0x100;
        public const int WVR_VREDRAW = 0x200;
        public const int WVR_REDRAW = (WVR_HREDRAW | WVR_VREDRAW);

        [StructLayout(LayoutKind.Sequential)]
        public struct NCCALCSIZE_PARAMS
        {
            public RECT rgrc0, rgrc1, rgrc2;
            public IntPtr lppos;
        }

        [Serializable, StructLayout(LayoutKind.Sequential)]
        public struct RECT
        {
            public int Left;
            public int Top;
            public int Right;
            public int Bottom;

            public RECT(int left_, int top_, int right_, int bottom_)
            {
                Left = left_;
                Top = top_;
                Right = right_;
                Bottom = bottom_;
            }

            public int Height { get { return Bottom - Top + 1; } }
            public int Width { get { return Right - Left + 1; } }
            public Size Size { get { return new Size(Width, Height); } }

            public Point Location { get { return new Point(Left, Top); } }

            // Handy method for converting to a System.Drawing.Rectangle
            public Rectangle ToRectangle()
            { return Rectangle.FromLTRB(Left, Top, Right, Bottom); }

            public static RECT FromRectangle(Rectangle rectangle)
            {
                return new RECT(rectangle.Left, rectangle.Top, rectangle.Right, rectangle.Bottom);
            }

            public void Inflate(int width, int height)
            {
                this.Left -= width;
                this.Top -= height;
                this.Right += width;
                this.Bottom += height;
            }

            public override int GetHashCode()
            {
                return Left ^ ((Top << 13) | (Top >> 0x13))
                    ^ ((Width << 0x1a) | (Width >> 6))
                    ^ ((Height << 7) | (Height >> 0x19));
            }

            #region Operator overloads

            public static implicit operator Rectangle(RECT rect)
            {
                return Rectangle.FromLTRB(rect.Left, rect.Top, rect.Right, rect.Bottom);
            }

            public static implicit operator RECT(Rectangle rect)
            {
                return new RECT(rect.Left, rect.Top, rect.Right, rect.Bottom);
            }

            #endregion
        }
    }

}

